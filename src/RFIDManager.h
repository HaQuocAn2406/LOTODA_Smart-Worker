#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <MFRC522.h>

extern String UID;
void Setup_RFID();
bool getUID();
bool getAdminUID();
#endif
