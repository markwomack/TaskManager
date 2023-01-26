//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows how the task manager can be set to
// monitor a physical push button, and when the button is
// pushed the task manager will start and when pushed again
// the task manager will stop.
// Please use the serial monitor to watch its activity.

#include <DebugMsgs.h>

#include "TaskManager.h"
#include "BlinkTask.h"

// This is a pin that is connected to a momentary push button
// that is pulled LOW (see README for details)
const int BUTTON_PIN(2);

// This is a simple task that increments a value and
// then prints its value.
class CounterTask : public Task {
  public:
    void start(void) {
      counter = 0;
    };

    void update(void) {
      // Increment the counter
      counter++;

      // Print out the current value
      DebugMsgs.debug().print("Counter: ").println(counter);
    };
    
  private:
    unsigned long counter;
};
CounterTask counterTask;

// This is a blink task used as the idle task
BlinkTask idleBlinkTask;

void setup() {
  Serial.begin(9600);

  // This will allow the printing of debug messages
  DebugMsgs.enableLevel(DEBUG);

  // Add a blink task to blink during idle, every 10th of a second
  taskManager.addIdleTask(&idleBlinkTask, 100);

  // Add a blink task that will blink every half second
  // This uses the builtin blink task.
  taskManager.addBlinkTask(500);

  // Add the counter task to execute every second
  taskManager.addTask(&counterTask, 1000);

  // Start monitoring a push button for state change. 
  // When the button is pressed the task manager will
  // start, and when it is pressed again the task manager
  // will stop.      
  taskManager.startMonitoringButton(BUTTON_PIN, LOW);
}

void loop() {
  // Run the task manager
  taskManager.update();
}
