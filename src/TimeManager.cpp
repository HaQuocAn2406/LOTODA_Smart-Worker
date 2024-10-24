#include "CONFIG.h"
#include "TimeManager.h"
#include "_SPIFFS.h"
const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int gio_end, phut_end, giay_end;
int gio_lam_viec, phut_lam_viec, giay_lam_viec;
String time_work_String;

void Setup_Time()
{
    timeClient.begin();
    timeClient.setTimeOffset(25200); // ofsfet UTC +7
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
}

// void Get_Date()
// {
//     while (!timeClient.update())
//     {
//         timeClient.forceUpdate();
//     }
//     formattedDate = timeClient.getFormattedDate();
//     Serial.print("Date: ");
//     Serial.println(formattedDate);
// }

void Get_time_Start()
{
    timeClient.update();
    data.gio_start = timeClient.getHours();
    data.phut_start = timeClient.getMinutes();
    data.giay_start = timeClient.getSeconds();
    data.timeStart = timeClient.getFormattedTime();
    uint8_t index = timeClient.getFormattedDate().indexOf('T');
    data.subTime = timeClient.getFormattedDate().substring(0, index + 1);
    Serial.println(data.subTime + data.timeStart);
    timeString = data.subTime + data.timeStart;
    Serial.print("time_start: ");
    Serial.print(data.gio_start);
    Serial.print(":");
    Serial.print(data.phut_start);
    Serial.print(":");
    Serial.println(data.giay_start);
}

void Get_time_End()
{
    timeClient.update();
    gio_end = timeClient.getHours();
    phut_end = timeClient.getMinutes();
    giay_end = timeClient.getSeconds();
    time_end_String = timeClient.getFormattedDate();
    Serial.print("time_end");
    Serial.print(":");
    Serial.print(gio_end);
    Serial.print(":");
    Serial.print(phut_end);
    Serial.print(":");
    Serial.println(giay_end);
}

void Working_Time(int gio_start, int phut_start, int giay_start)
{
    int thoi_gian_bat_dau = gio_start * 3600 + phut_start * 60 + giay_start;
    int thoi_gian_ket_thuc = gio_end * 3600 + phut_end * 60 + giay_end;
    int thoi_gian_lam_viec = thoi_gian_ket_thuc - thoi_gian_bat_dau;
    if (thoi_gian_lam_viec < 0)
    {
        thoi_gian_lam_viec += 24 * 3600; // Nếu thời gian làm việc kết thúc vào ngày tiếp theo
    }
    gio_lam_viec = thoi_gian_lam_viec / 3600;
    phut_lam_viec = (thoi_gian_lam_viec % 3600) / 60;
    giay_lam_viec = thoi_gian_lam_viec % 60;
    time_work_String = String(thoi_gian_lam_viec);
}
