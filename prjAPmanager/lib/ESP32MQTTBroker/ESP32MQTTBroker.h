#ifndef ESP32MQTTBroker_h
#define ESP32MQTTBroker_h

#include <sMQTTBroker.h>

class ESP32MQTTBroker: public sMQTTBroker {
public:
    ESP32MQTTBroker();
    void begin(unsigned short mqttPort = 1883);
    void loop();

protected:
    virtual bool onEvent(sMQTTEvent *event) override;
    unsigned long lastMemoryCheckTime;
    unsigned long freeRam;
};

#endif
