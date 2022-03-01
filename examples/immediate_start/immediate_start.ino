//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This examples shows how to use the task manager
// to manage timed callbacks. The program starts
// the task manager immediately, and it never stops.

#include "TaskManager.h"
#include <DebugMsgs.h>

// Change this value if the builtin led pin is different
const int LED_BUILTIN_PIN(13);

struct LEDContext {
  int ledState;
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
  
  // Set up the task manager with lifetime callbacks 
  taskManager.setup(&sketchSetupCallback, &sketchStartCallback,
    &sketchStopCallback, &sketchIdleCallback);

  // Start the task manager
  taskManager.start();
}

void loop() {
  // Run the task manager
  taskManager.loop();
}

// This method is called once to set things up.
void sketchSetupCallback() {
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  ledContext.ledState = LOW;
  digitalWrite(LED_BUILTIN_PIN, LOW);

  countContext.counter = 0;
}

// This method is called whenever the task manager is started,
// which in this case is just once.
void sketchStartCallback() {
  
  // register a callback to blink the led every quarter second (250 millis)
  taskManager.callbackEvery(250, ledCallback, (void*)&ledContext);

  // register a callback to increment and print a counter every second (1000 millis)
  taskManager.callbackEvery(1000, countCallback, (void*)&countContext);

}

// This method is called whenever the task manager is stopped,
// which in this case is never.
void sketchStopCallback() {
  // Nothing for this callback, never called
}

// This method is called whenever the task manager is not running
// and is therefore idle. In this case it is never called since the
// task manager is started immediately and is never idle.
void sketchIdleCallback() {
  // Nothing for this callback, never called.
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

  // Print out the new value
  DebugMsgs.print(millis()).print(" Counter: ").println(callbackCountContext->counter);
}
