#include "RFIDManager.h"
#include <SPI.h>
#include "CONFIG.h"
#define SS_PIN 5
#define RST_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);
char str[32] = "";
String UID;

void byteArray_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}

bool getUID()
{
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    mfrc522.PCD_Init();
  }
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }

  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID = str;
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return true;
}

bool getAdminUID()
{
  if (getUID())
  {
    data.Admin_UID = UID;
    return true;
  }
  return false;
}

void Setup_RFID()
{
  SPI.begin();
  mfrc522.PCD_Init();
  delay(500);
}
