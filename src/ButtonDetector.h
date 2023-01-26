//
// Licensed under Apache 2.0 license.
// See accompanying LICENSE file for details.
//

#ifndef BUTTONDETECTOR_H
#define BUTTONDETECTOR_H

const uint32_t DEBOUNCE_DELAY(50);    // the debounce time; increase if the output flickers

// This class is used by the task manager to monitor a
// momentary button on a given pin. It is adapted from a public
// domain Arduino example that can be found here:
//   https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce
//
class ButtonDetector {
  public:
    ButtonDetector() { _isSetup = false; };
    
    void setup(uint8_t buttonPin, uint8_t defaultButtonState) {
      _buttonPin = buttonPin;
      _defaultButtonState = defaultButtonState;
      pinMode(_buttonPin, INPUT);
      
      _buttonState = _defaultButtonState;
      _lastDebounceTime = 0;
      _isSetup = true;
    };

    // Returns true if the button has been pressed, false
    // at all other times, even button release.
    bool buttonPressed() {
      // If not setup, exit now.
      if (!_isSetup) {
        return false;
      }

      bool buttonPressed = false;
      
      // read the state of the button into a local variable:
      uint8_t reading = digitalRead(_buttonPin);

      // If the button changed, due to noise or pressing:
      if (reading != _lastButtonState) {
        // reset the debouncing timer
        _lastDebounceTime = millis();
      }

      if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != _buttonState) {
          _buttonState = reading;

          // only return true if the button was pressed
          if (_buttonState == !_defaultButtonState) {
            buttonPressed = true;
          }
        }
      }

      // save the reading. Next time through the loop, it'll be the lastButtonState:
      _lastButtonState = reading;

      return buttonPressed;
    };

  private:
    bool _isSetup;
    uint8_t _buttonPin;
    uint8_t _defaultButtonState;

    uint8_t _buttonState;
    uint8_t _lastButtonState;
    uint32_t _lastDebounceTime = 0;  // the last time the output pin was toggled
};

#endif // BUTTONDETECTOR_H
