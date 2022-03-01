//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows how the task manager can be
// started and stopped from within the code (ie 'manually').

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

unsigned long millisAtStart;

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
  // If the task manager is not executing, then start it (again).
  // This will only happen when the callbackThatWillStopTaskManager
  // method has manually stopped the task manager.
  if (!taskManager.isExecuting()) {
    DebugMsgs.println("Task manager is not running! Starting it now.");
    taskManager.start();
  }
  
  // Run the task manager
  taskManager.loop();
}

// This method is called once to set things up.
void sketchSetupCallback() {
  pinMode(LED_BUILTIN_PIN, OUTPUT);
}

// This method is called whenever the task manager is started,
// which in this case is just once.
void sketchStartCallback() {

  // Initialize our contexts
  ledContext.ledState = LOW;
  digitalWrite(LED_BUILTIN_PIN, LOW);

  countContext.counter = 0;

  millisAtStart = millis();

  // All of these callbacks are unregistered when task manager stops
  // so they need to be registered with every start.
  
  // register a callback to blink the led every half second (500 millis)
  taskManager.callbackEvery(500, ledCallback, (void*)&ledContext);

  // register a callback to increment and print a counter every second (1000 millis)
  taskManager.callbackEvery(1000, countCallback, (void*)&countContext);

  // register a callback that will eventually stop the task manager
  // It will check ever 10th of a second.
  taskManager.callbackEvery(100, callbackThatWillStopTaskManager, (void*)0);
}

// This method is called whenever the task manager is stopped.
void sketchStopCallback() {
  // Turn the led off
  ledContext.ledState = LOW;
  digitalWrite(LED_BUILTIN_PIN, LOW);
}

// This method is called whenever the task manager is not running
// and is therefore idle. In this case we are reusing the ledContext
// to blink the led at the idle rate. But since the task manager is
// almost immediately started when it is stopped, don't expect to
// see anything.
void sketchIdleCallback() {
  ledCallback(&ledContext);
}

// This method is registered above with the task manager.
// You should see the built-in led flash on and off at
// the frequency to callback was registered with.
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

// This method is registered above with the task manager.
// After 20 seconds, it will tell the task manager to stop.
void callbackThatWillStopTaskManager(void* context) {
  if ((millis() - millisAtStart)/1000 >= 20) {
    DebugMsgs.print(millis()).println(" Callback telling task manager to stop now.");
    taskManager.stop();
  }
}
