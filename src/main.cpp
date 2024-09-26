
#include <LCD_I2C.h>
#include <WiFi.h>
#include "MQTT_Client.h"
#include "CONFIG.h"
#include "TimeManager.h"
#include "RFIDManager.h"
#include "KeypadManager.h"
#include "sever.h"
#include "_SPIFFS.h"
#define AC_SSID "LOTODA WiFi Config"
#define AC_PASS "12345678"
#define PasswordLength 4
//----------------------------------------
LCD_I2C lcd(0x3F, 20, 4); // Default address of most PCF8574 modules, change according
                          // Variable
LOTODA_Server server;

Data data;
bool check = false;
bool scanning = false;
bool cardScanned = false;
bool writingToEEPROM = false;
bool Status_start = true;
bool passCorrect = false;
char customKey;
String wifiScanResult = "[]";
String timeString, time_end_String;
String formattedDate;
String data_time_start = "";
String Device_ID;
String ssid = "", password = "";

int gio, phut, giay;

void handleScan()
{
  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        Serial.println("Đã nhận lệnh Scan");
    if (scanning) {
      request->send(503, "application/json", "{\"status\":\"Scanning in progress\"}");
    } else {
      scanning = true; // Start the scan in the task
      request->send(200, "application/json", wifiScanResult); // Send previous results
    } });
}
TaskHandle_t scanTaskHandle;
void scanWiFiNetworks(void *parameter) // task scan wifi
{
  static long prevSend = 0;
  static long sendInterval = 1000 * 10;
  while (true)
  {
    if (scanning)
    {
      Serial.println("Đang Scan");
      int n = WiFi.scanNetworks(); // Perform the scan
      String json = "[";
      for (int i = 0; i < n; i++)
      {
        if (i > 0)
          json += ",";
        json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
        delay(10); // Yield for watchdog
      }
      json += "]";
      wifiScanResult = json; // Store result
      scanning = false;      // Mark scan as completed
    }
    else
    {
      delay(500); // Avoid blocking by pausing task between scans
    }
  }
}

void Get_Date();

void Save_EEPROM()
{
  Get_time_Start();
  Get_Date();
  // push_mqtt1(formattedDate, Device_ID, data.UID_Result, timeString, time_end_String, time_work_String, data.product, data.error);
  Status_start = false;
  cardScanned = false; // Reset trạng thái đã quét thẻ
  check = true;
}

void Start_Screen()
{
  if (Status_start && customKey == 'A')
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Scan Your Card");
    while (true)
    {
      if (getUID())
      {
        data.UID_Result = UID;
        writeStruct("/data.bin", data);
        Serial.print("UID_Result; ");
        Serial.println(data.UID_Result);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Worker UID:");
        lcd.print(data.UID_Result);
        cardScanned = true;
        lcd.setCursor(0, 2);
        lcd.print("Loading......");
        connect_to_broker();
        Save_EEPROM();
        delay(2000);
        lcd.clear();
        lcd.setCursor(5, 1);
        lcd.print("DONE!");
        delay(2000);
        lcd.clear();
        break; // Thoát khỏi vòng lặp khi đã đọc thành công thẻ
      }
      else
      {
        yield(); // avoid wdt enable
      }
    }
  }
  else if (cardScanned == false)
  {
    lcd.setCursor(1, 0);
    lcd.print("Welcome LOTODA");
    lcd.setCursor(1, 1);
    lcd.print("Press A start");
  }
}

void Get_Date()
{
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  Serial.print("Date: ");
  Serial.println(formattedDate);
}

void Time_end()
{
  Get_time_End();
  time_end_String = String(gio_end) + ":" + String(phut_end) + ":" + String(giay_end);
  Serial.print("time_end");
  Serial.print(":");
  Serial.print(gio_end);
  Serial.print(":");
  Serial.print(phut_end);
  Serial.print(":");
  Serial.println(giay_end);
}

void Time_work()
{
  readStruct("/data.bin", data);
  Working_Time(data.gio_start, data.phut_start, data.giay_start);
  Serial.print("time_work");
  Serial.print(":");
  Serial.print(gio_lam_viec);
  Serial.print(":");
  Serial.print(phut_lam_viec);
  Serial.print(":");
  Serial.println(giay_lam_viec);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Time work ");
  lcd.setCursor(3, 1);
  lcd.print(gio_lam_viec);
  lcd.print(":");
  lcd.print(phut_lam_viec);
  lcd.print(":");
  lcd.print(giay_lam_viec);
  lcd.setCursor(3, 3);
  lcd.print("GoodBye!!");
}

