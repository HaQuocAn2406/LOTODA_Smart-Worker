#pragma once
#ifndef SEVER_H
#define SEVER_H
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <SPIFFS.h>
class LOTODA_Server : public AsyncWebServer
{
private:
    int LED_state;
    void writeFile(const char *path, const char *message);
    String readFile(fs::SPIFFSFS &fs, const char *path);
    void Handle_Network();
    void Handle_NetworkAP();
    void Handle_SubmitForm();
    void Handle_Reboot();
public:
    LOTODA_Server(int value = 80) : AsyncWebServer(value),LED_state(0)
    {
        SPIFFS.begin();
    };
    void WiFi_Config();
};

#endif