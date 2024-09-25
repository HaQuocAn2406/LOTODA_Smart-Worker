#include "EEPROMManager.h"
#include <EEPROM.h>

int eer_gio_read, eer_phut_read, eer_giay_read;
String gio_string, phut_string, giay_string, worker_UID;
int gio_start = 0, phut_start = 0, giay_start = 0;
bool check;

void EEPROM_begin()
{
  EEPROM.begin(EEPROM_SIZE);
}

void saveStringToEEPROM(String uid, int startAddress)
{
  // Ghi chuỗi UID vào EEPROM từ địa chỉ startAddress
  for (int i = 0; i < uid.length(); ++i)
  {
    EEPROM.write(startAddress + i, uid[i]);
  }
  EEPROM.commit();
  // readEEPROMData();
}

void saveValueToEEPROM(int value, int address)
{
  EEPROM.writeInt(address, value);
  EEPROM.commit();
}

void Clear_EEPROM()
{
  for (int i = 0; i < EEPROM_SIZE; ++i)
  {
    EEPROM.write(i, 0);
    delay(10);
  }
  EEPROM.commit();
}

int ReadValue_EEPROM(int Address)
{
  return EEPROM.readInt(Address);
}
String ReadString_EEPROM(int Address)
{
  char data[100]; // Số lượng ký tự tối đa cho chuỗi là 100
  int len = 0;
  char k = EEPROM.read(Address);

  while (k != '\0' && len < 100)
  { // Đọc cho đến khi gặp ký tự null hoặc đạt đến giới hạn ký tự
    data[len] = k;
    len++;
    k = EEPROM.read(Address + len);
  }
  data[len] = '\0';
  return String(data);
  Serial.println(data);
}

void readFull_EEPROMData()
{
  if (EEPROM.read(0) != 0)
  {
    for (int i = 0; i < 8; ++i)
    {
      worker_UID += char(EEPROM.read(i));
    }
    gio_start = EEPROM.readInt(10);
    phut_start = EEPROM.readInt(15);
    giay_start = EEPROM.readInt(20);

    worker_UID = worker_UID.c_str();
    gio_string = gio_string.c_str();
    phut_string = phut_string.c_str();
    giay_string = giay_string.c_str();

    eer_gio_read = gio_start;
    eer_phut_read = phut_start;
    eer_giay_read = giay_start;
    Serial.println(worker_UID);
    Serial.print("time_start: ");
    Serial.print(eer_gio_read);
    Serial.print(":");
    Serial.print(eer_phut_read);
    Serial.print(":");
    Serial.println(eer_giay_read);
    check = true;
  }
}
