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
    _display->string(1, 1, "Nastaveni");
    String strPage = String(_settingsCurrentPage + 1) + "/" + String(_settingsPages);
    _display->string(126, 1, strPage, TEXT_RIGHT);
    
    if(_settingsArrowY[_settingsCurrentPage][_settingsCurrentRow] != -1)
        _display->string(1, _settingsArrowY[_settingsCurrentPage][_settingsCurrentRow] + 1, "->");

    if(_settingsCurrentPage == 0)
    {
        _display->string(11, 11, "Topeni:");
        if(_tempControl->getHeatingEnabled())
            _display->string(126, 11, "zapnuto", TEXT_RIGHT);
        else
            _display->string(126, 11, "vypnuto", TEXT_RIGHT);

        _display->string(1, 21, "Regulace:");
        
        String strSensPlus = String(_tempControl->getTempControlSensPlus(), 1) + "°C";
        _display->string(11, 31, "Citlivost +:");
        _display->string(126, 31, strSensPlus, TEXT_RIGHT);

        String strSensMinus = "-" + String(_tempControl->getTempControlSensMinus(), 1) + "°C";
        _display->string(11, 41, "Citlivost -:");
        _display->string(126, 41, strSensMinus, TEXT_RIGHT);

        String strInterval = String(_tempControl->getTempControlIntervalMinutes()) + "min";
        _display->string(11, 51, "Frekvence:");
        _display->string(126, 51, strInterval, TEXT_RIGHT);
    }
    else if(_settingsCurrentPage == 1)
    {
        _display->string(1, 11, "Detekce vetrani:");
        
        String strDiff = "-" + String(_tempControl->getTempControlDifference(), 1) + "°C";
        _display->string(11, 21, "Pokles o:");
        _display->string(126, 21, strDiff, TEXT_RIGHT);

        _display->string(1, 31, "Teplot. koef. rele:");

        String strRelayOn = String(_tempControl->getRelayTempCoefOn(), 2);
        _display->string(11, 41, "Pri sepnuti:");
        _display->string(126, 41, strRelayOn, TEXT_RIGHT);

        String strRelayOff = String(_tempControl->getRelayTempCoefOff(), 2);
        _display->string(11, 51, "Pri rozepnuti:");
        _display->string(126, 51, strRelayOff, TEXT_RIGHT);
    }
    else if(_settingsCurrentPage == 2)
    {
        _display->string(1, 11, "Kalibrace sensoru:");

        String strTempCal = String(_sht40->getTempCalibration(), 1) + "°C";
        String strTemp = String(_sht40->getTemperature(), 1) + "°C";
        _display->string(1, 21, "Teplota:");
        _display->string(126, 21, strTemp, TEXT_RIGHT);
        _display->string(11, 31, "Offset teploty:");
        _display->string(126, 31, strTempCal, TEXT_RIGHT);

        String strHumiCal = String(_sht40->getHumidityCalibration()) + "%rH";
        String strHumi = String(_sht40->getHumidity()) + "%rH";
        _display->string(1, 41, "Vlhkost:");
        _display->string(126, 41, strHumi, TEXT_RIGHT);
        _display->string(11, 51, "Offset vlhkosti:");
        _display->string(126, 51, strHumiCal, TEXT_RIGHT);
    }
    else
    {
        _display->string(1, 11, "IP zarizeni:");
        _display->string(11, 21, WiFi.localIP().toString());
        _display->string(1, 31, "IP MQTT serveru:");
        _display->string(11, 41, _mqtt->getMqttServerIP());
    }
        
    _display->display();
}