// void scrollMessage(int row, String message, int delayTime, int totalColumns)
// {

//   for (int i = 0; i < totalColumns; i++)
//   {

//     message = " " + message;
//   }

//   message = message + " ";

//   for (int position = 0; position < message.length(); position++)
//   {

//     lcd.setCursor(0, row);

//     lcd.print(message.substring(position, position + totalColumns));

//     delay(delayTime);
//   }
// }
void Working_Screen()
{
  lcd.setCursor(0, 0);
  lcd.print("ID:");
  lcd.print(data.UID_Result);
  lcd.setCursor(11, 0);
  lcd.print("  ");
  lcd.setCursor(12, 0);
  if (gio < 10)
  {
    lcd.print("0");
  }
  lcd.print(gio);
  lcd.print(":");
  if (phut < 10)
  {
    lcd.print("0");
  }
  lcd.print(phut);
  lcd.print(":");
  if (giay < 10)
  {
    lcd.print("0");
  }
  lcd.print(giay);
  lcd.setCursor(0, 1);
  lcd.print("D sp: ");
  if (data.product >= 999)
  {
    lcd.setCursor(5, 1);
  }
  lcd.print(data.product);
  lcd.setCursor(9, 1);
  lcd.print("|");
  lcd.setCursor(11, 1);
  lcd.print("B Er: ");
  if (data.error >= 999)
  {
    lcd.setCursor(16, 1);
  }
  lcd.print(data.error);
  lcd.setCursor(0, 3);
  lcd.print("Target: ");
  lcd.setCursor(8, 3);
  lcd.print(target);
  lcd.setCursor(13, 3);
  lcd.print("C Stop");
}

void Read_time()
{
  /*chương trình lấy thời gian thực để hiển thị*/
  timeClient.update();
  gio = timeClient.getHours();
  phut = timeClient.getMinutes();
  giay = timeClient.getSeconds();
  Working_Screen();
}

void Stop_work()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan to stop");
  lcd.setCursor(2, 1);
  lcd.print("Sp:");
  lcd.setCursor(6, 1);
  lcd.print(data.product);
  lcd.setCursor(17, 1);
  lcd.print("pcs");
  lcd.setCursor(2, 2);
  lcd.print("Error:");
  lcd.setCursor(9, 2);
  lcd.print(data.error);
  lcd.setCursor(17, 2);
  lcd.print("pcs");
  lcd.setCursor(0, 3);
  lcd.print(" * Back");
  while (true)
  {
    char key = customKeypad.getKey();
    if (getUID() || passCorrect == true)
    {
      lcd.clear();
      if (UID == data.UID_Result || UID == data.Admin_UID || passCorrect == true)
      {
        Time_end();
        Time_work();
        Get_Date();
        push_mqtt1(formattedDate, Device_ID, data.UID_Result, timeString, time_end_String, time_work_String, data.product, data.error);
        data.UID_Result = "";
        time_end_String = "";
        time_work_String = "";
        data.error = 0;
        data.product = 0;
        target = "0";
        check = false;
        Status_start = true;
        writeStruct("/data.bin", data);
        delay(2000);
        lcd.clear();
        break; // Thoát khỏi vòng lặp sau khi quét thẻ thành công
      }
      else
      {
        lcd.clear();
        lcd.setCursor(3, 1);
        lcd.print("Wrong Card");
        Serial.println("no");
        delay(1000);
        break; // Thoát khỏi vòng lặp sau khi quét thẻ thành công
      }
    }
    else if (key == '*')
    { // back
      Serial.println("Back");
      lcd.clear();
      break;
    }
  }
}

void Enter_product()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("# Save | * Back");
  lcd.setCursor(0, 1);
  lcd.print("No.Product:");
  lcd.setCursor(13, 1);
  lcd.print(data.product);
  int temp = data.product;
  while (true)
  {
    char key = customKeypad.getKey();
    if (key == '1')
    {
      temp = temp + 1;
      lcd.setCursor(13, 1);
      lcd.print(temp);
    }
    else if (key == '2')
    {
      if (temp > 0)
      {
        temp = temp - 1;
        lcd.setCursor(13, 1);
        lcd.print(temp);
      }
    }

    else if (key == '3')
    {
      temp = temp + 5;
      lcd.setCursor(13, 1);
      lcd.print(temp);
    }
    else if (key == '#')
    {
      // Nếu người dùng ấn '#', thoát khỏi vòng lặp
      lcd.clear();
      data.product = temp;
      writeStruct("/data.bin", data);
      Serial.println(data.product);
      break;
    }
    else if (key == '*')
    { // nhấn * thì thoát khỏi vòng lặp
      lcd.clear();
      break;
    }
  }
}

