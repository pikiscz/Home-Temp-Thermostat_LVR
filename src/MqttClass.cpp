#include "MqttClass.h"

MqttClass::MqttClass(const char* mqttServer, String mqttTopicsIn[], String mqttTopicsOut[])
{
    _mqttServer = mqttServer;
    
    _client.setClient(_espClient);
    _client.setServer(_mqttServer, 1883);

    int arraySizeIn = sizeof(mqttTopicsIn) / sizeof(mqttTopicsIn[0]);
    _mqttTopicsIn = new const String[arraySizeIn];
    _mqttTopicsIn = mqttTopicsIn;

    int arraySizeOut = sizeof(mqttTopicsOut) / sizeof(mqttTopicsOut[0]);
    _mqttTopicsOut = new const String[arraySizeOut];
    _mqttTopicsOut = mqttTopicsOut;
}

MqttClass::~MqttClass()
{

}

void MqttClass::init(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback)
{
    _client.setCallback(mqttCallback);
}