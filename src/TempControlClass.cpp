#include "TempControlClass.h"

TempControlClass::TempControlClass(
    Sht40Class* sht40,
    unsigned long sensorInterval,
    float tempSetMin,
    float tempSetMax,
    int defalutRoom
) : TempControlClass(sht40, sensorInterval, tempSetMin, tempSetMax, defalutRoom, -1 ) {}

TempControlClass::TempControlClass(
    Sht40Class* sht40,
    unsigned long sensorInterval,
    float tempSetMin,
    float tempSetMax,
    int defaultRoom,
    int relayPin
)
{
    _sht40 = sht40;
    _sensorInterval = sensorInterval;
    _lastSensorData = millis() - _sensorInterval + 2000;

    _tempSetMin = tempSetMin * 10;
    _tempSetMax = tempSetMax * 10;

    _defaultRoom = defaultRoom;
    _tempSet[_defaultRoom] = _tempSetMin;

    if(relayPin >= 0)
        pinMode(relayPin, OUTPUT);
}

TempControlClass::~TempControlClass() {}

void TempControlClass::getSensorData(unsigned long now)
{
    if(now - _lastSensorData > _sensorInterval)
    {
        _lastSensorData = now;
        _sht40->getData();

        _humidity[_defaultRoom] = _sht40->getHumidity();
        _tempAct[_defaultRoom] = _sht40->getTemperatureInt();

        if(_relays[_defaultRoom] == 1)
        {
            if(_relayTempCompOn <= 20)
            {
                int seconds = (now - _relayTimeOn) / 1000;
                _relayTempCompOn = _relayTempCompOff + (seconds * _relayTempCoefOn / 10000);
            }
            _tempAct[_defaultRoom] -= _relayTempCompOn;
        }

        if(_relays[_defaultRoom] == 0 && _relayTempCompOff > 0)
        {
            int seconds = (now - _relayTimeOff) / 1000;
            _relayTempCompOff = _relayTempCompOn - (seconds * _relayTempCoefOff / 10000);
            _tempAct[_defaultRoom] -= _relayTempCompOff;
        }

        #ifdef DEBUG_MODE
        Serial.println();
        Serial.print("Humidity: ");
        Serial.print(getHumidity(_defaultRoom)); Serial.println("% rH");
        Serial.print("Temperature: ");
        Serial.print(getTempAct(_defaultRoom)); Serial.println("°C");
        Serial.print("Temp Calibration: ");
        Serial.println(_sht40->getTempCalibration());
        Serial.print("Relay Temp Comp On:  ");
        Serial.println(getRelayTempCompOn());
        Serial.print("Relay Temp Comp Off: ");
        Serial.println(getRelayTempCompOff());
        #endif
    }
}

void TempControlClass::tempControl(unsigned long now)
{
    if(now - _lastTempControlEvent > _tempControlInterval)
    {
        _lastTempControlEvent = now;

        if(_tempAct[_defaultRoom] <= (_tempSet[_defaultRoom] - _controlSensMinus))
        {
            if(((_previousTemp1 - _tempAct[_defaultRoom]) <= _tempDifference) ||
                ((_previousTemp2 - _tempAct[_defaultRoom]) <= _tempDifference))
            {
                if(_relays[_defaultRoom] == 0)
                {
                    digitalWrite(_relayPin, HIGH);
                    _relays[_defaultRoom] = 1;
                    _relayTimeOn = now;
                    _relayTempCompOn = _relayTempCompOff;
                }
            }
        }

        if(_tempAct[_defaultRoom] >= (_tempSet[_defaultRoom] + _controlSensPlus))
        {
            if(_relays[_defaultRoom] == 1)
            {
                digitalWrite(_relayPin, LOW);
                _relays[_defaultRoom] = 0;
                _relayTimeOff = now;
                _relayTempCompOff = _relayTempCompOn;
            }
        }
        _previousTemp2 = _previousTemp1;
        _previousTemp1 = _tempAct[_defaultRoom];

        #ifdef DEBUG_MODE
        Serial.println("Temp-2\tTemp-1\tTemp0:");
        String str =
            String(_previousTemp2 / 10) + "°C\t"
            + String(_previousTemp1 / 10) + "°C\t"
            + String(_tempAct[_defaultRoom] / 10) + "°C";
        Serial.println(str);
        Serial.print("Relay: ");
        Serial.println(getRelays(_defaultRoom));
        #endif
    } 
}