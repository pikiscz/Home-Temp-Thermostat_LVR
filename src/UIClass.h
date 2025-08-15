#ifndef UICLASS_H
#define UICLASS_H

#include <Arduino.h>
#include "OledDisplayClass.h"


class UIClass
{
private:
    uint8_t _iconOnlineX[18] = {7, 8, 2, 4, 5, 6, 7, 9, 1, 8, 0, 2, 3, 4, 5, 7, 1, 2};
    uint8_t _iconOnlineY[18] = {0, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 5, 6};
    uint8_t _iconOfflineX[21] = {3, 4, 5, 2, 6, 1, 3, 5, 7, 1, 4, 7, 1, 3, 5, 7, 2, 6, 3, 4, 5};
    uint8_t _iconOfflineY[21] = {0, 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 6};
    uint8_t _iconRelayOnX[24] = {1, 4, 7, 0, 3, 6, 0, 3, 6, 0, 3, 6, 1, 4, 7, 1, 4, 7, 1, 4, 7, 0, 3, 6};
    uint8_t _iconRelayOnY[24] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7};
    uint8_t _iconDisabledX[30] = {1, 4, 7, 0, 3, 6, 0, 3, 6, 7, 0, 3, 4, 5, 6, 1, 2, 3, 4, 7, 0, 1, 4, 7, 1, 4, 7, 0, 3, 6};
    uint8_t _iconDisabledY[30] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7};

    const String* _roomNames;
    OledDisplayClass* _display;

    void DisplayActTime(int timeHH, int timeMM);
    inline String TimeFormater(int time) { return time<10 ? "0" + String(time) : String(time); };
    void DisplayConnectionStatus(bool online);
    void DisplayHeatingDisabled(bool heating);
    void DisplayRelayState(int relay);
    void DisplayRoomName(int roomNumber);
    void DisplayMainTemperature(float temp);
    void DisplaySecondaryTemperature(float temp);
    void DisplayHumidity(int humidity);

public:
    UIClass(OledDisplayClass* display, String roomNames[]);
    ~UIClass();
    
    void testPage(int timeHH, int timeMM, bool online, bool heating, int relay, int roomNumber, int mainTemp, int secTemp, int humidity);
};

#endif