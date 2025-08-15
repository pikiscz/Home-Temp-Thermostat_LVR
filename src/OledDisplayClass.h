#ifndef OLEDDISPLAYCLASS_H
#define OLEDDISPLAYCLASS_H

#include <Arduino.h>
#include <Wire.h>
#include <SH1106Wire.h>

class OledDisplayClass
{
private:
    #define FONT_DEFAULT ArialMT_Plain_10
    SH1106Wire _display;
    bool _flipScreen;
public:
    OledDisplayClass(u_int8_t adr, int sda, int scl, bool flipScreen);
    ~OledDisplayClass();

    void init();
    inline void display() { _display.display(); }
    inline void clear() { _display.clear(); }

    void string(int x, int y, String str);
    void string(int x, int y, String str, const u_int8_t* font);
    void string(int x, int y, String str, OLEDDISPLAY_TEXT_ALIGNMENT alignment);
    void string(int x, int y, String str, const u_int8_t* font, OLEDDISPLAY_TEXT_ALIGNMENT alignment);
};

#endif