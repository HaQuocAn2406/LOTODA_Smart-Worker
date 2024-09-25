#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#define MQTT_SERVER "app.lotoda.vn"
#define MQTT_PORT 1883
#define MQTT_USER "aNnT68MIn8"
#define MQTT_PASSWORD "2Cn9GnE22q6CdpanBGIC"
#define MQTT_LDP_TOPIC "aNnT68MIn8/cmnd/Taget"
#define MQTT_LDP_TOPIC1 "aNnT68MIn8/cmnd/Mess" 

extern PubSubClient client;

void Setup_MQTT();
void push_mqtt1(String Time, String Mac, String UID_Result, String timeString, String time_end_String, String time_work_String, int result_product, int result_error);
// void push_mqtt(String Time, String Mac, String UID_Result, String timeString);
void connect_to_broker();
void MQTT_tele();

#endif
