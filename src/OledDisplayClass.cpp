#include "OledDisplayClass.h"

OledDisplayClass::OledDisplayClass(u_int8_t adr, int sda, int scl, bool flipScreen): _display(adr, sda, scl)
{
    _flipScreen = flipScreen;
}

OledDisplayClass::~OledDisplayClass() {}

void OledDisplayClass::init(unsigned long sleepTimeOut)
{
    _display.init();
    if(_flipScreen)
    {
        _display.flipScreenVertically();
    }
    _display.clear();
    _display.display();
    _displayIsOn = true;
    _sleepTimeOut = sleepTimeOut;
    _displayOnTimer = millis();
}

void OledDisplayClass::init()
{
    init(0);
}

void OledDisplayClass::sleepTimer(unsigned long now)
{
    if(_displayIsOn && _sleepTimeOut > 0)
    {
        if(now - _displayOnTimer > _sleepTimeOut)
            setDisplayOff();
    }
}

void OledDisplayClass::resetSleepTimer(unsigned long now)
{
    _displayOnTimer = now;
    if(!_displayIsOn)
        setDisplayOn();
}

void OledDisplayClass::string(int x, int y, String str)
{
    _display.setFont(FONT_10);
    _display.setTextAlignment(TEXT_ALIGN_LEFT);
    _display.drawString(x, y, str);
}

void OledDisplayClass::string(int x, int y, String str, const u_int8_t* font)
{
    _display.setFont(font);
    _display.setTextAlignment(TEXT_ALIGN_LEFT);
    _display.drawString(x, y, str);
}

void OledDisplayClass::string(int x, int y, String str, OLEDDISPLAY_TEXT_ALIGNMENT alignment)
{
    _display.setFont(FONT_10);
    _display.setTextAlignment(alignment);
    _display.drawString(x, y, str);
}

void OledDisplayClass::string(int x, int y, String str, const u_int8_t* font, OLEDDISPLAY_TEXT_ALIGNMENT alignment)
{
    _display.setFont(font);
    _display.setTextAlignment(alignment);
    _display.drawString(x, y, str);
}