void UIClass::loop(unsigned long now) {
    if(_buttonMinus->isPressed())
    {
        if(_display->getDisplayOn())
        {
            if(!_drawSettings && _tempControl->getHeatingEnabled())
            {
                if(_tempControl->getTempSet(_currentRoom) > _tempControl->getTempSetMin())
                {
                    _tempControl->setTempSet(_tempControl->getTempSet(_currentRoom) - 0.5, _currentRoom);
                    
                    _tempSetChanged = true;
                    _tempSetToPublishTime = now;
                    _roomNumberToPublish = _currentRoom;
                }
            }

            if(_drawSettings)
            {
                if(_settingsCurrentPage == 0)
                {
                    switch(_settingsCurrentRow)
                    {
                    case 0:
                        _tempControl->setHeatingEnabled(!_tempControl->getHeatingEnabled());
                        break;

                    case 1:
                        _tempControl->setTempControlSensPlus(_tempControl->getTempControlSensPlus() - 0.1);
                        if(_tempControl->getTempControlSensPlus() < 0)
                            _tempControl->setTempControlSensPlus(0);
                        break;
                    
                    case 2:
                        _tempControl->setTempControlSensMinus(_tempControl->getTempControlSensMinus() - 0.1);
                        if(_tempControl->getTempControlSensMinus() < 0)
                            _tempControl->setTempControlSensMinus(0);
                        break;

                    case 3:
                        _tempControl->setTempControlIntervalMinutes(_tempControl->getTempControlIntervalMinutes() - 1);
                        if(_tempControl->getTempControlIntervalMinutes() < 1)
                            _tempControl->setTempControlIntervalMinutes(1);
                        break;

                    default:
                        break;
                    }
                }
                else if(_settingsCurrentPage == 1)
                {
                    switch(_settingsCurrentRow)
                    {
                    case 0:
                        _tempControl->setTempControlDifference(_tempControl->getTempControlDifference() - 0.1);
                        if(_tempControl->getTempControlDifference() < 0)
                            _tempControl->setTempControlDifference(0);
                        break;

                    case 1:
                        _tempControl->setRelayTempCoefOn(_tempControl->getRelayTempCoefOn() - 0.01);
                        if(_tempControl->getRelayTempCoefOn() < 0)
                            _tempControl->setRelayTempCoefOn(0);
                        break;
                    
                    case 2:
                        _tempControl->setRelayTempCoefOff(_tempControl->getRelayTempCoefOff() - 0.01);
                        if(_tempControl->getRelayTempCoefOff() < 0)
                            _tempControl->setRelayTempCoefOff(0);
                        break;
                    
                    default:
                        break;
                    }
                }
                else if(_settingsCurrentPage == 2)
                {
                    switch(_settingsCurrentRow)
                    {
                    case 0:
                        _sht40->setTempCalibrationInt(_sht40->getTempCalibrationInt() - 1);
                        break;

                    case 1:
                        _sht40->setHumidityCalibration(_sht40->getHumidityCalibration() - 1);
                        break;
                    
                    default:
                        break;
                    }
                }
            }
        }
        _display->resetSleepTimer(now);
    }

    if(_buttonPlus->isPressed())
    {
        if(_display->getDisplayOn())
        {
            if(!_drawSettings && _tempControl->getHeatingEnabled())
            {
                if(_tempControl->getTempSet(_currentRoom) < _tempControl->getTempSetMax())
                {
                    _tempControl->setTempSet(_tempControl->getTempSet(_currentRoom) + 0.5, _currentRoom);
                    
                    _tempSetChanged = true;
                    _tempSetToPublishTime = now;
                    _roomNumberToPublish = _currentRoom;
                }
            }

            if(_drawSettings)
            {
                if(_settingsCurrentPage == 0)
                {
                    switch(_settingsCurrentRow)
                    {
                    case 0:
                        _tempControl->setHeatingEnabled(!_tempControl->getHeatingEnabled());
                        break;

                    case 1:
                        _tempControl->setTempControlSensPlus(_tempControl->getTempControlSensPlus() + 0.1);
                        break;
                    
                    case 2:
                        _tempControl->setTempControlSensMinus(_tempControl->getTempControlSensMinus() + 0.1);
                        break;

                    case 3:
                        _tempControl->setTempControlIntervalMinutes(_tempControl->getTempControlIntervalMinutes() + 1);
                        break;

                    default:
                        break;
                    }
                }
                else if(_settingsCurrentPage == 1)
                {
                    switch(_settingsCurrentRow)
                    {
                    case 0:
                        _tempControl->setTempControlDifference(_tempControl->getTempControlDifference() + 0.1);
                        break;

                    case 1:
                        _tempControl->setRelayTempCoefOn(_tempControl->getRelayTempCoefOn() + 0.01);
                        break;
                    
                    case 2:
                        _tempControl->setRelayTempCoefOff(_tempControl->getRelayTempCoefOff() + 0.01);
                        break;
                    
                    default:
                        break;
                    }
                }
                else if(_settingsCurrentPage == 2)
                {
                    switch(_settingsCurrentRow)
                    {
                    case 0:
                        _sht40->setTempCalibrationInt(_sht40->getTempCalibrationInt() + 1);
                        break;

                    case 1:
                        _sht40->setHumidityCalibration(_sht40->getHumidityCalibration() + 1);
                        break;
                    
                    default:
                        break;
                    }
                }
            }
        }
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
                
                if(_tempSetChanged)
                    _tempSetToPublishTime += _tempSetToPublishDelay;
            }
            else
            {
                if(_settingsCurrentRow < _settingsRowsPrePage[_settingsCurrentPage] -1)
                {
                    _settingsCurrentRow++;
                }
                else
                {
                    _settingsCurrentRow = 0;

                    if(_settingsCurrentPage < _settingsPages - 1)
                        _settingsCurrentPage++;
                    else
                        _settingsCurrentPage = 0;
                }               
            }
        }
        _display->resetSleepTimer(now);
    }
    
    if(_tempSetChanged)
    {
        if(now - _tempSetToPublishTime > _tempSetToPublishDelay)
        {
            _tempSetChanged = false;
            _tempSetReadyToPublish = true;

            if(_roomNumberToPublish == _defaultRoom)
                _tempControl->delayedTempControl(1000);
        }
    }

    if(_buttonEnter->isLongPress())
    {
        _buttonEnter->resetLongPress();
        if(_drawSettings)
        {
            _drawSettings = false;
            _settingToSave = true;
        }
        else
        {
            _settingsCurrentPage = 0;
            _settingsCurrentRow = 0;
            _drawSettings = true;
        }
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
        }
    }
}

