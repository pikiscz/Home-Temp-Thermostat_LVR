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
    int _countOfTopicsIn;
    int _countOfTopicsOut;

    bool _connected = false;
    unsigned long _reconnectLastEvent;
    unsigned long _reconnectInterval = 5000;;

    WiFiClient _espClient;
    PubSubClient _client;

    void SerializeDoc(JsonDocument doc, int topic);

public:
    MqttClass(const char* mqttServer, String mqttTopicsIn[], String mqttTopicsOut[]);
    ~MqttClass();
    void init(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback);
    void subscribe();
    void reconnect();
    void loop(unsigned long now);
    inline bool getConnected() { return _connected; }

    void publish(String key1, float value1, int topic);
    void publish(String key1, float value1, String key2, float value2, int topic);
    void publish(String key1, float value1, String key2, float value2, String key3, float value3, int topic);
    void publish(String key1, float value1, String key2, float value2, String key3, float value3, String key4, float value4, int topic);
    
    void callback();
};

#endif