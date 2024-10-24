#ifndef _SPIFFS_H
#define _SPIFFS_H
#include <FS.h>
#include <SPIFFS.h>
#include "CONFIG.h"
void init_SPIFFS();
void writeFile(const char *path, const char *message);

String readFile(fs::SPIFFSFS &fs, const char *path);

void readStruct(const char *path, Data &data);
void writeStruct(const char *path, const Data &data);
#endif