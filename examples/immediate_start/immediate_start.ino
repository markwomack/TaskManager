//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This examples shows how to use the task manager
// to manage timed callbacks. The program starts
// the task manager immediately, and it never stops.

#include <DebugMsgs.h>

#include "TaskManager.h"
#include "Task.h"
#include "BlinkTask.h"

// This is a simple task that increments a counter
// and print out the value.
class CounterTask : public Task {
  public:
    void setup(void) {
      counter = 0;
    };

    void update(void) {
      // Increment the counter
     counter++;

      // Print out the new value
      DebugMsgs.debug().print("Counter: ").println(counter);
    }

  private:
    unsigned long counter;
};
CounterTask counterTask;

// This is a provided class to handle blinking of an LED.
BlinkTask blinkTask;

void setup() {
  Serial.begin(9600);

  // This will allow the printing of debug messages
  DebugMsgs.enableLevel(DEBUG);

  // Add the blink task to execute every quarter second
  taskManager.addTask(&blinkTask, 250);

  // Add the counter task to execute every second
  taskManager.addTask(&counterTask, 1000);

  // Start the task manager
  taskManager.start();
}

void loop() {
  // Run the task manager
  taskManager.update();
}
