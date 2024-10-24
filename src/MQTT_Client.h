#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "TimeManager.h"
#define MQTT_SERVER "app.lotoda.vn"
#define MQTT_PORT 1883
#define MQTT_USER "BS4dINg77F"
#define MQTT_PASSWORD "nSSKt0fUMpE2Dk564M4j"
#define MQTT_TAGET_TOPIC "BS4dINg77F/cmnd/Taget"
#define MQTT_MESS_TOPIC "BS4dINg77F/cmnd/Mess"
extern PubSubClient client;
extern String MQTT_Target_Topic;
extern String MQTT_Mess_Topic;
extern String MQTT_Report_Topic;
void Setup_MQTT();
bool push_Worker( String startTime, String endTime, String workTime);
bool push_report(char code);
bool push_Status();
bool connect_to_broker();
void MQTT_tele();

#endif
