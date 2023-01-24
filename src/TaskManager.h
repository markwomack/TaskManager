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

const int MAX_TASKS(10);

class TaskManager {
  public:
    TaskManager();
    int8_t addTask(Task* task, uint32_t periodInMillis);
    int8_t addBlinkTask(uint8_t ledPin, uint32_t periodInMillis);
    int8_t addBlinkTask(uint32_t periodInMillis = 1000);
    int8_t addIdleTask(Task* task, uint32_t periodInMillis);
    int8_t changeTaskPeriod(int8_t taskIdentifier, uint32_t newPeriodInMillis);
    int8_t removeTask(int8_t taskIndex);
    void removeAllTasks(void);
    bool isExecuting(void);
    void start(void);
    void startMonitoringButton(uint8_t buttonPin, uint8_t defaultButtonState);
    void update(void);
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

    TaskEvent _idleTaskEvent;
    TaskEvent _taskEvents[MAX_TASKS];
    bool _isExecuting;
    uint8_t _nextIndex;   
    
    BlinkTask _builtinBlinkTask;
    ButtonDetector _buttonDetector;
    bool startTask(TaskEvent* taskEvent);
    bool executeTask(TaskEvent* taskEvent);
    int8_t findFreeSlot(void);
    void emptyTaskEvent(TaskEvent* taskEvent);
};

extern TaskManager taskManager;

#endif // TASKMANAGER_H