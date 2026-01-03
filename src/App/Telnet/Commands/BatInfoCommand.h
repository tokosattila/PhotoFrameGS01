#ifndef BATINFO_COMMAND_CLASS
#define BATINFO_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class BatInfoCommand_ : public Command_ {
    public:
      const char *GetName() const override {
        return "batinfo";
      }
      bool Execute(const char *tArguments, WiFiClient& tClient) override {
        tClient.print(F(COLOR_WHITE "\r\n"));
        if (!UTL.MeasureBattery()) {
          tClient.println("  " COLOR_YELLOW "Battery:" COLOR_WHITE " LOW or not detected!");
          return false;
        }
        char tText[64];
        snprintf(tText, sizeof(tText), "  " COLOR_YELLOW "Battery:" COLOR_WHITE " %.2fV [%d%%]", UTL.mBatteryVoltage, UTL.mBatteryPercentage);
        tClient.println(tText);
        tClient.print(F("\r\n"));
        return true;
      }
      const char *Help() const override {
        return "batinfo                - show battery voltage and percentage";
      }
  };

}

#endif
