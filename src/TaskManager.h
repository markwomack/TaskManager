//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <inttypes.h>
#include "Task.h"
#include "BlinkTask.h"
#include "ButtonDetector.h"

// Maximum number of tasks allowed. If you can change this value, but
// if you are doing more than 10 tasks there may be a lot of contention
// between the tasks. YMMV.
const uint8_t MAX_TASKS(10);

// Maximum number of idle tasks allowed.
const uint8_t MAX_IDLE_TASKS(3);

// This class is used to manage the task manager. Its simplest usage is
// to add tasks to be executed, calling start(), and then calling
// update() to execute the tasks. Please see the accompanying examples
// to see working code of different types of usage.
//
class TaskManager {
  public:
    TaskManager();
  
    // Add a task that will execute every periodInMillis.
    // Returns a task identifer for reference in other methods, or -1 if the
    // task could not be added.
    int8_t addTask(Task* task, uint32_t periodInMillis);
  
    // Adds a BlinkTask using the ledPin that will execute every periodInMillis.
    // Only a single BlinkTask can be added using this method. It is provided as
    // a convenience. This method or the next one should be called only once.
    // If you need multiple led blinkers, add individual instances using addTask().
    // Returns a task identifer for reference in other methods, or -1 if the
    // task could not be added.
    int8_t addBlinkTask(uint8_t ledPin, uint32_t periodInMillis);
  
    // Adds a BlinkTask using the LED_BUILTIN pin that will execute every periodInMillis.
    // Only a single BlinkTask can be added using this method. It is provided as
    // a convenience. This method or the previous one should be called only once.
    // If you need multiple led blinkers, add individual instances using addTask().
    // Returns a task identifer for reference in other methods, or -1 if the
    // task could not be added.
    int8_t addBlinkTask(uint32_t periodInMillis = 1000);
  
    // Adds a task that will only be executed when the task manager is idle, and
    // the task will be executed every periodInMillis. For example, this method could
    // be used to add a BlinkTask that does a fast blink when the task manager is idle.
    int8_t addIdleTask(Task* task, uint32_t periodInMillis);
    int8_t addIdleBlinkTask(uint8_t ledPin, uint32_t periodInMillis);
    int8_t addIdleBlinkTask(uint32_t periodInMillis = 1000);
  
    // Changes the period of task referenced by taskIdentifier, and the task will
    // execute every newPeriodInMillis.
    int8_t changeTaskPeriod(int8_t taskIdentifier, uint32_t newPeriodInMillis);
  
    // Removes the task referenced by taskIdentifier, and the task will not be
    // executed any further. If memory was allocated for the original Task* used
    // when the task was added, this method will not free that memory. It is the
    // responsibility of the original creator to clean up memory.
    int8_t removeTask(int8_t taskIdentifier);
  
    // Removes the idle task referenced by taskIdentifier, and the task will not be
    // executed any further. If memory was allocated for the original Task* used
    // when the task was added, this method will not free that memory. It is the
    // responsibility of the original creator to clean up memory.
    int8_t removeIdleTask(int8_t taskIdentifier);
    
    // Removes all tasks that were previously added. No tasks will be executed
    // after this method is called.
    void removeAllTasks(void);
  
    // Returns true if the task manager is currently executing, false otherwise.
    bool isExecuting(void);
  
    // Starts the task manager. All previously added tasks will begin executing
    // at the frequency of the periodInMillis they were added with when the
    // update() method is periodically called.
    void start(void);
  
    // Starts the task manager and has it monitor a button connected to buttonPin.
    // defaultButtonState indicates the default state of the button (LOW or HIGH).
    // When the button changes to the opposite state (ie it is pressed) , then the
    // task manager will start and begin executing. When the button is pressed
    // again, the task manager will stop. Even though the task manager will be
    // monitoring for a button press, the task manager is not executing until the
    // button is first pressed, and when pressed again it will no longer be
    // executing.
    void startMonitoringButton(uint8_t buttonPin, uint8_t defaultButtonState);
  
    // Checks for the next task to be executed and executes it. This method should
    // be called periodically, typically in the Arduino loop() method. If this
    // method is not called, then no tasks will be executed.
    void update(void);
  
    // Stops the task manager. All added tasks will no longer be executed when
    // the update() method is called. If the task manager was previously started
    // using the startMonitoringButton() method, the button will continue to be
    // monitored as described in that method.
    void stop(void);

  private:
    
    enum TaskEventStatus {
      EMPTY,
      ACTIVE
    };
    
    struct TaskEvent {
        TaskEventStatus status;
        Task* task;
        uint32_t periodInMillis;
        uint32_t lastExecutionTime;
    };

    TaskEvent _idleTaskEvents[MAX_IDLE_TASKS];
    TaskEvent _taskEvents[MAX_TASKS];
    bool _isExecuting;
    uint8_t _nextIndex;   
    
    BlinkTask _builtinBlinkTask;
    BlinkTask _builtinIdleBlinkTask;
    ButtonDetector _buttonDetector;
    bool startTask(TaskEvent* taskEvent);
    void startAllTasks();
    void startAllIdleTasks();
    void stopAllTasks();
    void stopAllIdleTasks();
    uint8_t executeNextTask(uint8_t nextTaskIndex, TaskEvent* taskEvents, uint8_t taskEventsSize);
    bool executeTask(TaskEvent* taskEvent);
    int8_t findFreeSlot(void);
    int8_t findFreeIdleSlot(void);
    void emptyTaskEvent(TaskEvent* taskEvent);
};

// This is the global instance of the task manager that
// can be used in Arduino sketches.
extern TaskManager taskManager;

#endif // TASKMANAGER_H
