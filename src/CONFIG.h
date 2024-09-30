#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct Data
{
    int gio_start, phut_start, giay_start;
    int product, error; // number of product
    String UID_Result;
    String Admin_UID;
    String Admin_Pass;
    String target;
};
extern Data data;
// extern String target;
extern String mess;

//--------- Time -----------//
extern int gio_end, phut_end, giay_end;
extern int gio_lam_viec, phut_lam_viec, giay_lam_viec;
extern String time_work_String;
extern String timeString, time_end_String, formattedDate, Device_ID;
extern bool check;
#endif
