#ifndef TEMPCONTROLCLASS_H
#define TEMPCONTROLCLASS_H

#include <Arduino.h>
#include "Sht40Class.h"

#define DEBUG_MODE

class TempControlClass
{
private:
    Sht40Class* _sht40;

    unsigned long _sensorInterval;
    unsigned long _lastSensorData;
    unsigned long _tempControlInterval;
    unsigned long _lastTempControlEvent;
    int _tempSetMin;    //In degC multiplied by 10
    int _tempSetMax;    //In degC multiplied by 10

    int _controlSensMinus;   //In degC multiplied by 10
    int _controlSensPlus;    //In degC multiplied by 10

    int _previousTemp1;     //In degC multiplied by 10
    int _previousTemp2;     //In degC multiplied by 10
    int _tempDifference;    //In degC multiplied by 10

    unsigned long _relayTimeOn;
    unsigned long _relayTimeOff;
    int _relayTempCompOn;   //In degC multiplied by 10
    int _relayTempCompOff;  //In degC multiplied by 10
    float _relayTempCoefOn;   // 100* INT(10*degC)/seconds
    float _relayTempCoefOff;  // 100* INT(10*degC)/seconds

    int _humidity[5];
    int _tempAct[5];  //In degC multiplied by 10
    int _tempSet[5];  //In degC multiplied by 10
    int _relays[5];
    bool _heatingEnabled;
    int _signalRC;

    int _defaultRoom;
    int _relayPin;

public:
    TempControlClass(
        Sht40Class* sht40,
        unsigned long sensorInterval,
        float tempSetMin,
        float tempSetMax,
        int defaultRoom
    );
    TempControlClass(
        Sht40Class* sht40,
        unsigned long sensorInterval,
        float tempSetMin,
        float tempSetMax,
        int defaultRoom,
        int relayPin
    );
    ~TempControlClass();

    //Return temperature control sensitivity - minus
    inline float getTempControlSensMinus() {return float(_controlSensMinus) / 10; }
    //Set temperature control sensitivity - minus
    inline void setTempControlSensMinus(float sensMinus) {_controlSensMinus = sensMinus * 10; }

    //Return temperature control sensitivity - plus
    inline float getTempControlSensPlus() {return float(_controlSensPlus) / 10; }
    //Set temperature control sensitivity - plus
    inline void setTempControlSensPlus(float sensPlus) {_controlSensPlus = sensPlus * 10; }

    //Return temperature difference
    //If last three measured temperatures have difference greater than this value
    //it is assumed that windows are open so relay is not allowed to switch on
    inline float getTempControlDifference() { return float(_tempDifference) / 10; }
    //Set temperature difference
    //If last three measured temperatures have difference greater than this value
    //it is assumed that windows are open so relay is not allowed to switch on
    inline void setTempControlDifference(float tempDiff) { _tempDifference = tempDiff * 10; }

    //Return temperature compensation coefficient of relay when is switched on
    inline float getRelayTempCoefOn() { return float(_relayTempCoefOn) / 100; }
    //Set temperature compensation coeficient of relay when is switched on
    inline void setRelayTempCoefOn(float tempCoef) { _relayTempCoefOn = tempCoef * 100; }

    //Return temperature compensation coefficient of relay when is switched off
    inline float getRelayTempCoefOff() { return float(_relayTempCoefOff) / 100; }
    //Set temperature compensation coeficient of relay when is switched off
    inline void setRelayTempCoefOff(float tempCoef) { _relayTempCoefOff = tempCoef * 100; }

    //Return temperature compensation of switched on relay in degC
    inline float getRelayTempCompOn() { return float(_relayTempCompOn) / 10; }
    //Return temperature compensation of switched off relay in degC
    inline float getRelayTempCompOff() { return float(_relayTempCompOff) / 10; }

    //Return tempereture control interval in milliseconds
    inline unsigned long getTempControlInterval() { return _tempControlInterval; }
    //Set temperature control interval in milliseconds
    inline void setTempControlInterval(unsigned long time) { _tempControlInterval = time; }

    //Return termperature control interval in minutes
    inline int getTempControlIntervalMinutes() { return _tempControlInterval / 60000; }
    //Set termperature control interval in minutes
    inline void setTempControlIntervalMinutes(int time) { _tempControlInterval = time * 60000; }

    //Return minimal value of set temperature
    inline float getTempSetMin() { return float(_tempSetMin) / 10; }
    //Set minimal value of set temperature
    inline void setTempSetMin(float tempSetMin) { _tempSetMin = tempSetMin * 10; }

    //Return maximal value of set temperature
    inline float getTempSetMax() { return float(_tempSetMax) / 10; }
    //Set minimal value of set temperature
    inline void setTempSetMax(float tempSetMax) { _tempSetMax = tempSetMax * 10; }

    //Return actual humidity of selected room
    inline int getHumidity(int room) { return _humidity[room]; }
    //Set actual humidity for selected room
    inline void setHumidity(int humidity, int room) { _humidity[room] = humidity;}

    //Return actual temperature of selected room
    inline float getTempAct(int room) { return float(_tempAct[room]) / 10; }
    //Set actual temperature for selected room
    inline void setTempAct(float tempAct, int room) { _tempAct[room] = tempAct * 10; }

    //Return required temperature of selected room
    inline float getTempSet(int room) { return float(_tempSet[room]) / 10; }
    //Set required temperature for selected room
    inline void setTempSet(float tempSet, int room) { _tempSet[room] = tempSet * 10; }

    //Return relay state of selected room
    inline int getRelays(int room) { return _relays[room]; }
    //Set relay state for selected room
    inline void setRelays(int relay, int room) { _relays[room] = relay; }

    //Return TRUE when heating is enabled, otherwise return FALSE
    inline bool getHeatingEnabled() { return _heatingEnabled; }
    //Set TRUE for heating is enabled or FALSE when disabled
    inline void setHeatingEnabled(bool isEnabled) { _heatingEnabled = isEnabled; }

    //Return ripple control state
    inline int getSignalRC() { return _signalRC; }
    //Set state of ripple control
    inline void setSignalRC(int signalRC) { _signalRC = signalRC; }

    //Get SHT40 data
    void getSensorData();
    //Get SHT40 data at set time in ms
    void getSensorData(unsigned long time);
    //Temperature control loop function
    void tempControl();
    //Temperature control loop function
    //Next iterations at set time in ms
    void tempControl(unsigned long time);
};

#endif