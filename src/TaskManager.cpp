//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#include <Arduino.h>
#include <DebugMsgs.h>

#include "TaskManager.h"
#include "Task.h"

TaskManager::TaskManager() {
  for (int x = 0; x < MAX_TASKS; x++) {
    emptyTaskEvent(&_taskEvents[x]);
  }
  emptyTaskEvent(&_idleTaskEvent);
  _isExecuting = false;
  _nextIndex = 0;
}

int8_t TaskManager::addTask(Task* task, uint32_t periodInMillis) {
  // Find the next free spot in the taskEvents array
  int index = findFreeSlot();
  if (index == -1) {
    return -1;
  }

  // Initialize the taskEvent
  _taskEvents[index].status = ACTIVE;
  _taskEvents[index].task = task;
  _taskEvents[index].periodInMillis = periodInMillis;
  
  // Call the task setup method
  task->setup();
  
  // If the task manager is currently executing, call the
  // start method of the task
  if (_isExecuting) {
    startTask(&_taskEvents[index]);
  }
    
  // Return the index as the task identifier
  return index;
}

int8_t TaskManager::addBlinkTask(uint8_t ledPin, uint32_t periodInMillis) {
    // Set the led pin on builtin
    _builtinBlinkTask.setLedPin(ledPin);
  
    // add the builtin
    return addTask(&_builtinBlinkTask, periodInMillis);    
}

int8_t TaskManager::addBlinkTask(uint32_t periodInMillis) {
  // Add the builtin
  return addTask(&_builtinBlinkTask, periodInMillis);
}

void TaskManager::addIdleTask(Task* task, uint32_t periodInMillis) {
  _idleTaskEvent.status = ACTIVE;
  _idleTaskEvent.task = task;
  _idleTaskEvent.periodInMillis = periodInMillis;
  task->setup();
  startTask(&_idleTaskEvent);
}

int8_t TaskManager::changeTaskPeriod(int8_t taskIdentifier, uint32_t newPeriodInMillis) {
  // If the taskIdentifier is valid, update the periodInMillis value
  if (_taskEvents[taskIdentifier].status == ACTIVE) {
    _taskEvents[taskIdentifier].periodInMillis = newPeriodInMillis;
    return taskIdentifier;
  }
  
  // Return -1 if the taskIdentifier is not valid
  return -1;
}

int8_t TaskManager::removeTask(int8_t taskIdentifier) {
  // If the taskIdentifier is valid, call the stop method of the task
  // if the task manager is running and empty the element of the
  // _taskEvents array
  if (_taskEvents[taskIdentifier].status == ACTIVE) {
    if (_isExecuting) {
      _taskEvents[taskIdentifier].task->stop();
    }
    emptyTaskEvent(&_taskEvents[taskIdentifier]);
    return 0;
  }
  
  // Return -1 if the taskIdentifier is invalid
  return -1;
}

void TaskManager::removeAllTasks(void) {
  for (int x = 0; x < MAX_TASKS; x++) {
    // if the task manager is currently executing, then call the stop method
    // of any registered task
    if (_isExecuting && _taskEvents[x].status == ACTIVE) {
      _taskEvents[x].task->stop();
    }
    
    // empty out the _taskEvents element
    emptyTaskEvent(&_taskEvents[x]);
  } 
}

bool TaskManager::isExecuting(void) {
  return _isExecuting;
}

void TaskManager::start(void) {
  // If already executing, exit early
  if (_isExecuting) {
    return;
  }
  
  // If there is an idle task, call its stop method
  if (_idleTaskEvent.status == ACTIVE) {
    _idleTaskEvent.task->stop();
  }
  
  DebugMsgs.debug().println("*** Starting execution");
  
  // call the start method of all registered tasks
  for (int8_t x = 0; x < MAX_TASKS; x++) {
    startTask(&_taskEvents[x]);
  }
  
  // set the first index to be checked for execution in update() call.
  _nextIndex = 0;
  
  // task manager is now executing
  _isExecuting = true;
}

void TaskManager::startMonitoringButton(uint8_t buttonPin, uint8_t defaultButtonState) {
  // Setup the _buttonDetector to monitor the button
  _buttonDetector.setup(buttonPin, defaultButtonState);
  DebugMsgs.debug().println("*** Ready to start execution");
}

void TaskManager::update(void) {
  // if idle, execute the idle task
  if (!_isExecuting) {
    executeTask(&_idleTaskEvent);
  }

  // If the button was pressed, toggle _isExecuting and call
  // the appropriate start/stop task manager method
  if (_buttonDetector.buttonPressed()) {
    _isExecuting ? stop() : start();
  }

  // If still not executing, exit early
  if (!_isExecuting) {
    return;    
  } 
  
  uint8_t index = _nextIndex;
  bool executed = false;
  
  // find the first task to be executed and execute
  do {
    executed = executeTask(&_taskEvents[index]);
    
    // the next task to be checked for execution
    index = (index + 1) % MAX_TASKS;
  
    // if nothing has been executed and have not
    // looped around to beginning, then keep going
  } while(!executed && index != _nextIndex);
  
  // The next index to start with in next update call:
  // If the loop wrapped around, then increment to the next element
  // for the next update call. Otherwise, use the index returned by
  // the loop.
  _nextIndex = (index == _nextIndex) ? (index + 1) % MAX_TASKS : index;
}

/**
 Stop the excution of the task manager.
 */
void TaskManager::stop(void) {
  // If not executing, exit early
  if (!_isExecuting) {
    return;
  }
  
  DebugMsgs.debug().println("*** Stopping execution");

  // call the stop method of all the registered tasks
  for (int8_t x = 0; x < MAX_TASKS; x++) {
    if (_taskEvents[x].status == ACTIVE) {
      _taskEvents[x].task->stop();
    }      
  }
  
  // stop execution
  _isExecuting = false;

  DebugMsgs.debug().println("*** Ready to start execution");
  
  // Start the idle task
  startTask(&_idleTaskEvent);
}

// If the taskEvent is active, call the start method and
// set the lastExecutingTime. Return true if started,
// false if not.
bool TaskManager::startTask(TaskEvent* taskEvent) {
  if (taskEvent->status == ACTIVE) {
      taskEvent->task->start();
      taskEvent->lastExecutionTime = millis();
      return true;
  }
  return false;
}

// If the taskEvent is active, then execute the task if it is
// time to execute the task. Record the lastExecution time.
// Return true if executed, false if not.
bool TaskManager::executeTask(TaskEvent* taskEvent) {
  if (taskEvent->status == ACTIVE) {
    uint32_t currentMillis = millis();
    if (currentMillis >= taskEvent->lastExecutionTime + taskEvent->periodInMillis) {
      taskEvent->task->update();
      // Make sure this task event is still active (the update could have removed it)
      if (taskEvent->status == ACTIVE) {
        taskEvent->lastExecutionTime = millis();
      }
      return true;
    }
  }
  return false;
}

// Return the index of a free slot in the _taskEvents array or return -1.
int8_t TaskManager::findFreeSlot(void) {
  for (int8_t x = 0; x < MAX_TASKS; x++) {
    if (_taskEvents[x].status == EMPTY) {
      return x;
    }
  }

  return -1;  
}

// Sets a slot in the taskEvents array to EMPTY.
void TaskManager::emptyTaskEvent(TaskEvent* taskEvent) {
    taskEvent->status = EMPTY;
    taskEvent->task = NULL;
    taskEvent->periodInMillis = 0;
    taskEvent->lastExecutionTime = 0;
}

// Global instance of TaskManager
TaskManager taskManager;
