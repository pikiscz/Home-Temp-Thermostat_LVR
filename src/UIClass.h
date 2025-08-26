#ifndef UICLASS_H
#define UICLASS_H

#include <Arduino.h>
#include "OledDisplayClass.h"
#include "ButtonClass.h"
#include "Sht40Class.h"
#include "MqttClass.h"
#include "TempControlClass.h"

//#define DEBUG_MODE

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

    const char** _roomNames;
    int _roomsCount;
    int _defaultRoom;
    int _currentRoom;

    int _timeHH;
    int _timeMM;

    OledDisplayClass* _display;
    ButtonClass* _buttonMinus;
    ButtonClass* _buttonPlus;
    ButtonClass* _buttonEnter;
    Sht40Class* _sht40;
    MqttClass* _mqtt;
    TempControlClass* _tempControl;

    unsigned long _lastRefresh;
    unsigned long _refreshInterval;

    bool _tempSetChanged = false;
    unsigned long _tempSetToPublishTime;
    unsigned long _tempSetToPublishDelay = 1000;
    int _roomNumberToPublish;
    bool _tempSetReadyToPublish = false;

    bool _drawSettings;
    int _settingsPages = 4;
    int _settingsCurrentPage;
    int _settingsCurrentRow;
    int _settingsRowsPrePage[4] = {4, 3, 2, 1};
    int _settingsArrowY[4][5] = {
        {11, 31, 41, 51, -1},
        {21, 41, 51, -1, -1},
        {31, 51, -1, -1, -1},
        {-1, -1, -1, -1, -1}
    };
    bool _settingToSave;

    void DrawActTime(int timeHH, int timeMM);
    inline String TimeFormater(int time) { return time<10 ? "0" + String(time) : String(time); };
    void DrawConnectionStatus(bool online);
    void DrawHeatingDisabled();
    void DrawRelayState(int relay);
    void DrawRoomName(int roomNumber);
    void DrawMainTemperature(float temp);
    void DrawSecondaryTemperature(float temp);
    void DrawHumidity(int humidity);

    void DrawCurrentRoomHeatingOn();
    void DrawCurrentRoomHeatingOff();
    void DrawSettings();

public:
    UIClass(
        OledDisplayClass* display,
        ButtonClass* buttonMinus,
        ButtonClass* buttonPlus,
        ButtonClass* buttonEnter,
        Sht40Class* sht40,
        MqttClass* mqtt,
        TempControlClass* tempControl,
        const char** roomNames,
        int roomsCount,
        int defaultRoom
    );
    ~UIClass();
    //Set clock time
    inline void setActTime(int timeHH, int timeMM) { _timeHH = timeHH; _timeMM = timeMM; }
    //Set refresh time of loop()
    inline void setRefreshInterval(unsigned long refreshInterval) { _refreshInterval = refreshInterval; }
    //Main loop function
    void loop();

    //Return TRUE when tempSet is waiting to publish
    //setTempToPublish(false) should be called after publishing
    inline bool getTempToPublish() {return _tempSetReadyToPublish; }
    //Set TRUE when tempSet is waiting to publish
    inline void setTempToPublish(bool publish) { _tempSetReadyToPublish = publish; }
    //Return room number to publish tempSet
    inline int getRoomNumberToPublish() { return _roomNumberToPublish; }

    //Return TRUE when exiting settings menu
    //setSettingsToSave(false) should be called after saving
    inline bool getSettingsToSave() { return _settingToSave; }
    //Set TRUE when settings should be saved
    inline void setSettingsToSave(bool saveSettings) { _settingToSave = saveSettings; }
};

#endif