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
    int _numberOfTopicsIn;
    int _numberOfTopicsOut;

    bool _mqttConnected = false;

    WiFiClient _espClient;
    PubSubClient _client;

    void SerializeDoc(StaticJsonDocument<100> doc, int topic);

public:
    MqttClass(const char* mqttServer, String mqttTopicsIn[], String mqttTopicsOut[]);
    ~MqttClass();
    void init(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback);
    void subscribe();
    void reconnect();
    void publish(char* key1, float value1, int topic);
    void publish(char* key1, float value1, char* key2, float value2, int topic);
    void publish(char* key1, float value1, char* key2, float value2, char* key3, float value3, int topic);
    void publish(char* key1, float value1, char* key2, float value2, char* key3, float value3, char* key4, float value4, int topic);
    inline bool getConnectionStatus() { return _mqttConnected; }
};

#endif