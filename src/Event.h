//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
// Original copyright - Simon Monk
//

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Simon Monk
 http://www.simonmonk.org
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef Event_h
#define Event_h

#include <inttypes.h>

#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2

class Event
{

public:
  Event(void);
  void update(void);
  int8_t eventType;
  unsigned long period;
  int repeatCount;
  uint8_t pin;
  uint8_t pinState;
  void (*callback)(void*);
  unsigned long lastEventTime;
  int count;
  void* context;
};

#endif
