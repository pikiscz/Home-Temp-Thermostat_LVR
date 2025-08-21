#include "UIClass.h"

UIClass::UIClass(
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
)
{
    _display = display;
    _buttonMinus = buttonMinus;
    _buttonPlus = buttonPlus;
    _buttonEnter = buttonEnter;
    _sht40 = sht40;
    _mqtt = mqtt;
    _tempControl = tempControl;

    _roomsCount = roomsCount;

    _roomNames = roomNames;
    _defaultRoom = defaultRoom;
    _currentRoom = defaultRoom;

    _drawSettings = false;
}

UIClass::~UIClass() {}

void UIClass::DrawActTime(int timeHH, int timeMM)
{
    String str = TimeFormater(timeHH) + ":" + TimeFormater(timeMM);
    _display->string(111, 3, str, FONT_10, TEXT_RIGHT);
}

void UIClass::DrawConnectionStatus(bool online)
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

void UIClass::DrawHeatingDisabled()
{
    for(int i = 0; i < sizeof(_iconDisabledX); i++)
    {
        _display->pixel(3 + _iconDisabledX[i], 50 + _iconDisabledY[i]);
    }
}

void UIClass::DrawRelayState(int relay)
{
    if(relay == 1)
    {
        for(int i = 0; i < sizeof(_iconRelayOnX); i++) {
            _display->pixel(60 + _iconRelayOnX[i], 50 + _iconRelayOnY[i]);
        }
    }
}

void UIClass::DrawRoomName(int roomNumber)
{
    String str = _roomNames[roomNumber];
    _display->string(3, 3, str);
}

void UIClass::DrawMainTemperature(float temp)
{
    _display->string(84, 17, String(temp, 1), FONT_24, TEXT_RIGHT);
    _display->string(86, 19, "°C", FONT_10, TEXT_LEFT);
}

void UIClass::DrawSecondaryTemperature(float temp)
{
    _display->string(3, 47, String(temp,1) + "°C");
}

void UIClass::DrawHumidity(int humidity)
{
    _display->string(125, 47, String(humidity) + "%rH", TEXT_RIGHT);
}


void UIClass::loop(unsigned long now) {
    if(_buttonMinus->isPressed())
    {
        _display->resetSleepTimer(now);
    }
    if(_buttonPlus->isPressed())
    {
        _display->resetSleepTimer(now);
    }
    if(_buttonEnter->isPressed())
    {
        if(_display->getDisplayOn())
        {
            if(!_drawSettings)
            {
                if(_currentRoom < _roomsCount - 1)
                    _currentRoom++;
                else
                    _currentRoom = 0;
            }
            else
            {
                if(_settingsCurrentPage < _settingsPages - 1)
                    _settingsCurrentPage++;
                else
                    _settingsCurrentPage = 0;
            }
        }
        _display->resetSleepTimer(now);
    }

    if(_buttonEnter->isLongPress())
    {
        _buttonEnter->resetLongPress();
        if(_drawSettings)
            _drawSettings = false;
        else
            _drawSettings = true;
    }
    
    if(_display->getDisplayOn())
    {
        if(now - _lastRefresh > _refreshInterval)
        {
            _lastRefresh = now;
            
            if(!_drawSettings)
            {
                if(_tempControl->getHeatingEnabled())
                    DrawCurrentRoomHeatingOn();
                else
                    DrawCurrentRoomHeatingOff();
            }
            else
            {
                DrawSettings();
            }
        }
    }
    else
    {
        if(_currentRoom != _defaultRoom)
            _currentRoom = _defaultRoom;
        
        if(_drawSettings)
        {
            _drawSettings = false;
            _settingsCurrentPage = 0;
            _settingsCurrentRow = 0;
        }
    }
}

void UIClass::DrawCurrentRoomHeatingOn()
{
    _display->clear();
    DrawActTime(_timeHH, _timeMM);
    DrawConnectionStatus(_mqtt->getConnected());
    DrawRoomName(_currentRoom);
    if(_currentRoom != 4)
    {
        DrawRelayState(_tempControl->getRelays(_currentRoom));
        DrawMainTemperature(_tempControl->getTempSet(_currentRoom));
        DrawSecondaryTemperature(_tempControl->getTempAct(_currentRoom));
    }
    else
    {
        DrawMainTemperature(_tempControl->getTempAct(_currentRoom));
    }
    DrawHumidity(_tempControl->getHumidity(_currentRoom));
    _display->display();
}

void UIClass::DrawCurrentRoomHeatingOff()
{
    _display->clear();
    DrawActTime(_timeHH, _timeMM);
    DrawConnectionStatus(_mqtt->getConnected());
    DrawHeatingDisabled();
    DrawRoomName(_currentRoom);
    DrawMainTemperature(_tempControl->getTempAct(_currentRoom));
    DrawHumidity(_tempControl->getHumidity(_currentRoom));
    _display->display();
}

void UIClass::DrawSettings()
{
    _display->clear();
    _display->string(3, 3, "Nastaveni");
    String str = String(_settingsCurrentPage + 1) + "/" + String(_settingsPages);
    _display->string(125, 3, str, TEXT_RIGHT);
    _display->display();
}