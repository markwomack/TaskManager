//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef TASK_H
#define TASK_H

// This is the base class for all tasks implemented to
// execute in the task manager. It has all of the methods
// that a new task implementation can override with its
// own versions. All of the methods are not required to
// be overridden, only the ones needed for the tasks
// implementation.
//
// The call sequence for a task is:
//
// setup() - Called when the task is added to the task
//   manager. It is not called again once the task is added.
// start() - Called every time the task manager is started.
//   This allows for a task to reset any local members each
//   time the task manager is started. A task manager may be
//   started and stopped multiple times programmatically or
//   if a button is being monitored.
// stop() - Called every time the task manager is stopped.
//   This allows for a task to cleanup any local members each
//   time the task manager is stopped. A task manager may be
//   started and stopped multiple times programmatically or
//   if a button is being monitored.
// update() - Called when the task manager update() method
//  has been called and the time for the task to be executed
//  has been reached. A periodInMillis value was given when
//  the task was added, and the update() method will be called
//  each time that time has elapsed.
//
class Task {
  public:
    Task() {};
  
    // Called when the task is added to the task manager.
    virtual void setup(void) {
      // base version does nothing
    };
    
    // Called every time the task manager is started.
    virtual void start(void) {
      // base version does nothing
    };
  
    // Called when the task manager update() method is
    // called and the periodInMillis given when the task
    // was added has elapsed.
    virtual void update(void) {
      // base version does nothing
    };
  
    // Called every time the task manager is stopped.
    virtual void stop(void) {
      // base version does nothing
    };
};

#endif // TASK_H
