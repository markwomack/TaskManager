//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows how the task manager can be
// started and stopped from within the code (ie 'manually').

#include <DebugMsgs.h>

#include "TaskManager.h"
#include "Task.h"
#include "BlinkTask.h"

// This is a simple task to maintain a counter and print
// out its value.
class CounterTask : public Task {
  public:    
    CounterTask() {};
    
    void setup(void) {
      // initialize the counter
      counter = 0;
    };

    void update(void) {

      // Increment the counter
      counter++;
      
      // Print out the current time and the new value
      DebugMsgs.debug().print("Counter: ").println(counter);
    };
  
  private:
    unsigned long counter;
};
CounterTask counterTask;

// This is a task that will stop the task manager 20
// seconds after the task is started.
class TaskManagerStopperTask : public Task {
  public:

    TaskManagerStopperTask() {};
    
    void start(void) {
      millisAtStart = millis();
    };

    void update(void) {
      if ((millis() - millisAtStart)/1000 >= 20) {
        DebugMsgs.debug().println("Callback telling task manager to stop now.");
        taskManager.stop();
      }
    };

  private:
    unsigned long millisAtStart;
};
TaskManagerStopperTask taskManagerStopperTask;

// This is a provided task that will blink the LED.
BlinkTask blinkTask;

void setup() {
  Serial.begin(9600);

  // This will allow the printing of debug messages from the task manager
  DebugMsgs.enableLevel(DEBUG);
  
  // Blink the builtin LED every half second.
  taskManager.addTask(&blinkTask, 500);

  // Count and print every second.
  taskManager.addTask(&counterTask, 1000);

  // Check to see if task manager should be stopped (at 20 seconds),
  // check every 10th of a second. 
  taskManager.addTask(&taskManagerStopperTask, 100);

  // Start the task manager
  taskManager.start();
}

void loop() {
  // If the task manager is not executing, then start it (again).
  // This will only happen when the taskManagerStopperTask
  // has manually stopped the task manager.
  if (!taskManager.isExecuting()) {
    DebugMsgs.debug().println("Task manager is not running! Starting it now.");
    taskManager.start();
  }
  
  // Run the task manager
  taskManager.update();
}