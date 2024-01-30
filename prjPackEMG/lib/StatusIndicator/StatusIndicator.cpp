#include "StatusIndicator.h"

void initNeopixel(){
    neopixel.begin();
    neopixel.clear();
    neopixel.setBrightness(NEO_BRIGTHNESS);
    neopixel.show();
}

void setState(State state) {
    switch (state) {
        case CONNECTING_WIFI:
            setColor(WHITE);
            break;
        case WIFI_CONNECTED:
            setColor(GREEN); 
            break;
        case CONNECTING_MQTT:
            setColor(ORANGE);
            break;
        case MQTT_CONNECTED_:
            setColor(GREEN); 
            break;
        case MQTT_CONNECT_ERROR:
            setColor(RED); 
            break;
        case FREERTOS_TASK_ERROR:
            setColor(YELLOW); 
            break;
        case NORMAL_OPERATION:
            setColor(CYAN); 
            break;
    }
}

void setColor(uint32_t color) {
    neopixel.setPixelColor(0, color);
    neopixel.show();
}
