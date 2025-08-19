#include "Sht40Class.h"

Sht40Class::Sht40Class(uint8_t i2cAdr) : _sht40()
{
    _i2cAdr = i2cAdr;
}

Sht40Class::~Sht40Class() {}

bool Sht40Class::init()
{
    bool result = _sht40.begin(&Wire);

    if(!result)
    {
        return result;
    }

    _sht40.setPrecision(SHT4X_HIGH_PRECISION);
    _sht40.setHeater(SHT4X_NO_HEATER);

    return result;
}

void Sht40Class::getData()
{
    _sht40.getEvent(&_humidity, &_temperature);

    _humidityInt = _humidity.relative_humidity;
    _temperatureInt = (10 * _temperature.temperature) + _tempCalibrationInt;
}