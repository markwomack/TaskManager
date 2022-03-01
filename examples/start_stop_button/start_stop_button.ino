//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows how the task manager can be set to
// monitor a physical push button, and when the button is
// pushed the task manager will start and when pushed again
// the task manager will stop.

#include "TaskManager.h"
#include <DebugMsgs.h>

// Change this value if the builtin led pin is different
const int LED_BUILTIN_PIN(13);

// This is a pin that is connected to a momentary push button
// that is pulled LOW (see README for details)
const int BUTTON_PIN(1);

struct LEDContext {
  char ledState;
};
LEDContext ledContext;

struct CountContext {
  unsigned long counter;
};
CountContext countContext;

TaskManager taskManager;

void setup() {
  Serial.begin(9600);

  // This will allow the printing of debug messages from the task manager
  DebugMsgs.enableLevel(DEBUG);
  
  // Set up the task manager with lifetime callbacks and the button info
  // Button can default to LOW or HIGH, but in this example it is pulled LOW
  taskManager.setup(BUTTON_PIN, LOW, &sketchSetupCallback, &sketchStartCallback,
    &sketchStopCallback, &sketchIdleCallback);
}

void loop() {
  // Run the task manager
  taskManager.loop();
}

// This method is called once to set things up.
void sketchSetupCallback() {
  pinMode(LED_BUILTIN_PIN, OUTPUT);
}

// This method is called whenever the task manager is started,
// which in this case is when the button is pushed and the task
// manager is not executing.
void sketchStartCallback() {

  // Initialize the contexts
  ledContext.ledState = LOW;
  digitalWrite(LED_BUILTIN_PIN, LOW);

  countContext.counter = 0;

  // All of these callbacks are unregistered when task manager stops
  // so they need to be registered with every start.
  
  // register a callback to blink the led every half second (500 millis)
  taskManager.callbackEvery(500, ledCallback, (void*)&ledContext);

  // register a callback to increment and print a counter every second (1000 millis)
  taskManager.callbackEvery(1000, countCallback, (void*)&countContext);
}

// This method is called whenever the task manager is stopped, which
// in this case is whenever the button is pushed and the task manager
// is executing.
void sketchStopCallback() {
  // Turn the led off
  ledContext.ledState = LOW;
  digitalWrite(LED_BUILTIN_PIN, LOW);
}

// This method is called whenever the task manager is not running
// and is therefore idle. In this case we are reusing the ledContext
// to blink the led at the idle rate.
void sketchIdleCallback() {
  ledCallback(&ledContext);
}

// This method is registered above with the task manager.
// You should see the built-in led flash on and off at
// the frequency the callback was registered with.
void ledCallback(void* context) {
  // change led state
  char newLedState = ((LEDContext*)context)->ledState == LOW ? HIGH : LOW;
  digitalWrite(LED_BUILTIN_PIN, newLedState);

  // store state for next callback
  ((LEDContext*)context)->ledState = newLedState;
}

// This method is registered above with the task manager.
// You can monitor the output in the Arduino IDE Serial Monitor.
void countCallback(void* context) {
  // Get the expected context
  CountContext* callbackCountContext = (CountContext*)context;

  // Increment the counter
  callbackCountContext->counter++;

  // Print out the current time and the new value
  DebugMsgs.print(millis()).print(" Counter: ").println(callbackCountContext->counter);
}
