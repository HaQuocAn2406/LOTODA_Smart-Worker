#ifndef OTA_H
#define OTA_H
#include <otadrive_esp.h>

#define APIKEY "b182dce6-64e4-4650-972c-588489ec0fcc"

void init_OTA();
void onUpdateProgress(int progress, int totalt);
void checkNewVersion();
#endif