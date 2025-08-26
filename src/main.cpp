#include "OledDisplayClass.h"
#include "ButtonClass.h"
#include "Sht40Class.h"
#include "MqttClass.h"
#include "TempControlClass.h"
#include "UIClass.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <Preferences.h>

using namespace std;

/*
Load WiFi SSID & Pass and MQTT IP Address:
  const char* ssid = "WiFi_SSID";
  const char* pass = "WiFi_Pass";
  const char* mqttServer = "192.168.0.10";
*/
#include "networkCredentials.h"

#define DEBUG_MODE
//#define DEBUG_MODE_MQTT

#define BAUDRATE 115200

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDRESS_DISPLAY 0x3C
#define I2C_ADDRESS_SHT40 0x45
#define FLIP_SCREEN false

#define BUTTON1_PIN 25  //Button Minus
#define BUTTON2_PIN 26  //Button Plus
#define BUTTON3_PIN 27  //Button Enter

#define RELAY_PIN 19

unsigned long mqttLastEvent;
unsigned long mqttInterval = 60000; //ms
unsigned long reconnectInterval = 5000; //ms
unsigned long mqttToPublishDelay = 1000; //ms

unsigned long displaySleepTimeOut = 60000; //ms
unsigned long uiRefreshIneterval = 100; //ms

unsigned long buttonDebounceTime = 50; //ms
unsigned long buttonLongPressTime = 2000; //ms

unsigned long sensorInterval = 60000; //ms

const float tempSetMin = 15;
const float tempSetMax = 25;

/*
0 = Living Room
1 = Work Room
2 = Bed Room
3 = Bath Room
4 = Outside
*/
const char* roomNames[] = {
  "Obyvak",
  "Pracovna",
  "Loznice",
  "Koupelna",
  "Venkovni"
};
const int roomsCount = 5;
const int defaultRoom = 1;  // LIVING ROOM

const char* mqttPublishTopics[] = {
  "/home/temp/therm_LVR_in",
  "/home/temp/therm_WKR_in",
  "/home/temp/therm_BDR_in",
  "/home/temp/therm_BHR_in",
  "/home/temp/therm_OUT_in",
  "/home/temp/heating_mode_in",
  "/home/electro/switchboard_in"
};
const char* mqttSubscribeTopicsRooms[] = {
  "/home/temp/therm_LVR_out",
  "/home/temp/therm_WKR_out",
  "/home/temp/therm_BDR_out",
  "/home/temp/therm_BHR_out",
  "/home/temp/therm_OUT_out"
};
const char* mqttSubscribeTopicsOthers[] = {
  "/home/temp/heating_mode_out",
  "/home/electro/switchboard_out",
  "/home/time"
};

const char* mqttHumidityKeys[] = {
  "humidity_LVR",
  "humidity_WKR",
  "humidity_BDR",
  "humidity_BHR",
  "humidity_OUT"
};
const char* mqttTempActKeys[] = {
  "temp_act_LVR",
  "temp_act_WKR",
  "temp_act_BDR",
  "temp_act_BHR",
  "temp_act_OUT"
};
const char* mqttTempSetKeys[] = {
  "temp_set_LVR",
  "temp_set_WKR",
  "temp_set_BDR",
  "temp_set_BHR",
  "temp_set_OUT"
};
const char* mqttRelayKeys[] = {
  "relay_LVR",
  "relay_WKR",
  "relay_BDR",
  "relay_BHR",
  "rekat_OUT"
};
const char* mqttOtherKeys[] = {
  "heating_enabled",
  "signal_RC",
  "hours",
  "minutes"
};

void GetPreferences();
void PutPreferences();
Preferences preferences;

OledDisplayClass display(
  I2C_ADDRESS_DISPLAY, I2C_SDA, I2C_SCL, FLIP_SCREEN);

ButtonClass buttonMinus(
  BUTTON1_PIN, INPUT_PULLUP, COUNT_NONE,
  buttonDebounceTime, buttonLongPressTime);
ButtonClass buttonPlus(
  BUTTON2_PIN, INPUT_PULLUP, COUNT_NONE,
  buttonDebounceTime, buttonLongPressTime);
