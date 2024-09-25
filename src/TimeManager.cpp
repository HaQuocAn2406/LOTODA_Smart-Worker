#include "CONFIG.h"
#include "TimeManager.h"

const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

extern int gio_start, phut_start, giay_start;
int gio_end, phut_end, giay_end;
int gio_lam_viec, phut_lam_viec, giay_lam_viec;
String time_work_String;

void Setup_Time() {
    timeClient.begin();
    timeClient.setTimeOffset(+7 * 60 * 60);
}

void Get_time_Start() {
    timeClient.update();
    gio_start = timeClient.getHours();
    phut_start = timeClient.getMinutes();
    giay_start = timeClient.getSeconds();
}

void Get_time_End() {
    timeClient.update();
    gio_end = timeClient.getHours();
    phut_end = timeClient.getMinutes();
    giay_end = timeClient.getSeconds();
}

void Working_Time(int eer_gio_read, int eer_phut_read, int eer_giay_read) {
    int thoi_gian_bat_dau = eer_gio_read * 3600 + eer_phut_read * 60 + eer_giay_read;
    int thoi_gian_ket_thuc = gio_end * 3600 + phut_end * 60 + giay_end;

    int thoi_gian_lam_viec = thoi_gian_ket_thuc - thoi_gian_bat_dau;
    Serial.println(thoi_gian_lam_viec);
    if (thoi_gian_lam_viec < 0) {
        thoi_gian_lam_viec += 24 * 3600; // Nếu thời gian làm việc kết thúc vào ngày tiếp theo
    }

    gio_lam_viec = thoi_gian_lam_viec / 3600;
    phut_lam_viec = (thoi_gian_lam_viec % 3600) / 60;
    giay_lam_viec = thoi_gian_lam_viec % 60;
    time_work_String = String(gio_lam_viec) + ":" + String(phut_lam_viec) + ":" + String(giay_lam_viec);
}


