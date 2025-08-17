#include "OledDisplayClass.h"
#include "Sht40Class.h"
#include "MqttClass.h"
#include "TempControlClass.h"
#include "UIClass.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <Preferences.h>
#include <ezButton.h>

using namespace std;

/*
Load WiFi SSID & Pass and MQTT IP Address:
  const char* ssid = "WiFi_SSID";
  const char* pass = "WiFi_Pass";
  const char* mqttServer = "192.168.0.10";
*/
#include "networkCredentials.h"

#define DEBUG_MODE
#define DEBUG_MODE_MQTT
#define DEBUG_MODE_TEMP

#define BAUDRATE 115200

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDRESS_DISPLAY 0x3C
#define I2C_ADDRESS_SHT40 0x45
#define FLIP_SCREEN false

#define BUTTON1_PIN 25  //Button Minus
#define BUTTON2_PIN 26  //Button Plus
#define BUTTON3_PIN 27  //Button Enter

unsigned long mqttLastEvent;
unsigned long mqttInterval = 60000; //ms
unsigned long reconnectInterval = 5000; //ms
unsigned long mqttToPublishDelay = 1000; //ms

unsigned long displaySleepTimeOut = 60000; //ms
unsigned long uiRefreshIneterval = 100; //ms

float tempSetMin = 15;
float tempSetMax = 25;

/*
0 = Outside
1 = Living Room
2 = Work Room
3 = Bed Room
4 = Bath Room
*/
String roomNames[] = {
  "Venkovni",
  "Obyvak",
  "Pracovna",
  "Loznice",
  "Koupelna"
};

String mqttTopicIn[] = {
  "/home/temp/heating_mode_in",
  "/home/temp/therm_LVR_in",
  "/home/temp/therm_WKR_in",
  "/home/temp/therm_BDR_in",
  "/home/temp/therm_BHR_in",
  "/home/temp/therm_OUT_in",
  "/home/electro/switchboard_in"
};
String mqttTopicOut[] = {
  "/home/temp/heating_mode_out",
  "/home/temp/therm_LVR_out",
  "/home/temp/therm_WKR_out",
  "/home/temp/therm_BDR_out",
  "/home/temp/therm_BHR_out",
  "/home/temp/therm_OUT_out",
  "/home/electro/switchboard_out",
  "/home/time"
};
const int numberOfTopics = 8;


Preferences preferences;

OledDisplayClass display(I2C_ADDRESS_DISPLAY, I2C_SDA, I2C_SCL, FLIP_SCREEN);

Sht40Class sht40(I2C_ADDRESS_SHT40);

TempControlClass tempControl(&sht40);

void MqttCallback(char* topic, byte* message, unsigned long length);
MqttClass mqtt(mqttServer, mqttTopicIn, mqttTopicOut);

UIClass ui(&display, &sht40, &mqtt,roomNames);


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
  display.string(0, 0, "Living Room");

  //--------------------------------------------------------------------------
  // Flash Memory Init
  #ifdef DEBUG_MODE
  Serial.println("Reading flash memory:");
  #endif
  preferences.begin("temp_control", false);
  sht40.setTempCalibrationInt(preferences.getInt("sht40_temp", 0));
  /*
  TO-DO
  */
  preferences.end();

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

  String strIP = "Local IP:" + WiFi.localIP().toString();
  display.string(50, 10, "OK");
  display.string(0, 20, strIP);
  display.display();
 
  //--------------------------------------------------------------------------
  // MQTT Server Init
  mqtt.init(MqttCallback);
  mqtt.subscribe();

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

  delay(1000);
  
  
  ui.setRefreshInterval(uiRefreshIneterval);
  

  mqttLastEvent = millis() - mqttInterval + 3000; //3 seconds to call mqtt publish
}

void loop()
{
  unsigned long now = millis();

  display.sleepTimer(now);

  mqtt.loop(now);

  ui.refresh(now);
  

}

void MqttCallback(char* topic, byte* message, unsigned long length)
{
  #ifdef DEBUG_MODE_MQTT
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.println("Message:");
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
}