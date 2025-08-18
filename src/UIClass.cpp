#include "UIClass.h"

UIClass::UIClass(
    OledDisplayClass* display,
    Sht40Class* sht40,
    MqttClass* mqtt,
    String roomNames[],
    int defaultRoom
)
{
    _display = display;
    _sht40 = sht40;
    _mqtt = mqtt;

    _countOfRooms = sizeof(roomNames) / sizeof(roomNames[0]);

    _roomNames = new const String[_countOfRooms];
    _roomNames = roomNames;
    _defaultRoom = defaultRoom;
    _currentRoom = defaultRoom;
}

UIClass::~UIClass()
{

}

void UIClass::DisplayActTime(int timeHH, int timeMM)
{
    String str = TimeFormater(timeHH) + ":" + TimeFormater(timeMM);
    _display->string(111, 3, str, FONT_10, TEXT_RIGHT);
}

void UIClass::DisplayConnectionStatus(bool online)
{
    if(online)
    {
        for(int i = 0; i < sizeof(_iconOnlineX); i++)
        {
            _display->pixel(114 + _iconOnlineX[i], 6 + _iconOnlineY[i]);
        }
    }
    else
    {
        for(int i = 0; i < sizeof(_iconOfflineX); i++){
            _display->pixel(114 + _iconOfflineX[i], 6 + _iconOfflineY[i]);
        }
    }
}

void UIClass::DisplayHeatingDisabled(bool heating)
{
    if(!heating)
    {
        for(int i = 0; i < sizeof(_iconDisabledX); i++)
        {
            _display->pixel(3 + _iconDisabledX[i], 50 + _iconDisabledY[i]);
        }
    }
}

void UIClass::DisplayRelayState(int relay)
{
    if(relay == 1)
    {
        for(int i = 0; i < sizeof(_iconRelayOnX); i++) {
            _display->pixel(60 + _iconRelayOnX[i], 50 + _iconRelayOnY[i]);
        }
    }
}

void UIClass::DisplayRoomName(int roomNumber)
{
    String str = _roomNames[roomNumber];
    _display->string(3, 3, str);
}

void UIClass::DisplayMainTemperature(float temp)
{
    _display->string(84, 17, String(temp, 1), FONT_24, TEXT_RIGHT);
    _display->string(86, 19, "°C", FONT_10, TEXT_LEFT);
}

void UIClass::DisplaySecondaryTemperature(float temp)
{
    _display->string(3, 47, String(temp,1) + "°C");
}

void UIClass::DisplayHumidity(int humidity)
{
    _display->string(125, 47, String(humidity) + "%rH", TEXT_RIGHT);
}

void UIClass::refresh(unsigned long now) {
    if(now - _lastRefresh > _refreshInterval)
    {
        _lastRefresh = now;
        testPage();
    }
}

void UIClass::testPage()
{
    _display->clear();
    DisplayActTime(_timeHH, _timeMM);
    DisplayConnectionStatus(_mqtt->getConnected());
    DisplayHeatingDisabled(true);
    DisplayRelayState(true);
    DisplayRoomName(1);
    DisplayMainTemperature(_sht40->getTemperature());
    DisplaySecondaryTemperature(21.5);
    DisplayHumidity(_sht40->getHumidity());
    _display->display();
}