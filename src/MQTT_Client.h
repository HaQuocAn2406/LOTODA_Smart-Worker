#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#define MQTT_SERVER "app.lotoda.vn"
#define MQTT_PORT 1883
#define MQTT_USER "BS4dINg77F"
#define MQTT_PASSWORD "nSSKt0fUMpE2Dk564M4j"
#define MQTT_TAGET_TOPIC "BS4dINg77F/cmnd/Taget"
#define MQTT_MESS_TOPIC "BS4dINg77F/cmnd/Mess" 

extern PubSubClient client;
extern String MQTT_Target_Topic;
extern String MQTT_Mess_Topic;
void Setup_MQTT();
void push_mqtt1(String Time, String Mac, String UID_Result, String timeString, String time_end_String, String time_work_String, int result_product, int result_error);
// void push_mqtt(String Time, String Mac, String UID_Result, String timeString);
void connect_to_broker();
void MQTT_tele();

#endif
