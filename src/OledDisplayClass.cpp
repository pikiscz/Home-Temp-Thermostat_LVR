#include "OledDisplayClass.h"

OledDisplayClass::OledDisplayClass(u_int8_t adr, int sda, int scl, bool flipScreen): _display(adr, sda, scl)
{
    _flipScreen = flipScreen;
}

OledDisplayClass::~OledDisplayClass()
{

}

void OledDisplayClass::init()
{
    _display.init();
    if(_flipScreen)
    {
        _display.flipScreenVertically();
    }
    _display.clear();
    _display.display();
}

void OledDisplayClass::string(int x, int y, String str)
{
    _display.setFont(FONT_DEFAULT);
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
    _display.setFont(FONT_DEFAULT);
    _display.setTextAlignment(alignment);
    _display.drawString(x, y, str);
}

void OledDisplayClass::string(int x, int y, String str, const u_int8_t* font, OLEDDISPLAY_TEXT_ALIGNMENT alignment)
{
    _display.setFont(font);
    _display.setTextAlignment(alignment);
    _display.drawString(x, y, str);
}