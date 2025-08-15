#ifndef MQTTCLASS_H
#define MQTTCLASS_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

class MqttClass
{
private:
    const char* _mqttServer;
    const String* _mqttTopicsIn;
    const String* _mqttTopicsOut;

    WiFiClient _espClient;
    PubSubClient _client;

public:
    MqttClass(const char* mqttServer);
    ~MqttClass();
    void init(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback);
};

#endif