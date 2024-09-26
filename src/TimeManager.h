#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <NTPClient.h>
#include <WiFiUdp.h>

extern NTPClient timeClient;

void Setup_Time();
void Get_time_Start();
void Time_start();
void Get_time_End();
void Working_Time(int eer_gio_read, int eer_phut_read, int eer_giay_read);

#endif
