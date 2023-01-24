//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef TASK_H
#define TASK_H

class Task {
  public:
    Task() {};
    virtual void setup(void) {
      // base version does nothing
    };
    virtual void start(void) {
      // base version does nothing
    };
    virtual void update(void) {
      // base version does nothing
    };
    virtual void stop(void) {
      // base version does nothing
    };
};

#endif // TASK_H