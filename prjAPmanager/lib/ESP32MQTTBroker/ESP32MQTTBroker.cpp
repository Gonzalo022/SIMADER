#include "ESP32MQTTBroker.h"

ESP32MQTTBroker::ESP32MQTTBroker() {

}

void ESP32MQTTBroker::begin(unsigned short mqttPort) {
    Serial.println("[MQTTBroker] Starting MQTT Broker...");
    
    if(!init(mqttPort))
        Serial.println("[MQTTBroker] Failed to start MQTT Broker...");
    
}

void ESP32MQTTBroker::loop() {
    update();
}

bool ESP32MQTTBroker::onEvent(sMQTTEvent *event) {
    switch(event->Type()) {
    case NewClient_sMQTTEventType:
        {
        sMQTTNewClientEvent *e = (sMQTTNewClientEvent*)event;
        sMQTTClient *client = e->Client();
        std::string clientId = client->getClientId();

        Serial.print("[MQTTBroker] New client connected with ID: ");
        Serial.println(clientId.c_str());
        }
        break;
    case LostConnect_sMQTTEventType:
        Serial.println("[MQTTBroker] Broker lost connection, reconnecting WiFi...");
        WiFi.reconnect();
        break;
    case Subscribe_sMQTTEventType:
        Serial.println("[MQTTBroker] Client subscribed to a topic");
        break;
    
        
    case UnSubscribe_sMQTTEventType:
        Serial.println("[MQTTBroker] Client unsubscribed from a topic");
        break;
    }
    return true;
}
