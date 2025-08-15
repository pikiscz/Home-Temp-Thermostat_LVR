#include "MqttClass.h"

MqttClass::MqttClass(const char* mqttServer)
{
    _mqttServer = mqttServer;
    
    _client.setClient(_espClient);
    _client.setServer(_mqttServer, 1883);

    
}

MqttClass::~MqttClass()
{

}

void MqttClass::init(std::function<void(char*, uint8_t*, unsigned int)> mqttCallback)
{
    _client.setCallback(mqttCallback);
}