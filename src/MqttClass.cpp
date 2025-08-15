#include "MqttClass.h"

MqttClass::MqttClass(const char* mqttServer, String mqttTopicsIn[], String mqttTopicsOut[])
{
    _mqttServer = mqttServer;
    
    _client.setClient(_espClient);
    _client.setServer(_mqttServer, 1883);

    _numberOfTopicsIn = sizeof(mqttTopicsIn) / sizeof(mqttTopicsIn[0]);
    _mqttTopicsIn = new const String[_numberOfTopicsIn];
    _mqttTopicsIn = mqttTopicsIn;

    _numberOfTopicsOut = sizeof(mqttTopicsOut) / sizeof(mqttTopicsOut[0]);
    _mqttTopicsOut = new const String[_numberOfTopicsOut];
    _mqttTopicsOut = mqttTopicsOut;
}

MqttClass::~MqttClass()
{

}

void MqttClass::init(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback)
{
    _client.setCallback(mqttCallback);
}

void MqttClass::subscribe()
{
    for(int i = 0; i < _numberOfTopicsIn; i++)
    {
        _client.subscribe(_mqttTopicsIn[i].c_str());
    }
}

void MqttClass::reconnect()
{
    if(!_client.connected())
    {
        #ifdef DEBUG_MODE
        Serial.println("Attempting MQTT connection...");
        #endif

        String clientId = "ESP32Client-LVR-";
        clientId += String(random(0xffff), HEX);

        if(_client.connect(clientId.c_str()))
        {
            #ifdef DEBUG_MODE
            Serial.println("connected");
            #endif
            subscribe();
            _mqttConnected = true;
            publish("connected", true, 1);
        }
        else
        {
            #ifdef DEBUG_MODE
            Serial.print("failed, rc=");
            Serial.print(client.state());
            #endif
            _mqttConnected = false;
        }
    }
}


void MqttClass::publish(char* key1, float value1, int topic)
{
    StaticJsonDocument<100> doc;
    doc[key1] = value1;
    SerializeDoc(doc, topic);
}

void MqttClass::publish(char* key1, float value1, char* key2, float value2, int topic)
{
    StaticJsonDocument<100> doc;
    doc[key1] = value1;
    doc[key2] = value2;
    SerializeDoc(doc, topic);
}

void MqttClass::publish(char* key1, float value1, char* key2, float value2, char* key3, float value3, int topic)
{
    StaticJsonDocument<100> doc;
    doc[key1] = value1;
    doc[key2] = value2;
    doc[key3] = value3;
    SerializeDoc(doc, topic);
}

void MqttClass::publish(char* key1, float value1, char* key2, float value2, char* key3, float value3, char* key4, float value4, int topic)
{
    StaticJsonDocument<100> doc;
    doc[key1] = value1;
    doc[key2] = value2;
    doc[key3] = value3;
    doc[key4] = value4;
    SerializeDoc(doc, topic);
}

void MqttClass::SerializeDoc(StaticJsonDocument<100> doc, int topic)
{
    char output[100];

    serializeJson(doc, output);

    #ifdef DEBUG_MODE
     Serial.println();
    Serial.print("Publishing on topic: ");
    Serial.println(mqttTopicIn[topic]);
    Serial.println("Message:");
    Serial.println(output);
    #endif

    _client.publish(_mqttTopicsIn[topic].c_str(), output);
}