void Enter_Error()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("# Save | * Back");
  lcd.setCursor(0, 1);
  lcd.print("No.Error:");
  lcd.setCursor(10, 1);
  lcd.print(data.error);
  int temp = data.error;
  while (true)
  {
    char key = customKeypad.getKey();
    if (key == '1')
    {
      temp = temp + 1;
      lcd.setCursor(10, 1);
      lcd.print(temp);
    }
    else if (key == '2')
    {
      if (temp > 0)
      {
        temp = temp - 1;
        lcd.setCursor(10, 1);
        lcd.print(temp);
      }
    }
    else if (key == '3')
    {
      temp = temp + 5;
      lcd.setCursor(10, 1);
      lcd.print(temp);
    }
    else if (key == '#')
    {
      // Nếu người dùng ấn '#', thoát khỏi vòng lặp
      lcd.clear();
      data.error = temp;
      writeStruct("/data.bin", data);
      Serial.println(data.error);
      break;
    }
    else if (key == '*')
    { // nhấn * thì thoát khỏi vòng lặp
      lcd.clear();
      break;
    }
  }
}

void Job_run()
{
  Read_time();
  switch (customKey)
  {
  case 'C':
    Stop_work();
    break;
  case 'D':
    Enter_product();
    break;
  case 'B':
    Enter_Error();
    break;
  }
}

char *Mac_text(uint8_t dia_chi_mac[])
{
  static char combined_result[14];
  sprintf(combined_result, "lotoda32%02X%02X%02X", dia_chi_mac[3], dia_chi_mac[4], dia_chi_mac[5]);
  return combined_result;
}

void MAC_ID()
{
  uint8_t dia_chi_mac_wifi[6];
  WiFi.macAddress(dia_chi_mac_wifi);
  Serial.print("\nDefault ESP32 MAC Address: ");
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", dia_chi_mac_wifi[0], dia_chi_mac_wifi[1], dia_chi_mac_wifi[2],
                dia_chi_mac_wifi[3], dia_chi_mac_wifi[4], dia_chi_mac_wifi[5]);
  Device_ID = Mac_text(dia_chi_mac_wifi);
  Serial.println("Combined result: " + String(Device_ID));
}

void wifiConfig()
{
  writeFile("/ssid.txt", "");
  writeFile("/pass.txt", "");
  ESP.restart();
}

void connect_WiFi()
{
  if (ssid == "" || password == "")
  {
    WiFi.softAP(AC_SSID, AC_PASS);
    lcd.setCursor(0, 0);
    lcd.print("Config mode running");
    lcd.setCursor(0, 2);
    lcd.print("IP:");
    lcd.setCursor(5, 2);
    lcd.print(WiFi.softAPIP());
    server.WiFi_Config();
  }
  else
  {
    static uint8_t count_retry = 0;
    WiFi.begin(ssid, password);
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) // Try to connect wifi
    {
      delay(2000);
      count_retry++;
      Serial.print(".");
      if (count_retry >= 5)
      {
        lcd.clear();
        while (true)
        {
          char key = customKeypad.getKey();
          lcd.setCursor(0, 0);
          lcd.print("Connect Fail");
          lcd.setCursor(0, 1);
          lcd.print("Check Your Network!!");
          lcd.setCursor(0, 2);
          lcd.print("Press \'*\' to Retry ");
          lcd.setCursor(0, 3);
          lcd.print("Press \'#\' to Config");
          if (key == '*')
          {
            ESP.restart();
          }
          else if (key == '#')
          {
            wifiConfig();
          }
        }
      }
    }
  }
}

void setAdminPass()
{
  lcd.clear();
  data.Admin_Pass = ""; // Xoa Pass Cu
  uint8_t xpos = 2;
  while (data.Admin_Pass.length() < PasswordLength)
  {
    char key = customKeypad.getKey();
    lcd.setCursor(0, 0);
    lcd.print("Set PIN(4 Num)");
    if (key >= '0' && key <= '9')
    {
      data.Admin_Pass += key;
      lcd.setCursor(++xpos, 1);
      lcd.print("*"); // Display asterisk for each input (like in real passwords)
    }
    else if (key == '*')
    {
      break;
    }
  }
  lcd.clear();
  writeStruct("/data.bin", data);
}

