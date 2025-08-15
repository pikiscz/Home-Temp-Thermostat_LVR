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
    int _temperatureInt;    //Sensor temperature (float) multiplied by 10
    int _tempCalibrationInt;    //In degC multiplied by 10
public:
    Sht40Class(u_int8_t i2cAdr);
    ~Sht40Class();
    
    bool init();
    inline int getSerial() { return _sht40.readSerial(); }
    inline int getTempCalibrationInt() { return _tempCalibrationInt; }
    inline void setTempCalibrationInt(int tempCalInt) { _tempCalibrationInt = tempCalInt; }
    
    void getData();
    inline int getHumidity() { return _humidityInt; }
    inline int getTemperatureInt() { return _temperatureInt; }
};

#endif