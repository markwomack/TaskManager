//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
// Original copyright - Simon Monk
//

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Simon Monk
 http://www.simonmonk.org
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Event.h"

Event::Event(void)
{
    eventType = EVENT_NONE;
}

void Event::update(void)
{
    unsigned long now = millis();
    if (now - lastEventTime >= period)
    {
        switch (eventType)
        {
            case EVENT_EVERY:
                (*callback)(context);
                break;

            case EVENT_OSCILLATE:
                pinState = ! pinState;
                digitalWrite(pin, pinState);
                break;
        }
        lastEventTime = now;
        count++;
    }
    if (repeatCount > -1 && count >= repeatCount)
    {
        eventType = EVENT_NONE;
    }
}
