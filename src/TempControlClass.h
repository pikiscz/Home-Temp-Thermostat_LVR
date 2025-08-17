#ifndef TEMPCONTROLCLASS_H
#define TEMPCONTROLCLASS_H

#include <Arduino.h>
#include "Sht40Class.h"

class TempControlClass
{
private:
    Sht40Class* _sht40;
public:
    TempControlClass(Sht40Class* sht40);
    ~TempControlClass();
};

#endif