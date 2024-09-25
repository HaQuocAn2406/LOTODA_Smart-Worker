#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

extern PubSubClient client; 

void Setup_MQTT();
void push_mqtt1(String Time, String Mac, String UID_Result, String timeString, String time_end_String, String time_work_String, int result, int result_error);
void push_mqtt(String Time, String Mac, String UID_Result, String timeString);
void connect_to_broker();
void MQTT_tele();

#endif