ButtonClass buttonEnter(
  BUTTON3_PIN, INPUT_PULLUP, COUNT_NONE,
  buttonDebounceTime, buttonLongPressTime);

Sht40Class sht40(I2C_ADDRESS_SHT40);

TempControlClass tempControl(
  &sht40, sensorInterval, tempSetMin, tempSetMax, defaultRoom, RELAY_PIN);

void MqttCallback(char* topic, byte* message, unsigned long length);
MqttClass mqtt(mqttServer, MqttCallback);

UIClass ui(
  &display, &buttonMinus, &buttonPlus, &buttonEnter,
  &sht40, &mqtt, &tempControl,
  roomNames, roomsCount, defaultRoom);


/*============================================================================
  SETUP FUNCTION
==============================================================================*/
void setup()
{
  delay(100);
  
  #ifdef DEBUG_MODE
  Serial.begin(BAUDRATE);
  while(!Serial);
  Serial.println();
  Serial.println("Thermostat LVR");
  Serial.println();
  #endif

  //--------------------------------------------------------------------------
  // OLED Display Init
  display.init(displaySleepTimeOut);
  display.string(0, 0, String(roomNames[defaultRoom]));

  //--------------------------------------------------------------------------
  // Flash Memory Init
  #ifdef DEBUG_MODE
  Serial.println("Reading flash memory:");
  #endif
  
  GetPreferences();

  #ifdef DEBUG_MODE
  Serial.print("SHT40 temp calibration: ");
  Serial.println(sht40.getTempCalibration());
  Serial.println("Temp control sensitivity: ");
  Serial.print("-plus: ");
  Serial.println(tempControl.getTempControlSensPlus());
  Serial.print("-minus: ");
  Serial.println(tempControl.getTempControlSensMinus());
  Serial.print("Temp control interval: ");
  Serial.println(tempControl.getTempControlIntervalMinutes());
  Serial.print("Temp difference: ");
  Serial.println(tempControl.getTempControlDifference());
  Serial.print("Relay temp compensation ON: ");
  Serial.println(tempControl.getRelayTempCoefOn());
  Serial.print("Relay temp compensation OFF: ");
  Serial.println(tempControl.getRelayTempCoefOff());
  Serial.println();
  #endif
  
  //--------------------------------------------------------------------------
  // WiFi Init
  #ifdef DEBUG_MODE
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  #endif

  display.string(0, 10, "WiFi ...");
  display.display();

  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    #ifdef DEBUG_MODE
    Serial.print(".");
    #endif
  }

  #ifdef DEBUG_MODE
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  #endif

  String strIP = "Local IP:  " + WiFi.localIP().toString();
  display.string(50, 10, "OK");
  display.string(0, 20, strIP);
  display.display();
 
  //--------------------------------------------------------------------------
  // MQTT Server Init
  int subscribeCount1, subscribeCount2, publishCount;
  for(const char* topic : mqttSubscribeTopicsRooms)
    subscribeCount1++;

  for(const char* topic : mqttSubscribeTopicsOthers)
    subscribeCount2++;

  for(const char* topic : mqttPublishTopics)
    publishCount++;
  
  mqtt.setSubscribeTopics(
    mqttSubscribeTopicsRooms, subscribeCount1,
    mqttSubscribeTopicsOthers, subscribeCount2,
    false
  );
  mqtt.subscribe();
  
  mqtt.setPublishTopics(mqttPublishTopics, publishCount, defaultRoom);

  String strMQTT = "MQTT IP: " + mqtt.getMqttServerIP();
  display.string(0, 30, strMQTT);

  //--------------------------------------------------------------------------
  // Sensor SHT40
  #ifdef DEBUG_MODE
  Serial.println("SHT40 test");
  #endif
  if(!sht40.init())
  {
    #ifdef DEBUG_MODE
    Serial.println("Couldn't find SHT40");
    #endif
    display.string(0, 40, "SHT40 error!");
    display.string(0, 50, "Restart in 60s!");
    display.display();
    delay(60000);
    ESP.restart();
  }
  #ifdef DEBUG_MODE
  Serial.println("Found SHT40 sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht40.getSerial(), HEX);
  #endif
  display.string(0, 40, "SHT40 ...");
  display.display();

  delay(500);

  sht40.getData();
  display.string(50, 40, String(sht40.getHumidity()) + "%rH");
  display.string(90, 40, String(float(sht40.getTemperature()), 1) + "°C");
  display.display();
  
  ui.setRefreshInterval(uiRefreshIneterval);
  tempControl.tempControl(2000);
  mqttLastEvent = millis() - mqttInterval + 2000; //2 seconds to call mqtt publish
  
  delay(2000);
}

