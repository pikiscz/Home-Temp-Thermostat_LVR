#include "TempControlClass.h"

TempControlClass::TempControlClass(Sht40Class* sht40)
{
    _sht40 = sht40;
}

TempControlClass::~TempControlClass()
{

}