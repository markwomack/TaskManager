//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#include <Arduino.h>
#include <DebugMsgs.h>

#include "TaskManager.h"
#include "Timer.h"

#define MAX_NUMBER_OF_CALLBACKS (MAX_NUMBER_OF_EVENTS - 1)

TaskManager::TaskManager() { /* Nothing to see here, move along.*/ }

// This is a static method that is used to start execution.  It first calls
// the sketchSetupCallback method and then is schedules execution of the execute method.
void localStartExecution(TaskManagerContext* taskManagerContext) {
	// If already executing, then just return
	if (taskManagerContext->isExecuting) {
		return;
	}
	DebugMsgs.println("*** Starting execution");
	(*(taskManagerContext->sketchStartCallback))();
	taskManagerContext->isExecuting = true;
}

// This is a static method that is used to end execution.  It first stops the
// calls to the execute method and then it calls the sketchStopCallback method.
void localStopExecution(TaskManagerContext* taskManagerContext) {
	// If not executing, then just return
	if (!taskManagerContext->isExecuting) {
		return;
	}
	DebugMsgs.println("*** Stopping execution");
	
	// Stop execution of all registered callbacks
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		taskManagerContext->callbackReferences[index] = 
			taskManagerContext->timer.stop(
				taskManagerContext->callbackReferences[index]);
	}
	
	(*(taskManagerContext->sketchStopCallback))();
	taskManagerContext->isExecuting = false;
	DebugMsgs.println("*** Ready to start execution");
}

// This is a static method that checks the state of the start/reset button.  It
// is checked every 10ms to allow for bouncing/noise in the button.
void localCheckButton(void* context) {
	TaskManagerContext* taskManagerContext = (TaskManagerContext*)context;
	int currentButtonState = digitalRead(taskManagerContext->buttonPin);
	if (currentButtonState == !taskManagerContext->buttonDefaultState &&
	      taskManagerContext->oldButtonState == taskManagerContext->buttonDefaultState) {
		if (!taskManagerContext->isExecuting) {
			localStartExecution(taskManagerContext);
		} else {
			localStopExecution(taskManagerContext);
		}
	}
	taskManagerContext->oldButtonState = currentButtonState;
}

// Called to perform idle behavior when the task manager is not executing.
void localDoIdle(void* context) {
  TaskManagerContext* taskManagerContext = (TaskManagerContext*)context;
  if (!taskManagerContext->isExecuting) {
    (*(taskManagerContext->idleCallback))();
  }
}

void TaskManager::setup(void (*sketchSetupCallback)(), void (*sketchStartCallback)(),
    void (*sketchStopCallback)(), void (*idleCallback)()) {
  this->setup(-1, LOW, sketchSetupCallback, sketchStartCallback,
    sketchStopCallback, idleCallback);
}

void TaskManager::setup(int8_t buttonPin, char buttonDefaultState,
    void (*sketchSetupCallback)(), void (*sketchStartCallback)(),
    void (*sketchStopCallback)(), void (*idleCallback)()) {

	DebugMsgs.println("*** Setting up");
			
	_taskManagerContext.buttonPin = buttonPin;
	_taskManagerContext.oldButtonState = buttonDefaultState;
  _taskManagerContext.buttonDefaultState = buttonDefaultState;
	_taskManagerContext.isExecuting = false;
	_taskManagerContext.sketchStartCallback = sketchStartCallback;
	_taskManagerContext.sketchStopCallback = sketchStopCallback;
	_taskManagerContext.idleCallback = idleCallback;
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		_taskManagerContext.callbackReferences[index] = TIMER_NOT_AN_EVENT;
	}
	
	// Call the sketchSetupCallback just once
  (*(sketchSetupCallback))();
    
  // Register internal callback for tracking button pushes if pin registered
  if (_taskManagerContext.buttonPin != -1) {
  	pinMode(_taskManagerContext.buttonPin, INPUT);
  	_taskManagerContext.timer.every(10, localCheckButton, (void*)&_taskManagerContext);
  }

  // Register idle callback if callback registered.
	if (_taskManagerContext.idleCallback) {
	  _taskManagerContext.timer.every(50, localDoIdle, (void*)&_taskManagerContext);
	}
	
	DebugMsgs.println("*** Ready to start execution");
}

void TaskManager::start(void) {
  localStartExecution(&_taskManagerContext);
}

bool TaskManager::isExecuting(void) {
  return _taskManagerContext.isExecuting;
}

// Called by the sketch to execute a loop.
void TaskManager::loop() {
	_taskManagerContext.timer.update();
}

// Can be called by the sketch to stop execution of the task manager.
void TaskManager::stop() {
 DebugMsgs.println("*** Stopping execution by request!");
  localStopExecution(&_taskManagerContext);
}

// Called by the sketch to register a callback that will get executed as part
// of the loop processing.
int8_t TaskManager::callbackEvery(unsigned long period,
		void (*callback)(void*), void* context) {
			
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		if (_taskManagerContext.callbackReferences[index] != TIMER_NOT_AN_EVENT) {
			continue;
		}
		_taskManagerContext.callbackReferences[index] = 
			_taskManagerContext.timer.every(period, callback, context);
		return _taskManagerContext.callbackReferences[index];
	}
	return CALLBACK_NOT_INSTALLED;
}

// Called by the sketch to stop the execution of specific callback with the
// given callback id.
int8_t TaskManager::removeCallback(int8_t callbackId) {
	for(int index = 0; index < MAX_NUMBER_OF_CALLBACKS; index++) {
		if (_taskManagerContext.callbackReferences[index] != callbackId) {
			continue;
		}
		_taskManagerContext.callbackReferences[index] = 
			_taskManagerContext.timer.stop(callbackId);
		return _taskManagerContext.callbackReferences[index];
	}
	return CALLBACK_NOT_INSTALLED;
}
