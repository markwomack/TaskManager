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

/**
 Register a task with the task manager, return and identifier for the task.
 -1 will be returned if the task could not be added. The setup method of the
 task will be called, and if the task manager is running, the start method
 of the task will also be called.
 */
int8_t TaskManager::addTask(Task* task, uint32_t periodInMillis) {
  int index = findFreeSlot();
  if (index == -1) {
    return -1;
  }

  _taskEvents[index].status = ACTIVE;
  _taskEvents[index].task = task;
  _taskEvents[index].periodInMillis = periodInMillis;
  task->setup();
  
  if (_isExecuting) {
    startTask(&_taskEvents[index]);
  }
    
  return index;
}

int8_t TaskManager::addBlinkTask(uint8_t ledPin, uint32_t periodInMillis) {
    _builtinBlinkTask.setLedPin(ledPin);
    return addTask(&_builtinBlinkTask, periodInMillis);    
}

/**
 Add a BlinkTask with the given period.
 */
int8_t TaskManager::addBlinkTask(uint32_t periodInMillis) {
  return addTask(&_builtinBlinkTask, periodInMillis);
}

int8_t TaskManager::addIdleTask(Task* task, uint32_t periodInMillis) {
  _idleTaskEvent.status = ACTIVE;
  _idleTaskEvent.task = task;
  _idleTaskEvent.periodInMillis = periodInMillis;
  task->setup();
  startTask(&_idleTaskEvent);
}

/**
 Change the period that the task in executed. Return -1 if the task
 does not exist.
 */
int8_t TaskManager::changeTaskPeriod(int8_t taskIdentifier, uint32_t newPeriodInMillis) {
  if (_taskEvents[taskIdentifier].status == ACTIVE) {
    _taskEvents[taskIdentifier].periodInMillis = newPeriodInMillis;
    return taskIdentifier;
  }
  
  return -1;
}

/**
 Remove a task from the task manager. Return -1 if the task does
 not exist. If the task manager is currently running, call the
 stop method of the task.
 */
int8_t TaskManager::removeTask(int8_t taskIdentifier) {
  if (_taskEvents[taskIdentifier].status == ACTIVE) {
    if (_isExecuting) {
      _taskEvents[taskIdentifier].task->stop();
    }
    emptyTaskEvent(&_taskEvents[taskIdentifier]);
    return taskIdentifier;
  }
  
  return -1;
}

/**
  Remove all of the registered tasks. If the task manager is currently
  running, call the stop method for task.
 */
void TaskManager::removeAllTasks(void) {
  for (int x = 0; x < MAX_TASKS; x++) {
    // if the task manager is currently executing, then call the stop method
    // of any registered task
    if (_isExecuting && _taskEvents[x].status == ACTIVE) {
      _taskEvents[x].task->stop();
    }
    emptyTaskEvent(&_taskEvents[x]);
  } 
}

bool TaskManager::isExecuting(void) {
  return _isExecuting;
}

/**
  Start the exeution of the task manager.
 */
void TaskManager::start(void) {
  if (_isExecuting) {
    return;
  }
  
  if (_idleTaskEvent.status == ACTIVE) {
    _idleTaskEvent.task->stop();
  }
  
  DebugMsgs.debug().println("*** Starting execution");
  
  // call the start method of all registered tasks
  for (int8_t x = 0; x < MAX_TASKS; x++) {
    startTask(&_taskEvents[x]);
  }
  
  // set the first index to be checked for execution and start execution
  _nextIndex = 0;
  _isExecuting = true;
}

void TaskManager::startMonitoringButton(uint8_t buttonPin, uint8_t defaultButtonState) {
  _buttonDetector.setup(buttonPin, defaultButtonState);
  DebugMsgs.debug().println("*** Ready to start execution");
}

/**
 Find the next task that it is time to execute or do nothing
 if it is not time to execute any registered tasks.
 */
void TaskManager::update(void) {
  if (!_isExecuting) {
    executeTask(&_idleTaskEvent);
  }

  if (_buttonDetector.buttonPressed()) {
    _isExecuting ? stop() : start();
  }

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
  
  // if nothing has been executed and have not looped around to beginning, then keep going
  } while(!executed && index != _nextIndex);
  
  // The next index to start with in next update call.
  _nextIndex = index;
}

/**
 Stop the excution of the task manager.
 */
void TaskManager::stop(void) {
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
  
  startTask(&_idleTaskEvent);
}

bool TaskManager::startTask(TaskEvent* taskEvent) {
  if (taskEvent->status == ACTIVE) {
      taskEvent->task->start();
      taskEvent->lastExecutionTime = millis();
      return true;
  }
  return false;
}

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

/**
 Return the index of a free slot in the taskEvents array or return -1
 if no slot is available.
 */
int8_t TaskManager::findFreeSlot(void) {
  for (int8_t x = 0; x < MAX_TASKS; x++) {
    if (_taskEvents[x].status == EMPTY) {
      return x;
    }
  }

  return -1;  
}

/**
 Sets a slot in the taskEvents array to EMPTY.
 */
void TaskManager::emptyTaskEvent(TaskEvent* taskEvent) {
    taskEvent->status = EMPTY;
    taskEvent->task = NULL;
    taskEvent->periodInMillis = 0;
    taskEvent->lastExecutionTime = 0;
}

TaskManager taskManager;