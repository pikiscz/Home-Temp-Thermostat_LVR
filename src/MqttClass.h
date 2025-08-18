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
    const char** _subscribeTopics1;
    const char** _subscribeTopics2;
    const char** _publishTopics;
    int _subscribeTopicsCount1;
    int _subscribeTopicsCount2;
    int _publishTopicsCount;


    bool _connected = false;
    unsigned long _reconnectLastEvent;
    unsigned long _reconnectInterval = 5000;;

    WiFiClient _espClient;
    PubSubClient _client;

    void SerializeDoc(const char* topic, JsonDocument doc);

public:
    MqttClass(const char* mqttServer);
    MqttClass(
        const char* mqttServer,
        std::function<void(char*, uint8_t*, unsigned int)> mqttCallback
    );
    ~MqttClass();
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback);
    void subscribeTopics(const char* topics[], int count);
    void subscribeTopics(
        const char* topics1[], int count1,
        const char* topics2[], int count2,
        bool joinToOne
    );
    void publishTopics(const char* topics[], int count);
    void subscribe();
    bool reconnect();
    void loop(unsigned long now);
    inline bool getConnected() { return _connected; }

    void publish(
        const char* topic,
        const char* key1, float value1
    );
    void publish(
        const char* topic,
        const char* key1, float value1,
        const char* key2, float value2
    );
    void publish(
        const char* topic,
        const char* key1, float value1,
        const char* key2, float value2,
        const char* key3, float value3
    );
    void publish(
        const char* topic,
        const char* key1, float value1,
        const char* key2, float value2,
        const char* key3, float value3,
        const char* key4, float value4
    );
    void publish(String key1, float value1, int topic);
    void publish(String key1, float value1, String key2, float value2, int topic);
    void publish(String key1, float value1, String key2, float value2, String key3, float value3, int topic);
    void publish(String key1, float value1, String key2, float value2, String key3, float value3, String key4, float value4, int topic);
    
};

#endif