void resetAll()
{
  data.Admin_Pass = "";
  data.Admin_UID = "";
  data.UID_Result = "";
  data.gio_start = 0;
  data.phut_start = 0;
  data.giay_start = 0;
  check = false;
  Status_start = true;
  cardScanned = false;
  passCorrect = false;
  writeStruct("/data.bin", data);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reseting...");
  delay(1000);
  ESP.restart();
}

void adminMenu()
{
  while (passCorrect)
  {
    char key = customKeypad.getKey();
    lcd.setCursor(0, 0);
    lcd.print("> 1.Set Password");
    lcd.setCursor(0, 1);
    lcd.print("> 2.Stop Worker");
    lcd.setCursor(0, 2);
    lcd.print("> 3.Change WiFi");
    lcd.setCursor(0, 3);
    lcd.print("> 4.Reset All");
    switch (key)
    {
    case '1':
      setAdminPass();
      break;
    case '2':
      Stop_work(); // Xử lý thêm phần stop bằng thẻ admin
      break;
    case '3':
      wifiConfig();
      break;
    case '4':
      resetAll();
      break;
    case '*':
      passCorrect = false;
      lcd.clear();
      break;
    }
  }
}

void detectAdmin()
{
  bool admin = false;
  if (customKey == '9')
  {
    admin = true;
  }
  if (getUID() || admin == true)
  {
    if (UID == data.Admin_UID || admin == true)
    {
      UID = "";
      lcd.clear();
      String inputPass;
      uint8_t xpos = 5;
      while (inputPass != data.Admin_Pass && inputPass.length() < PasswordLength)
      {
        char key = customKeypad.getKey();
        lcd.setCursor(7, 0);
        lcd.print("PIN:");
        if (key >= '0' && key <= '9')
        {
          inputPass += key;
          lcd.setCursor(++xpos, 1);
          lcd.print("*"); // Display asterisk for each input (like in real passwords)
        }
      }
      if (inputPass == data.Admin_Pass)
      {
        passCorrect = true;
        adminMenu();
      }
      else
      {
        lcd.clear();
        lcd.setCursor(7, 0);
        lcd.print("Wrong PIN");
        delay(1000);
        lcd.clear();
        passCorrect = false;
        UID = "";
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Setup_RFID();
  readStruct("/data.bin", data);
  Serial.println(data.Admin_UID);
  MAC_ID();
  lcd.backlight();
  lcd.begin();
  ssid = readFile(SPIFFS, "/ssid.txt");
  password = readFile(SPIFFS, "/pass.txt");
  Serial.printf("SSID: %s\n", ssid);
  Serial.printf("PASS: %s\n", password);
  lcd.clear();
  connect_WiFi(); // Kết nối wifi
  handleScan();   // Nhận lệnh scan wifi
  xTaskCreatePinnedToCore(
      scanWiFiNetworks, // Task function
      "WiFiScanTask",   // Task name
      10000,            // Stack size (bytes)
      NULL,             // Parameter passed to the task
      1,                // Task priority
      &scanTaskHandle,  // Task handle
      1                 // CPU core
  );
  while (WiFi.status() != WL_CONNECTED)
  {
    yield();
  } // do nothing if wifi connect fail
  Setup_MQTT();
  Setup_Time();
  lcd.clear();
  while (data.Admin_UID == "")
  {
    lcd.setCursor(3, 0);
    lcd.print("No Admin ID!");
    lcd.setCursor(0, 2);
    lcd.print("Please Scan Admin ID");
    if (getUID())
    {
      data.Admin_UID = UID;
      writeStruct("/data.bin", data);
      Serial.print("Admin UID: ");
      Serial.println(data.Admin_UID);
      setAdminPass();
      break;
    }
  }
  if (data.UID_Result == "")
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Welcome LOTODA");
    lcd.setCursor(1, 1);
    lcd.print("Press A start");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Welcome Back!");
    lcd.setCursor(3, 1);
    lcd.print("ID:");
    lcd.print(data.UID_Result);
    delay(3000);
    lcd.clear();
    check = true;
  }
}

void loop()
{
  customKey = customKeypad.getKey();
  if (check)
  {
    Job_run();
    MQTT_tele();
  }
  else // wait new worker
  {
    Start_Screen();
  }
  client.loop();
  detectAdmin();
}