/*============================================================================
  MAIN LOOP FUNCTION
==============================================================================*/
void loop()
{
  buttonMinus.loop();
  buttonPlus.loop();
  buttonEnter.loop();

  display.sleepTimer();

  tempControl.getSensorData();
  
  tempControl.tempControl();

  mqtt.loop();

  unsigned long now = millis();
  if(now - mqttLastEvent > mqttInterval)
  {
    mqttLastEvent = now;
    
    if(mqtt.getSynced())
    {
      mqtt.publish(
        mqttPublishTopics[defaultRoom],
        mqttTempSetKeys[defaultRoom], tempControl.getTempSet(defaultRoom),
        mqttTempActKeys[defaultRoom], tempControl.getTempAct(defaultRoom),
        mqttHumidityKeys[defaultRoom], tempControl.getHumidity(defaultRoom),
        mqttRelayKeys[defaultRoom], tempControl.getRelays(defaultRoom)
      );
    }
    else
    {
      mqtt.publish(
        mqttPublishTopics[defaultRoom],
        mqttTempActKeys[defaultRoom], tempControl.getTempAct(defaultRoom),
        mqttHumidityKeys[defaultRoom], tempControl.getHumidity(defaultRoom),
        mqttRelayKeys[defaultRoom], tempControl.getRelays(defaultRoom)
      );
    }
  }

  ui.loop();

  if(ui.getSettingsToSave())
  {
    ui.setSettingsToSave(false);
    PutPreferences();
    tempControl.getSensorData(0);
    tempControl.tempControl(0);
    mqttLastEvent = millis() - mqttInterval;
    mqtt.publish(
      mqttPublishTopics[5],
      mqttOtherKeys[0], tempControl.getHeatingEnabled()
    );
  }

  if(ui.getTempToPublish())
  {
    ui.setTempToPublish(false);
    mqtt.publish(
      mqttPublishTopics[ui.getRoomNumberToPublish()],
      mqttTempSetKeys[ui.getRoomNumberToPublish()],
      tempControl.getTempSet(ui.getRoomNumberToPublish())
    );
    
  }
}

/*----------------------------------------------------------------------------
  Preferences Functions
 -----------------------------------------------------------------------------*/
void GetPreferences()
{
  preferences.begin("tempCtrl", false);
  sht40.setTempCalibration(preferences.getFloat("tempCal", 0));
  sht40.setHumidityCalibration(preferences.getInt("humiCal", 0));
  tempControl.setTempControlSensMinus(preferences.getFloat("sensMinus", 0.3));
  tempControl.setTempControlSensPlus(preferences.getFloat("sensPlus", 0.3));
  tempControl.setTempControlIntervalMinutes(preferences.getInt("interval", 5));
  tempControl.setTempControlDifference(preferences.getFloat("tempDiff", 0.5));
  tempControl.setRelayTempCoefOn(preferences.getFloat("tCoefOn", 0.6));
  tempControl.setRelayTempCoefOff(preferences.getFloat("tCoefOff", 0.4));
  preferences.end();
}

void PutPreferences()
{
  preferences.begin("tempCtrl", false);
  preferences.putFloat("tempCal", sht40.getTempCalibration());
  preferences.putInt("humiCal", sht40.getHumidityCalibration());
  preferences.putFloat("sensMinus", tempControl.getTempControlSensMinus());
  preferences.putFloat("sensPlus", tempControl.getTempControlSensPlus());
  preferences.putInt("interval", tempControl.getTempControlIntervalMinutes());
  preferences.putFloat("tempDiff", tempControl.getTempControlDifference());
  preferences.putFloat("tCoefOn", tempControl.getRelayTempCoefOn());
  preferences.putFloat("tCoefOff", tempControl.getRelayTempCoefOff());
  preferences.end();
}

