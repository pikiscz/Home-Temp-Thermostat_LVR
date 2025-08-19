#ifndef BUTTONCLASS_H
#define BUTTONCLASS_H

#include <Arduino.h>

#define COUNT_FALLING 0
#define COUNT_RISING  1
#define COUNT_BOTH    2

class ButtonClass
{
private:
    int _buttonPin;
    unsigned long _debounceTime;
    unsigned long _lastDebounceTime;
    unsigned long _longPressTime;
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

    inline void setDebounceTime(unsigned long time) { _debounceTime = time; }
    inline int getState() { return _lastSteadyState; }
    inline int getStateRaw() { return digitalRead(_buttonPin); }
    
    bool isPressed();
    bool isReleased();
    inline bool isLongPress() { return _longPress; }
    inline void resetLongPress() { _longPress = false; }

    //Count mode: COUNT_FALLING, COUNT_RISING, COUNT_BOTH
    inline void setCoutMode(int mode) { _countMode = mode; }
    inline unsigned long getCount() { return _count; }
    inline void resetCount() { _count = 0; }
    
    void loop(unsigned long now);
};

#endif