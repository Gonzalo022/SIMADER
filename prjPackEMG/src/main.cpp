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

#include <WiFi.h>
#include <WiFiAP.h>
#include <PubSubClient.h>
#include <StatusIndicator.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define VBATPIN A13

/*********** Wi-Fi variables ***********/
WiFiClient espClient;

const char* ssid = "ESP32-AP";
const char* password = "prueba1234";

const char* broker_ip = "192.168.1.1";
const unsigned short broker_port = 2883;

/*********** MQTT variables ***********/

PubSubClient client(espClient);

const char* topic_emg = "sensors/emg/s1";
const char* topic_bat = "sensors/bat/s1";

/*********** Program constants ***********/

const float vbatMin = 3.0;
const float vbatMax = 4.2;

QueueHandle_t xQueueSensor;

Adafruit_NeoPixel neopixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void wifiConnect()
{
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    setState(CONNECTING_WIFI);
    delay(3000);
  }

  setState(WIFI_CONNECTED);
  delay(2000);
}

void mqttConnect() {
  client.setServer(broker_ip, broker_port);
  while (!client.connected()) {
    setState(CONNECTING_MQTT);
    delay(1000);

    if (client.connect("ESP32ClientEMG1")) {
      setState(MQTT_CONNECTED_);
      delay(2000);

    } else {
      setState(MQTT_CONNECT_ERROR);
      delay(5000);
    }
  }
}

void vTaskRetrieveData(void *pvParameters) {
  const portTickType xTicksToWait = 500 / portTICK_RATE_MS;
  portBASE_TYPE xStatus;
  for (;;) {
    int data;
    data = analogRead(A2);

    xStatus = xQueueSendToBack(xQueueSensor, &data, xTicksToWait);
    if (xStatus != pdPASS)
      setState(NORMAL_OPERATION);
    
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelete(NULL);
}

void vTaskSendData(void *pvParameters){
  mqttConnect();
  const portTickType xTicksToWait = 500 / portTICK_RATE_MS;
  portBASE_TYPE xStatus;
  
  for(;;){
    int valueReceived;
    xStatus = xQueueReceive(xQueueSensor, &valueReceived, xTicksToWait);
    if (xStatus == pdPASS)
      client.publish(topic_emg, String(valueReceived).c_str());

    vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelete(NULL);
}

void vTaskMQTTClientManager(void *pvParameters) {
  for (;;) {
    if (WiFi.status() != WL_CONNECTED){
      wifiConnect();
      setState(NORMAL_OPERATION);
    }

    if (!client.connected()){
      mqttConnect();
      setState(NORMAL_OPERATION);
    }

    client.loop();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  vTaskDelete(NULL);
}

void vTaskCheckBatteryLife(void *pvParameters) {
  for (;;) {
    float measuredvbat = analogReadMilliVolts(VBATPIN);
    measuredvbat *= 2;
    measuredvbat /= 1000;

    float batteryPercentage = (measuredvbat - vbatMin) / (vbatMax - vbatMin) * 100;
    batteryPercentage = constrain(batteryPercentage, 0, 100);

    client.publish(topic_bat, (String(batteryPercentage) + "%").c_str());

    vTaskDelay(pdMS_TO_TICKS(1000 * 60 * 2)); //1000ms * 60 sec * 2  = 2 minutes
  }
  vTaskDelete(NULL);
}

void appMain(void){
  xQueueSensor = xQueueCreate(50, sizeof(int));
  xTaskCreate(vTaskMQTTClientManager, "ClientManager", 4096, NULL, 2, NULL);
  xTaskCreate(vTaskCheckBatteryLife, "BatteryLife", 4096, NULL, 2, NULL);
  xTaskCreate(vTaskSendData, "SendData", 4096, NULL, 3, NULL);
  xTaskCreate(vTaskRetrieveData, "RetrieveData", 4096, NULL, 3, NULL);
}

void setup() {
  initNeopixel();
  wifiConnect();
  mqttConnect();
  setState(NORMAL_OPERATION);
  appMain();
}

void loop(){
  
}
