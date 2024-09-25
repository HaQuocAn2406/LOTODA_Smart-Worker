#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

extern String target;
extern String mess;

//--------- Time -----------//
extern int gio_start, phut_start, giay_start, gio_end, phut_end, giay_end;
extern int gio_lam_viec, phut_lam_viec, giay_lam_viec;
extern String time_work_String;

//-------- RFID -----------//
extern String UID_Result;


extern String timeString,time_end_String,formattedDate,Device_ID;

extern int eer_gio_read, eer_phut_read, eer_giay_read;
extern String gio_string, phut_string, giay_string, worker_UID;
extern bool check;
extern int result_product, result_error;

#endif
