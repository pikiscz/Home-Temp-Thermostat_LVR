#ifndef BUTTONCLASS_H
#define BUTTONCLASS_H

#include <Arduino.h>

#define COUNT_NONE      0
#define COUNT_FALLING   1
#define COUNT_RISING    2
#define COUNT_BOTH      3

class ButtonClass
{
private:
    int _buttonPin;
    unsigned long _debounceTime;
    unsigned long _lastDebounceTime;
    unsigned long _longPressTime;
    unsigned long _lastLongPressTime;
    unsigned long _count;
    int _countMode;
    int _pressedState;
    int _unpressedState;
    bool _longPress;

    int _previousSteadyState;
    int _lastSteadyState;
    int _lastFlickerableState;

public:
    ButtonClass(int buttonPin);
    //Pull-up mode: INPUT_PULLUP, INPUT_PULLDOWN
    //Count mode: COUNT_FALLING, COUNT_RISING, COUNT_BOTH
    //Debounce time in ms. Long press time in ms.
    ButtonClass(
        int buttonPin,
        int pullUpMode,
        int countMode,
        unsigned long debounceTime,
        unsigned long longPressTime
    );
    ~ButtonClass();
    //Set debounce time in ms
    inline void setDebounceTime(unsigned long time) { _debounceTime = time; }
    //Set long press time in ms
    inline void setLongPressTime(unsigned long time) { _longPressTime = time; }
    //Return last steady state after debouncing
    inline int getState() { return _lastSteadyState; }
    //Return actual state
    inline int getStateRaw() { return digitalRead(_buttonPin); }
    
    //Return TRUE when button is steady pressed, otherwise return FALSE
    bool isPressed();
    //Return TRUE when button is steady released, otherwise return FALSE
    bool isReleased();
    //Return TRUE when button is hold longer than setLongPressTime
    //Remains TRUE unitl another button press
    //resetLongPress() sets back to FALSE
    inline bool isLongPress() { return _longPress; }
    //Reset isLongPress() back to FALSE
    inline void resetLongPress() { _longPress = false; }

    //Count mode: COUNT_NONE, COUNT_FALLING, COUNT_RISING, COUNT_BOTH
    inline void setCoutMode(int mode) { _countMode = mode; }
    //Return count of button presses/releases
    inline unsigned long getCount() { return _count; }
    //Reset press/release counter to 0
    inline void resetCount() { _count = 0; }
    //Loop function
    void loop(unsigned long now);
};

#endif