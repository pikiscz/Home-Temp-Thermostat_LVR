#ifndef OLEDDISPLAYCLASS_H
#define OLEDDISPLAYCLASS_H

#include <Arduino.h>
#include <Wire.h>
#include <SH1106Wire.h>


#define FONT_10 ArialMT_Plain_10
#define FONT_24 ArialMT_Plain_24
#define TEXT_LEFT TEXT_ALIGN_LEFT
#define TEXT_CENTER TEXT_ALIGN_CENTER
#define TEXT_RIGHT TEXT_ALIGN_RIGHT

class OledDisplayClass //: public SH1106Wire
{
private:
    SH1106Wire _display;
    bool _flipScreen;
    bool _displayIsOn;
    unsigned long _displayOnTimer;
    unsigned long _sleepTimeOut;

public:
    OledDisplayClass(u_int8_t adr, int sda, int scl, bool flipScreen);
    ~OledDisplayClass();

    void init(unsigned long sleepTimeOut);
    void init();

    //Turn on display
    inline void setDisplayOn() { _display.displayOn(); _displayIsOn = true; }
    //Turn off display
    inline void setDisplayOff() { _display.displayOff(); _displayIsOn = false; }
    //Return TRUE when display is turned on and FALSE when it is turned off
    inline bool getDisplayOn() { return _displayIsOn; }
    //Set time in ms of inactivity before turning off
    inline void setSleepTimeOut(unsigned long sleepTimeOut) { _sleepTimeOut = sleepTimeOut; }
    //Check time of inactivity and then turn of display
    void sleepTimer(unsigned long now);
    //Reset inactivity timer and turn on display in case it is off
    void resetSleepTimer(unsigned long now);

    inline void display() { _display.display(); }
    inline void clear() { _display.clear(); }

    inline void pixel(int x, int y) { _display.setPixel(x, y); }
    void string(int x, int y, String str);
    void string(int x, int y, String str, const u_int8_t* font);
    void string(int x, int y, String str, OLEDDISPLAY_TEXT_ALIGNMENT alignment);
    void string(int x, int y, String str, const u_int8_t* font, OLEDDISPLAY_TEXT_ALIGNMENT alignment);
};

#endif