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
#include <ESP32MQTTBroker.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char* ap_ssid = "ESP32-AP";
const char* ap_password = "prueba1234";

const char* broker_ip = "192.168.1.1";
const unsigned short broker_port = 2883;

ESP32MQTTBroker mqtt_broker;
WiFiClient espClient;
PubSubClient client(espClient);

const char* topic_all = "sensors/#";

void launchAP() {
  IPAddress local_IP(192, 168, 1, 1); 
  IPAddress gateway(192, 168, 1, 1);  
  IPAddress subnet(255, 255, 255, 0);

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("[AP] Failed to configure the softAP");
  }

  if (!WiFi.softAP(ap_ssid, ap_password)) {
    Serial.println("[AP] Failed to start the softAP");

  } else {
    Serial.println("[AP] Access point started");
    Serial.print("[AP] IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void mqttConnect() {
  client.setServer("127.0.0.1", broker_port);
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");

    if (client.connect("ESP32Manager")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");

      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("[MQTTBroker] '%s': ", topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void vTaskMQTTBrokerManager(void *pvParameters){
  for(;;){
    mqtt_broker.loop();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  vTaskDelete(NULL);
}

void vTaskMQTTClientManager(void *pvParameters){
  mqttConnect();
  client.subscribe(topic_all);
  client.setCallback(mqttCallback);
  for (;;) {        
    if (!client.connected())
      mqttConnect();
        
    client.loop();
    vTaskDelay(pdMS_TO_TICKS(100));
    }
  vTaskDelete(NULL);
}

void appMain(void){
  xTaskCreate(vTaskMQTTBrokerManager, "BrokerManager", 4096, NULL, 3, NULL);
  xTaskCreate(vTaskMQTTClientManager, "ClientManager", 4096, NULL, 2, NULL);
}

void setup(){
  Serial.begin(115200);
  launchAP();

  mqtt_broker.begin(broker_port);
  appMain();
}

void loop(){
  
}
