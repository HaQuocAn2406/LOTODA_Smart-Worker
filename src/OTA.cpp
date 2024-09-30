#include "OTA.h"
#include <LCD_I2C.h>
void init_OTA()
{
    OTADRIVE.setInfo(APIKEY, "v@1.0.5");
    OTADRIVE.onUpdateFirmwareProgress(onUpdateProgress);
}
__attribute__((weak)) void checkNewVersion()
{
    updateInfo inf = OTADRIVE.updateFirmwareInfo();
    Serial.printf("\nfirmware info: %s ,%dBytes\n%s\n",
                  inf.version.c_str(), inf.size, inf.available ? "New version available" : "No newer version");
    if (inf.available)
        OTADRIVE.updateFirmware();
    OTADRIVE.syncResources();
}
__attribute__((weak)) void onUpdateProgress(int progress, int totalt)
{
  static int last = 0;
  int progressPercent = (100 * progress) / totalt;
  Serial.print("*");
  if (last != progressPercent && progressPercent % 10 == 0)
  {
    //print every 10%
    Serial.printf("%d", progressPercent);
  }
  last = progressPercent;
}