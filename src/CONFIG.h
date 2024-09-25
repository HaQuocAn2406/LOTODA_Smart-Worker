#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct Data
{

    int product, error; // number of product
    String UID_Result;
};
static Data data;

extern String target;
extern String mess;

//--------- Time -----------//
extern int gio_start, phut_start, giay_start, gio_end, phut_end, giay_end;
extern int gio_lam_viec, phut_lam_viec, giay_lam_viec;
extern String time_work_String;

//-------- RFID -----------//
extern String UID_Result;

extern String timeString, time_end_String, formattedDate, Device_ID;

extern String worker_UID;
extern bool check;
extern int result_product, result_error;

#endif
