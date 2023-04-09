//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows the dynamic removing and adding
// callbacks while the task manager is executing.
// Please use the serial monitor to see its activity.

#include <DebugMsgs.h>  // https://github.com/markwomack/ArduinoLogging

#include "TaskManager.h"
#include "Task.h"
#include "BlinkTask.h"

// This is a context shared between the two tasks
struct SharedContext {
  unsigned long counter;
  int8_t taskIdentifier;
};
SharedContext sharedContext;

// This is the first task, it counts to 10 in increments of 1.
// When it reaches 10, it adds the second task and removes
// itself.
class Counter1Task : public Task {
  public:
    void setSharedContext(SharedContext* sharedContext) {
      // reference to the shared context
      _sharedContext = sharedContext;
    };

    void setNextTask(Task* nextTask) {
      // reference to the second task
      _nextTask = nextTask;
    };

    void start(void) {
      // This counter always resets to zero
      _sharedContext->counter = 0;
    };

    void update(void) {
      
      // Increment the counter
      _sharedContext->counter++;

      // Print out the current value
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

// This is the second task, it counts to 20 in increments of 2.
// When it reaches 20, it adds the first task and removes itself.
class Counter2Task : public Task {
  public:
    void setSharedContext(SharedContext* sharedContext) {
      // reference to the shared context
      _sharedContext = sharedContext;
    };

    void setNextTask(Task* nextTask) {
      // reference to the first task
      _nextTask = nextTask;
    };

    void update(void) {
      
      // Increment the counter
      _sharedContext->counter += 2;

      // Print out current value
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

// This is a blink task used to indicate
// the code is running
BlinkTask blinkTask;

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

  // Add the first task, and set its identifier into the shared context
  sharedContext.taskIdentifier = taskManager.addTask(&counter1Task, 1000);
  
  // Start the task manager
  taskManager.start();
}

void loop() {
  // Run the task manager
  taskManager.update();
}
