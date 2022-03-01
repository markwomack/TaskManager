//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

// This example shows the dynamic removing and adding
// callbacks while the task manager is executing.

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

int8_t callbackId;

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

  // All of these callbacks are unregistered when task manager stops
  // so they need to be registered with every start.
  
  // register a callback to blink the led every half second (500 millis)
  taskManager.callbackEvery(500, ledCallback, (void*)&ledContext);

  // register a callback to increment and print a counter every second (1000 millis)
  callbackId = taskManager.callbackEvery(1000, countCallback1, (void*)&countContext);
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
void countCallback1(void* context) {
  // Get the expected context
  CountContext* callbackCountContext = (CountContext*)context;

  // Increment the counter
  callbackCountContext->counter++;

  // Print out the current time and the new value
  DebugMsgs.print(millis()).print(" Counter1: ").println(callbackCountContext->counter);

  // If the counter is greater or equal to 10, remove this callback
  // and register a new callback.
  if (callbackCountContext->counter >= 10) {
    // remove the callback by its id
    taskManager.removeCallback(callbackId);

    // register the new callback, and use the same context
    callbackId = taskManager.callbackEvery(500, countCallback2, context);
  }
}

// This method is registered above with the task manager.
// You can monitor the output in the Arduino IDE Serial Monitor.
void countCallback2(void* context) {
  // Get the expected context
  CountContext* callbackCountContext = (CountContext*)context;

  // Increment the counter by 2
  callbackCountContext->counter += 2;

  // Print out the current time and the new value
  DebugMsgs.print(millis()).print(" Counter2: ").println(callbackCountContext->counter);

  // If the counter is greater or equal to 20, remove this callback
  // and register a new callback.
  if (callbackCountContext->counter >= 20) {
    
    // reset the counter
    callbackCountContext->counter = 0;
    
    // remove the callback by its id
    taskManager.removeCallback(callbackId);

    // register the new callback, and use the same context
    callbackId = taskManager.callbackEvery(1000, countCallback1, context);
  }
}
