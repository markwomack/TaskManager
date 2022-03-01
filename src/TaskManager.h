//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <Arduino.h>
#include <inttypes.h>
#include "Timer.h"

#define CALLBACK_NOT_INSTALLED TIMER_NOT_AN_EVENT

// State stored and sent to the task manager callbacks
struct TaskManagerContext {
  Timer timer;
  int8_t buttonPin;
  char buttonDefaultState;
  char oldButtonState;
  boolean isExecuting;
  void (*sketchStartCallback)();
  void (*sketchStopCallback)();
  void (*idleCallback)();
  int8_t callbackReferences[MAX_NUMBER_OF_EVENTS];
};

/*
 * Use this class to monitor a button that will call start and stop
 * execution when pressed. Callbacks can be registered to execute
 * periodically while the executing.
 */
class TaskManager {

public:
  TaskManager();

  /**
   *  Sets up the task manager with the callbacks that are called at
   *  various points in the lifetime of manager.
   *  
   * sketchSetupCallback - The method to call with initial setup. Only called once.
   * sketchStartCallback - The method called when button pressed to start.
   * sketchStopCallback - The method called when button pressed to stop.
   * idleCallback - The method called when the task manager is not executing.
   **/
  void setup(void (*sketchSetupCallback)(), void (*sketchStartCallback)(),
             void (*sketchStopCallback)(), void (*idleCallback)()=0);
    
  /** 
   *  Sets up the task manager with the callbacks that are called at
   *  various points in the lifetime of manager and the pin to monitor
   *  for state change to start and stop the manager.
   *  
   * buttonPin - The pin to monitor for change in button state.
   * buttonDefaultState - The default state of the button. Button can be pulled HIGH
   *   or LOW.
   * sketchSetupCallback - The method to call with initial setup. Only called once.
   * sketchStartCallback - The method called when button pressed to start.
   * sketchStopCallback - The method called when button pressed to stop.
   * idleCallback - The method called when the task manager is not executing.
   **/
  void setup(int8_t buttonPin, char buttonDefaultState, void (*sketchSetupCallback)(),
             void (*sketchStartCallback)(), void (*sketchStopCallback)(),
             void (*idleCallback)()=0);

  /**
   * Can be called to start execution of the task manager. This is generally used
   * when a buttonPin has not been given in the setup method.
   */
  void start(void);

  /**
   * Returns true if the task manager is currently running.
   */
  bool isExecuting(void);
  
  /*
   * Call this in the loop method of the sketch. As part of its execution, it will
   * make calls to registered callbacks if it is time to call them.
   */
  void loop(void);

  /*
   * Can be called anywhere in the sketch to stop execution of the task manager.
   * The registered sketchStopCallback method will be called. For example, a
   * registered callback may encounter an error condition that requires that
   * everything stop. It would call stopExecution to do this.
   */
  void stop(void);

  /*
   * Use to register callback methods that should be called after the button has
   * been pressed to start.
   * 
   * period - The period that the callback should be called in milliseconds. For
   *   example, a value of 1000 would call the callback every second.
   * callback - The callback method which takes a void* parameter. The void* will
   *   be a pointer to the context as passed in the context parameter.
   * context - A pointer to a struct that will be passed to the callback method
   *   each time it is called. Allows for data to be stored between calls. It is
   *   up to the callback method to convert it to the proper pointer to access
   *   the data.
   *   
   * Returns a callback id that uniquely identifies the registered callback.
   * This value can be passed to callbackStop to remove the callback
   * from execution.
   */
  int8_t callbackEvery(unsigned long period, void (*callback)(void*),
                       void* context);

  /*
   * Will remove the callback that matches the id from future execution.
   * 
   * Returns TIMER_NOT_AN_EVENT if the callbackId was valid and the callback
   * was removed. Returns CALLBACK_NOT_INSTALLED if the callbackId was not
   * valid.
   */
  int8_t removeCallback(int8_t callbackId);
	
protected:
  TaskManagerContext _taskManagerContext;
};

#endif
