
#include <LCD_I2C.h>
#include <WiFi.h>
#include "MQTT_Client.h"
#include "CONFIG.h"
#include "TimeManager.h"
#include "RFIDManager.h"
#include "EEPROMManager.h"
#include "KeypadManager.h"
#include "sever.h"
//----------------------------------------
LCD_I2C lcd(0x3F, 20, 4); // Default address of most PCF8574 modules, change according
                          // Variable
LOTODA_Server server;

bool scanning = false;
String wifiScanResult = "[]";

int readsuccess;
char customKey;
bool cardScanned = false;
bool writingToEEPROM = false;
bool Status_star = true;
int gio, phut, giay;
// String result_product = "0", result_error = "0";
int result_product = 0;
int result_error = 0;
String timeString, time_end_String;
String formattedDate;
String data_time_start = "";
char *Mac_text(uint8_t dia_chi_mac[]);
String Device_ID;
bool newDataReceived = true;// Not used
String ssid = "", password = "";
String AC_SSID = "LOTODA WiFi Config";
String AC_PASS = "12345678";

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

void Read_Start()
{
  if (Status_star && customKey == 'A')
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Moi quet the");
    while (true)
    {
      readsuccess = getUID();
      if (readsuccess)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Ma the:");
        lcd.print(UID_Result);
        cardScanned = true;
        lcd.setCursor(0, 2);
        lcd.print("Press B continue");
        delay(1000);
        break; // Thoát khỏi vòng lặp khi đã đọc thành công thẻ
      }
    }
  }
}

void Time_start()
{
  Get_time_Start(); // Thời gian bắt đầu vào ca làm
  timeString = String(gio_start) + ":" + String(phut_start) + ":" + String(giay_start);
  saveValueToEEPROM(gio_start, 10);
  saveValueToEEPROM(phut_start, 15);
  saveValueToEEPROM(giay_start, 20);
  readFull_EEPROMData();
}

