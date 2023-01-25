//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef BLINKTASK_H
#define BLINKTASK_H

#include <Arduino.h>
#include "Task.h"

// This is a task that will blink an LED connected to a given pin.
// Blinking is a ubiquitous way to indicate that a sketch is
// running as expected, or has run into trouble. So, it is only
// fitting to have a task implemented to perform this common task.
// The LED will blink at the rate given by the periodInMillis
// when the task is added. If a value of 1000 is given, then the
// LED will blink on or off every second.
//
// If no pin value is given when the BlinkTask is created and
// not set by using setLedPin(), then the value in the Arduino
// define constant LED_BUILTIN is used by default. The builtin
// led pin is board specific, but is usually 13 (as it is on the
// Arduino Uno).
//
// Since using blinking is so common, a convenience method is
// provided in the TaskManager, addBlinkTask, to add a single
// blink task. It is the equivalent of defining a BlinkTask
// instance and then adding it to the task manager with addTask().
//
class BlinkTask : public Task {
  public:
    BlinkTask(uint8_t ledPin = LED_BUILTIN) {
      _ledPin = ledPin;
    };
    
    void setLedPin(uint8_t ledPin) {
      _ledPin = ledPin;
    };
    
    void setup(void) {
      // Setup the pin mode
      pinMode(_ledPin, OUTPUT);
    };

    void start(void) {
      // Start in the off position, update pin
      _state = LOW;
      digitalWrite(_ledPin, _state);
    }
    
    void update(void) {
      // Change state, update pin
      _state = !_state;
      digitalWrite(_ledPin, _state);
    };

    void stop(void) {
      // Stop in the off position, update pin
      _state = LOW;
      digitalWrite(_ledPin, _state);      
    }
    
  private:
    uint8_t _ledPin;
    int _state;
};

#endif // BLINKTASK_H
