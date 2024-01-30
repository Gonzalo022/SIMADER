#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include <Adafruit_NeoPixel.h>

#define CYAN    0x00FFFF
#define GREEN   0x00FF00
#define RED     0xFF0000
#define YELLOW  0xFFFF00
#define ORANGE  0xFFA500
#define WHITE   0xFFFFFF

#define NEO_BRIGTHNESS 10


extern Adafruit_NeoPixel neopixel;

enum State {
        CONNECTING_WIFI,
        WIFI_CONNECTED,
        CONNECTING_MQTT,
        MQTT_CONNECT_ERROR,
        MQTT_CONNECTED_,
        FREERTOS_TASK_ERROR,
        NORMAL_OPERATION
};

void initNeopixel();
void setState(State state);
void setColor(uint32_t color);


#endif // STATUSINDICATOR_H
