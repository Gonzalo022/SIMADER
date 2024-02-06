/*
Copyright (c) 2023 Gonzalo Arévalo Gómez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
