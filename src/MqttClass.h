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
    int _defaultTopic;

    bool _connected = false;
    unsigned long _reconnectLastEvent;
    unsigned long _reconnectInterval = 5000;
    bool _synchoronized;

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
    //Return MQTT IP Address
    inline String getMqttServerIP() {return String(_mqttServer); }
    //Set callback function to receive messages
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback);
    //Set list of topics to be subscribed for callback function
    void setSubscribeTopics(const char* topics[], int count);
    void setSubscribeTopics(
        const char* topics1[], int count1,
        const char* topics2[], int count2,
        bool joinToOne
    );
    //Set a list of publishing topics
    void setPublishTopics(const char* topics[], int count, int defaultTopic);
    //Return list of publishing topics
    inline const char* getPublishTopics(int topicNumber) { return _publishTopics[topicNumber]; }
    //Subscribe to all topics set by subscribeTopics()
    void subscribe();
    //Try to connect to MQTT server with random client ID
    //Return TRUE when succesfully connected, othewise return FALSE
    bool reconnect();
    //Loop funciton checks for connection status and incomming messages
    void loop(unsigned long now);
    //Return TRUE when connected to MQTT server
    inline bool getConnected() { return _connected; }
    //Return synchornizetion status
    //Status is set via getSynced()
    inline bool getSynced() { return _synchoronized; }
    //Set synchronizetion status to TRUE
    inline void setSynced() { _synchoronized = true; }
    //Set synchonization status
    inline void setSynced( bool synchornized) { _synchoronized = synchornized; }
    //Publish to MQTT server
    void publish(
        const char* topic,
        const char* key1, float value1
    );
    //Publish to MQTT server
    void publish(
        const char* topic,
        const char* key1, float value1,
        const char* key2, float value2
    );
    //Publish to MQTT server
    void publish(
        const char* topic,
        const char* key1, float value1,
        const char* key2, float value2,
        const char* key3, float value3
    );
    //Publish to MQTT server
    void publish(
        const char* topic,
        const char* key1, float value1,
        const char* key2, float value2,
        const char* key3, float value3,
        const char* key4, float value4
    );
};

#endif