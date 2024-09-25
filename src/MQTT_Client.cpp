#include "MQTT_Client.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "CONFIG.h"

#define MQTT_SERVER "app.lotoda.vn"
#define MQTT_PORT 1883
#define MQTT_USER "aNnT68MIn8"
#define MQTT_PASSWORD "2Cn9GnE22q6CdpanBGIC"
#define MQTT_LDP_TOPIC "aNnT68MIn8/cmnd/Taget"
#define MQTT_LDP_TOPIC1 "aNnT68MIn8/cmnd/Mess"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastRandomTime = 0;
unsigned long randomInterval = 5000;

void connect_to_broker() {
  while (!client.connected()) {
    Serial.print("Đang kết nối đến MQTT...");
    String clientId = "IoTLab4";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("đã kết nối");
      client.subscribe(MQTT_LDP_TOPIC);
      client.subscribe(MQTT_LDP_TOPIC1);
    } else {
      Serial.print("thất bại, mã lỗi=");
      Serial.print(client.state());
      Serial.println(", thử lại sau 2 giây");
      delay(2000);
    }
  }
}


void callback(char* topic, byte *payload, unsigned int length) {
  if (strcmp(topic, MQTT_LDP_TOPIC) == 0) {
    // Xử lý dữ liệu từ MQTT_LDP_TOPIC
    Serial.println("Received message from MQTT_LDP_TOPIC:");
    Serial.write(payload, length);
    Serial.println();
    target = String((char*)payload, length);
    Serial.println(target);
  } else if (strcmp(topic, MQTT_LDP_TOPIC1) == 0) {
    // Xử lý dữ liệu từ MQTT_LDP_TOPIC1
    Serial.println("Received message from MQTT_LDP_TOPIC1:");
    Serial.write(payload, length);
    Serial.println();
    mess = String((char*)payload, length);
    Serial.println(mess);
  }
}

void Setup_MQTT() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  connect_to_broker();
}

void push_mqtt1(String Time, String Mac, String UID_Result, String timeString, String time_end_String, String time_work_String, int result, int result_error) {
  String topic_sensor = "aNnT68MIn8/tele/" + Mac + "/" + UID_Result + "/Time_Work";
  const char *topic_sensor_cstr = topic_sensor.c_str();
  
  //const size_t bufferSize = JSON_OBJECT_SIZE(5) + UID_Result.length() + timeString.length() + time_end_String.length() + 20 + time_work_String.length() + 20 + result + 20 + result_error + 20;
  JsonDocument jsonDocument;
  jsonDocument["Time"] = Time;
  jsonDocument["Uid"] = UID_Result;
  jsonDocument["TimeStart"] = timeString;
  jsonDocument["Time_End"] = time_end_String;
  jsonDocument["Time_Work"] = time_work_String;
  jsonDocument["Product"] = result;
  jsonDocument["Error"] = result_error;
  String jsonString;
  serializeJson(jsonDocument, jsonString);
  char payload[jsonString.length() + 1];
  jsonString.toCharArray(payload, jsonString.length() + 1);
  client.publish(topic_sensor_cstr, payload);
  Serial.println("đã gửi MQTT xong");
}

void push_mqtt(String Time, String Mac, String UID_Result, String timeString) {
  String topic_sensor = "aNnT68MIn8/tele/" + Mac + "/" + UID_Result + "/Start";
  const char *topic_sensor_cstr = topic_sensor.c_str();
  //const size_t bufferSize = JSON_OBJECT_SIZE(3) + Time.length() + UID_Result.length() + timeString.length() + 20;
  JsonDocument jsonDocument;
  jsonDocument["Time"] = Time;
  jsonDocument["Uid"] = UID_Result;
  jsonDocument["TimeStart"] = timeString;
  String jsonString;
  serializeJson(jsonDocument, jsonString);
  char payload[jsonString.length() + 1];
  jsonString.toCharArray(payload, jsonString.length() + 1);
  client.publish(topic_sensor_cstr, payload);
  Serial.println("đã gửi MQTT xong");
}

void MQTT_tele(){
    unsigned long currentTime = millis();
    if (currentTime - lastRandomTime >= randomInterval) {
      //push_mqtt(formattedDate,Device_ID ,UID_Result, timeString);
      push_mqtt1(formattedDate, Device_ID , worker_UID, timeString, time_end_String, time_work_String, result_product, result_error);
    lastRandomTime = currentTime;
  }
}