void Get_Date()
{
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  Serial.printf("Date: %s\n",formattedDate);
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
  Working_Time(eer_gio_read, eer_phut_read, eer_giay_read);
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

void scrollMessage(int row, String message, int delayTime, int totalColumns)
{

  for (int i = 0; i < totalColumns; i++)
  {

    message = " " + message;
  }

  message = message + " ";

  for (int position = 0; position < message.length(); position++)
  {

    lcd.setCursor(0, row);

    lcd.print(message.substring(position, position + totalColumns));

    delay(delayTime);
  }
}

void Working_Screen()
{
  lcd.setCursor(0, 0);
  lcd.print("ID:");
  lcd.print(worker_UID);
  lcd.setCursor(11, 0);
  lcd.print("  ");
  lcd.setCursor(13, 0);
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
  lcd.setCursor(0, 1);
  lcd.print("D sp: ");
  if (result_product  >= 999)
  {
    lcd.setCursor(5, 1);
  }
  lcd.print(result_product);
  lcd.setCursor(9, 1);
  lcd.print("|");
  lcd.setCursor(11, 1);
  lcd.print("B Er: ");
  if (result_error >= 999)
  {
    lcd.setCursor(16, 1);
  }
  lcd.print(result_error);
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

void Save_EEPROM()
{
  if (cardScanned && (customKey == 'B'))
  {
    saveStringToEEPROM(UID_Result, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loading......");
    delay(3000);
    Time_start();
    Get_Date();
    push_mqtt1(formattedDate, Device_ID, UID_Result, timeString, time_end_String, time_work_String, result_product, result_error);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("DA XONG");
    Status_star = false;
    cardScanned = false; // Reset trạng thái đã quét thẻ
    delay(2000);
    check = true;
  }
}

void Stop_work()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan to stop");
  lcd.setCursor(2, 1);
  lcd.print("Sp:");
  lcd.setCursor(6, 1);
  lcd.print(result_product);
  lcd.setCursor(12, 1);
  lcd.print("pcs");
  lcd.setCursor(2, 2);
  lcd.print("Error:");
  lcd.setCursor(9, 2);
  lcd.print(result_error);
  lcd.setCursor(12, 2);
  lcd.print("pcs");
  lcd.setCursor(0, 3);
  lcd.print(" * Back");
  while (true)
  {
    char key = customKeypad.getKey();
    readsuccess = getUID();
    if (readsuccess)
    {
      Serial.println(readsuccess);
      lcd.clear();
      if (worker_UID == UID_Result)
      {
        Time_end();
        Time_work();
        Get_Date();
        push_mqtt1(formattedDate, Device_ID, UID_Result, timeString, time_end_String, time_work_String, result_product, result_error);
        Clear_EEPROM();
        worker_UID = "";
        UID_Result = "";
        gio_string = "";
        phut_string = "";
        result_error = 0;
        result_product = 0;
        giay_string = "";
        time_end_String = "";
        time_work_String = "";
        delay(5000);
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Welcome LOTODA");
        lcd.setCursor(1, 1);
        lcd.print("Press A start");
        check = false;
        Status_star = true;
        Serial.println("yes");
        break; // Thoát khỏi vòng lặp sau khi quét thẻ thành công
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("The sai");
        Serial.println("no");
        delay(1000);
        break; // Thoát khỏi vòng lặp sau khi quét thẻ thành công
      }
    }
    else if (key == '*')
    { // thoát khỏi vòng lăp
      Serial.println("khac");
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
  lcd.print("So sp:");
  lcd.setCursor(8, 1);
  lcd.print(result_product);
  while (true)
  {
    char key = customKeypad.getKey();
    if (key == '1')
    {
      result_product = result_product + 1;
      lcd.setCursor(8, 1);
      lcd.print(result_product);
    }
    else if (key == '2')
    {
      if (result_product > 0)
      {
        result_product = result_product - 1;
        lcd.setCursor(8, 1);
        lcd.print("            ");
        lcd.setCursor(8, 1);
        lcd.print(result_product);
      }
    }

    else if (key == '3')
    {
      result_product = result_product + 5;
      lcd.setCursor(8, 1);
      lcd.print(result_product);
    }
    else if (key == '#')
    {
      // Nếu người dùng ấn '#', thoát khỏi vòng lặp
      lcd.clear();
      saveValueToEEPROM(result_product, 24);
      Serial.println(result_product);
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
  lcd.print("Sp Error:");
  lcd.setCursor(10, 1);
  lcd.print(result_error);
  while (true)
  {
    char key = customKeypad.getKey();
    if (key == '1')
    {
      result_error = result_error + 1;
      lcd.setCursor(10, 1);
      lcd.print(result_error);
    }
    else if (key == '2')
    {
      if (result_error > 0)
      {
        result_error = result_error - 1;
        lcd.setCursor(10, 1);
        lcd.print("            ");
        lcd.setCursor(10, 1);
        lcd.print(result_error);
      }
    }
    else if (key == '3')
    {
      result_error = result_error + 5;
      lcd.setCursor(10, 1);
      lcd.print(result_error);
    }
    else if (key == '#')
    {
      // Nếu người dùng ấn '#', thoát khỏi vòng lặp
      lcd.clear();
      saveValueToEEPROM(result_error, 30);
      Serial.println(result_error);
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
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", dia_chi_mac_wifi[0], dia_chi_mac_wifi[1], dia_chi_mac_wifi[2], dia_chi_mac_wifi[3], dia_chi_mac_wifi[4], dia_chi_mac_wifi[5]);
  Device_ID = Mac_text(dia_chi_mac_wifi);
  Serial.println("Combined result: " + String(Device_ID));
}

void writeFile(const char *path, const char *message)
{
  File file = SPIFFS.open(path, FILE_WRITE);

  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  if (file.print(message))
  {
    Serial.println("File written successfully");
  }
  else
  {
    Serial.println("Failed to write file");
  }

  file.close(); // Don't forget to close the file
}

String readFile(fs::SPIFFSFS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

void connect_WiFi()
{
  if (ssid == "" || password == "")
  {
    WiFi.softAP(AC_SSID, AC_PASS);
    lcd.setCursor(0, 0);
    lcd.print("Config mode running");
    server.WiFi_Config();
  }
  else
  {
    static uint8_t count_retry = 0;
    WiFi.begin(ssid, password);
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
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
            writeFile("/ssid.txt", "");
            writeFile("/pass.txt", "");
            ESP.restart();
          }
        }
      }
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM_begin();
  Clear_EEPROM();
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
  while (WiFi.status() != WL_CONNECTED) // kh làm gì tới khi kết nối wifi thành công
  {
    yield();
  }
  Setup_MQTT();
  Setup_Time();
  Setup_RFID();
  readFull_EEPROMData();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading....");
  delay(1000);
  lcd.clear();
  result_product = ReadValue_EEPROM(24);
  result_error = ReadValue_EEPROM(30);
  if (check == false)
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
    lcd.print(worker_UID);
    delay(3000);
    lcd.clear();
  }
}

void loop()
{
  if (!client.connected())
  {
    connect_to_broker();
  }
  client.loop();
  customKey = customKeypad.getKey();
  if (check)
  {
    Job_run();
    MQTT_tele();
  }
  else
  {
    Read_Start();
    Save_EEPROM();
  }
  // delay(100);
}
