#ifndef SHT40CLASS_H
#define SHT40CLASS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT4x.h>

class Sht40Class
{
private:
    Adafruit_SHT4x _sht40;
    u_int8_t _i2cAdr;
    sensors_event_t _humidity;
    sensors_event_t _temperature;
    int _humidityInt;       //Relative humidity as integer
    int _humidityCalibrationInt;
    int _temperatureInt;    //Sensor temperature (float) multiplied by 10
    int _tempCalibrationInt;    //In degC multiplied by 10
public:
    Sht40Class(u_int8_t i2cAdr);
    ~Sht40Class();
    
    bool init();
    //Return serial number of sensor
    inline int getSerial() { return _sht40.readSerial(); }

    //Return humidity calibration in %
    inline int getHumidityCalibration() {return _humidityCalibrationInt; }
    //Set humidity calibration in %
    inline void setHumidityCalibration(int humidityCal) { _humidityCalibrationInt = humidityCal; }

    //Return temperature offset in degC multiplied by 10
    //(eg: Return 12 for 1.2degC)
    inline int getTempCalibrationInt() { return _tempCalibrationInt; }
    //Set temperature offset in degC multiplied by 10
    //(eg: Set 12 for 1.2degC)
    inline void setTempCalibrationInt(int tempCalInt) { _tempCalibrationInt = tempCalInt; }
    
    //Return temperature offset in degC
    inline float getTempCalibration() { return float(_tempCalibrationInt) / 10; }
    //Set temperature offset in degC
    inline void setTempCalibration(float tempCal) { _tempCalibrationInt = tempCal * 10; }
    
    //Read sensor data
    void getData();
    //Return last readed value of humidity
    int getHumidity();
    //Return last readed temperature in degC multiplied by 10
    //(eg: Return 215 for 21.5degC)
    int getTemperatureInt();
    //Return last readed temperature
    float getTemperature();
};

#endif