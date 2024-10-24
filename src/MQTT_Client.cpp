#include "MQTT_Client.h"
#include "CONFIG.h"
#include "_SPIFFS.h"
#include "WiFi.h"
WiFiClient wifiClient;
PubSubClient client(wifiClient);
unsigned long lastRandomTime = 0;
unsigned long Interval = 5000;
String MQTT_Target_Topic;
String MQTT_Mess_Topic;
String MQTT_Report_Topic;
bool connect_to_broker()
{
  static uint8_t try_count = 0;
  Serial.print("Đang kết nối đến MQTT...");
  String clientId = "IoTLab4";
  clientId += String(random(0xffff), HEX);
  while (try_count < 5)
  {
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
    {
      Serial.println("đã kết nối");
      Serial.println(data.UID_Result);
      MQTT_Target_Topic = String(MQTT_USER) + "/tele/" + data.UID_Result + "/Target";
      MQTT_Report_Topic = String(MQTT_USER) + "/report/" + Device_ID + "/Request";
      MQTT_Mess_Topic = String(MQTT_USER) + "/mess/" + Device_ID + "/Message";
      client.subscribe(MQTT_Target_Topic.c_str());
      client.subscribe(MQTT_Report_Topic.c_str());
      client.subscribe(MQTT_Mess_Topic.c_str());
      push_Status();
      return true;
    }
    else
    {
      Serial.print("thất bại, mã lỗi=");
      Serial.print(client.state());
      Serial.println(", thử lại sau 2 giây");
      try_count++;
      delay(2000);
    }
    if (try_count >= 5)
      return false;
  }
  return false;
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String MQTT_Respond_Topic = String(MQTT_USER) + "/respond/" + Device_ID + "/Status";
  if (strcmp(topic, MQTT_Target_Topic.c_str()) == 0)
  {
    // Xử lý dữ liệu từ MQTT_TAGET_TOPIC
    String _payload;
    isNewTarge = true;
    Serial.println("Received message from MQTT_TAGET_TOPIC:");
    Serial.write(payload, length);
    Serial.println();
    _payload = String((char *)payload, length);
    data.target = _payload.toInt();
    writeStruct("/data.bin", data);
    client.publish(MQTT_Respond_Topic.c_str(), "Recv Success");
  }
  else if (strcmp(topic, MQTT_Mess_Topic.c_str()) == 0)
  {
    // Xử lý dữ liệu từ MQTT_MESS_TOPIC
    Serial.println("Received message from MQTT_MESS_TOPIC:");
    Serial.write(payload, length);
    Serial.println();
    mess = String((char *)payload, length);
    Serial.println(mess);
    client.publish(MQTT_Respond_Topic.c_str(), "Recv Success");
  }
}

void Setup_MQTT()
{
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  connect_to_broker();
}

bool push_Worker(String startTime, String endTime, String workTime)
{
  if (data.UID_Result == "")
  {
    return false;
  }
  String topic_sensor = String(MQTT_USER) + "/tele/" + data.UID_Result + "/Time_Work";
  JsonDocument jsonDocument;
  jsonDocument["Time"] = timeClient.getFormattedDate();
  jsonDocument["UserID"] = data.UID_Result;
  jsonDocument["TimeStart"] = startTime;
  jsonDocument["TimeEnd"] = endTime;
  jsonDocument["TimeWork"] = time_work_String;
  jsonDocument["Product"] = data.product;
  jsonDocument["Diff"] = data.error;
  jsonDocument["Target"] = data.target;
  jsonDocument["deviceID"] = Device_ID;
  String jsonString;
  serializeJson(jsonDocument, jsonString);
  char payload[jsonString.length() + 1];
  jsonString.toCharArray(payload, jsonString.length() + 1);
  if (client.publish(topic_sensor.c_str(), payload))
  {
    Serial.println("đã gửi Thông Tin Worker xong");
    return true;
  }
  return false;
}
bool push_Status()
{
  String MQTT_Status_Topic = String(MQTT_USER) + "/tele/" + "Device" + "/" + Device_ID;
  JsonDocument jsonDocument;
  jsonDocument["Time"] = timeClient.getFormattedDate();
  jsonDocument["SSID"] = WiFi.SSID();
  jsonDocument["RSSI"] = WiFi.RSSI();
  jsonDocument["IP"] = WiFi.localIP();
  jsonDocument["deviceID"] = Device_ID;
  if (isWorking)
  {
    jsonDocument["status"] = "Working";
  }
  else
  {
    jsonDocument["status"] = "Free";
  }
  String jsonString;
  serializeJson(jsonDocument, jsonString);
  char payload[jsonString.length() + 1];
  jsonString.toCharArray(payload, jsonString.length() + 1);
  if (client.publish(MQTT_Status_Topic.c_str(), payload))
  {
    Serial.println("đã gửi STATUS  xong");
    return true;
  }
  return false;
}
bool push_report(char code)
{
  switch (code)
  {
  case '1':
    // const char* payload = "Yêu Cầu Bổ Sung Vật Liệu";
    if (client.publish(MQTT_Report_Topic.c_str(), "Yêu Cầu Bổ Sung Vật Liệu"))
    {
      return true;
    }
  case '2':
    if (client.publish(MQTT_Report_Topic.c_str(), "Sự Cố Máy Móc"))
    {
      return true;
    }
  case '3':
    if (client.publish(MQTT_Report_Topic.c_str(), "Tình Huống Khẩn Cấp !!!"))
    {
      return true;
    }
  default:
    if (client.publish(MQTT_Report_Topic.c_str(), "Null"))
    {
      return true;
    }
  }
  return false;
}

void MQTT_tele()
{
  unsigned long currentTime = millis();
  if (currentTime - lastRandomTime >= Interval)
  {
    // push_mqtt(formattedDate,Device_ID ,UID_Result, timeString);
    lastRandomTime = currentTime;
    push_Status();
    push_Worker(timeString, time_end_String, time_work_String);
  }
}
