#ifndef EEPROMMANAGER_H
#define EEPROMMANAGER_H

#include <Arduino.h>

#define EEPROM_SIZE 512

void saveStringToEEPROM(String uid, int startAddress);
void saveValueToEEPROM(int value, int address);
int ReadValue_EEPROM(int Address);
void Clear_EEPROM();
// void saveToEEPROM(String uid, int startAddress);
String Read_EEPROM(char Address);
void readFull_EEPROMData();
void EEPROM_begin();

#endif
