//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef BLINKTASK_H
#define BLINKTASK_H

#include <Arduino.h>
#include "Task.h"

class BlinkTask : public Task {
  public:
    BlinkTask(uint8_t ledPin = LED_BUILTIN) {
      _ledPin = ledPin;
    };
    
    void setLedPin(uint8_t ledPin) {
      _ledPin = ledPin;
    };
    
    void setup(void) {
      pinMode(_ledPin, OUTPUT);
    };

    void start(void) {
      _state = LOW;
      digitalWrite(_ledPin, _state);
    }
    
    void update(void) {
      _state = !_state;
      digitalWrite(_ledPin, _state);
    };

    void stop(void) {
      _state = LOW;
      digitalWrite(_ledPin, _state);      
    }
    
  private:
    uint8_t _ledPin;
    int _state;
};

#endif // BLINKTASK_H