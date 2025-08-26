#include "ButtonClass.h"

ButtonClass::ButtonClass(int buttonPin) : ButtonClass(
    buttonPin, INPUT_PULLUP, COUNT_NONE, 50, 2000) {}

ButtonClass::ButtonClass(
    int buttonPin,
    int pullUpMode,
    int countMode,
    unsigned long debounceTime,
    unsigned long longPressTime
)
{
    _buttonPin = buttonPin;
    pinMode(_buttonPin, pullUpMode);

    _countMode = countMode;
    _debounceTime = debounceTime;
    _longPressTime = longPressTime;

    if(pullUpMode == INPUT_PULLDOWN)
    {
        _pressedState = HIGH;
        _unpressedState = LOW;
    }
    else
    {
        _pressedState = LOW;
        _unpressedState = HIGH;
    }

    _previousSteadyState = digitalRead(_buttonPin);
    _lastSteadyState = _previousSteadyState;
    _lastFlickerableState = _previousSteadyState;
    _lastDebounceTime = 0;
}

ButtonClass::~ButtonClass() {}

bool ButtonClass::isPressed()
{
    if(_previousSteadyState == _unpressedState && _lastSteadyState == _pressedState)
        return true;
    else
        return false;
}

bool ButtonClass::isReleased()
{
    if(_previousSteadyState == _pressedState && _lastSteadyState == _unpressedState)
        return true;
    else
        return false;
}

void ButtonClass::loop()
{
    unsigned long now = millis();
    
    int currentState = digitalRead(_buttonPin);

    if(currentState != _lastFlickerableState)
    {
        _lastDebounceTime = now;
        _lastFlickerableState = currentState;
    }

    if(now - _lastDebounceTime > _debounceTime)
    {
        _previousSteadyState = _lastSteadyState;
        _lastSteadyState = currentState;
    }

    if(_previousSteadyState != _lastSteadyState)
    {
        if(_previousSteadyState == HIGH && _lastSteadyState == LOW)
        {
            _lastLongPressTime = now;
            if(_longPress)
                _longPress = false;
            
            if(_countMode == COUNT_FALLING || _countMode == COUNT_BOTH)
                _count++;
        }

        if(_previousSteadyState == LOW && _lastSteadyState == HIGH)
        {
            if(now - _lastLongPressTime > _longPressTime)
                _longPress = true;

            if(_countMode == COUNT_RISING || _countMode == COUNT_BOTH)
                _count++;
        }
    }
}