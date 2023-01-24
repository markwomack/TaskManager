//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows the dynamic removing and adding
// callbacks while the task manager is executing.

#include <DebugMsgs.h>

#include "TaskManager.h"
#include "Task.h"
#include "BlinkTask.h"

struct SharedContext {
  unsigned long counter;
  int8_t taskIdentifier;
};
SharedContext sharedContext;

class Counter1Task : public Task {
  public:
    void setSharedContext(SharedContext* sharedContext) {
      _sharedContext = sharedContext;
    };

    void setNextTask(Task* nextTask) {
      _nextTask = nextTask;
    };

    void start(void) {
      // This counter always starts at zero
      _sharedContext->counter = 0;
    };

    void update(void) {
      
      // Increment the counter
      _sharedContext->counter++;

      // Print out the current time and the new value
      DebugMsgs.debug().print("Counter1: ").println(_sharedContext->counter);

      // If the counter is greater or equal to 10, remove this task
      // and register a new task.
      if (_sharedContext->counter >= 10) {
        int8_t newTaskIdentifer;
        
        // register the new task, execute it every half second
        newTaskIdentifer = taskManager.addTask(_nextTask, 500);

        // remove this task by its identifier
        taskManager.removeTask(_sharedContext->taskIdentifier);

        // remember the new tasks identifier
        _sharedContext->taskIdentifier = newTaskIdentifer;
      }
    };

  private:
    SharedContext* _sharedContext;
    Task* _nextTask;
};
Counter1Task counter1Task;

class Counter2Task : public Task {
  public:
    void setSharedContext(SharedContext* sharedContext) {
      _sharedContext = sharedContext;
    };

    void setNextTask(Task* nextTask) {
      _nextTask = nextTask;
    };

    void update(void) {
      
      // Increment the counter
      _sharedContext->counter += 2;

      // Print out the current time and the new value
      DebugMsgs.debug().print("Counter2: ").println(_sharedContext->counter);

      // If the counter is greater or equal to 20, remove this task
      // and register a new task.
      if (_sharedContext->counter >= 20) {
        int8_t newTaskIdentifer;
        
        // register the new task, execute it every second
        newTaskIdentifer = taskManager.addTask(_nextTask, 1000);

        // remove this task by its identifier
        taskManager.removeTask(_sharedContext->taskIdentifier);

        // remember the new tasks identifier
        _sharedContext->taskIdentifier = newTaskIdentifer;
      }
    };

  private:
    SharedContext* _sharedContext;
    Task* _nextTask;
};
Counter2Task counter2Task;

BlinkTask blinkTask;

int8_t callbackId;

void setup() {
  Serial.begin(9600);

  // This will allow the printing of debug messages
  DebugMsgs.enableLevel(DEBUG);

  // Tasks reference the shared context and each other
  counter1Task.setSharedContext(&sharedContext);
  counter1Task.setNextTask(&counter2Task);
  counter2Task.setSharedContext(&sharedContext);
  counter2Task.setNextTask(&counter1Task);      
  
  // Add a blink task to blink every half second
  taskManager.addTask(&blinkTask, 500);

  // Add the first counter task, and set its identifier into the shared context
  sharedContext.taskIdentifier = taskManager.addTask(&counter1Task, 1000);
  
  // Start the task manager
  taskManager.start();
}

void loop() {
  // Run the task manager
  taskManager.update();
}