/*----------------------------------------------------------------------------
  MQTT CallBack Function
 -----------------------------------------------------------------------------*/
void MqttCallback(char* topic, byte* message, unsigned long length)
{
  #ifdef DEBUG_MODE_MQTT
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  #endif

  String json;

  for(int i = 0; i < length; i++) {
    json += (char)message[i];
    #ifdef DEBUG_MODE_MQTT
    Serial.print((char)message[i]);
    #endif
  }
  #ifdef DEBUG_MODE_MQTT
  Serial.println();
  #endif

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);

  if(error) {
    #ifdef DEBUG_MODE_MQTT
    Serial.println();
    Serial.println("Json deserialization failed:");
    Serial.println(error.c_str());
    #endif
    return;
  }

  int i;
  for(const char* topicRooms : mqttSubscribeTopicsRooms)
  {
    if(strcmp(topic, mqttSubscribeTopicsRooms[i]) == 0)
    {
      if(i != defaultRoom)
      {
        if(doc[mqttHumidityKeys[i]].is<float>())
        {
          tempControl.setHumidity(doc[mqttHumidityKeys[i]], i);
          #ifdef DEBUG_MODE_MQTT
          String str = String(mqttHumidityKeys[i]) + " = " + tempControl.getHumidity(i);
          Serial.println(str);
          #endif
        }
        
        if(doc[mqttTempActKeys[i]].is<float>())
        {
          tempControl.setTempAct(doc[mqttTempActKeys[i]], i);
          #ifdef DEBUG_MODE_MQTT
          String str = String(mqttTempActKeys[i]) + " = " + tempControl.getTempAct(i);
          Serial.println(str);
          #endif
        }

        if(doc[mqttRelayKeys[i]].is<int>())
        {
          tempControl.setRelays(doc[mqttRelayKeys[i]], i);
          #ifdef DEBUG_MODE_MQTT
          String str = String(mqttRelayKeys[i]) + " = " + tempControl.getRelays(i);
          Serial.println(str);
          #endif
        }
      }

      if(doc[mqttTempSetKeys[i]].is<float>())
      {
        float temp = doc[mqttTempSetKeys[i]];
        if(temp != tempControl.getTempSet(i))
        {
          tempControl.setTempSet(temp, i);
          if(i == defaultRoom)
          {
            if(!mqtt.getSynced())
              mqtt.setSynced();
            tempControl.tempControl(5000);
          }
          #ifdef DEBUG_MODE_MQTT
          String str = String(mqttTempSetKeys[i]) + " = " + tempControl.getTempSet(i);
          Serial.println(str);
          Serial.println(mqtt.getSynced());
          #endif
        }
      }
    }
    
    i++;
  }

  if(strcmp(topic, mqttSubscribeTopicsOthers[0]) == 0)
  {
    if(doc[mqttOtherKeys[0]].is<bool>())
    {
      tempControl.setHeatingEnabled(doc[mqttOtherKeys[0]]);
    }
  }
  
  if(strcmp(topic, mqttSubscribeTopicsOthers[1]) == 0)
  {
    if(doc[mqttOtherKeys[1]].is<int>())
    {
      tempControl.setSignalRC(doc[mqttOtherKeys[1]]);
    }
  }
  
  if(strcmp(topic, mqttSubscribeTopicsOthers[2]) == 0)
  {
    int timeHH, timeMM;
    if(doc[mqttOtherKeys[2]].is<int>())
    {
      timeHH = doc[mqttOtherKeys[2]];
      #ifdef DEBUG_MODE_MQTT
      String str = String(mqttOtherKeys[2]) + " = " + timeHH;
      Serial.println(str);
      #endif
    }
    if(doc[mqttOtherKeys[3]].is<int>())
    {
      timeMM = doc[mqttOtherKeys[3]];
      #ifdef DEBUG_MODE_MQTT
      String str = String(mqttOtherKeys[3]) + " = " + timeMM;
      Serial.println(str);
      #endif
    }
    ui.setActTime(timeHH, timeMM);
  }
}