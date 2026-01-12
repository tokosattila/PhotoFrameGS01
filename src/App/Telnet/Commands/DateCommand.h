#ifndef DATE_COMMAND_CLASS
#define DATE_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class DateCommand_ : public Command_ {
    public:
      const char *GetName() const override { 
        return "date"; 
      }
      bool Execute(const char *tArguments, WiFiClient &tClient) override {
        if (!TME.IsInternetAvailable()) {
          tClient.print(F(COLOR_RED "\r\n  Error: NTP unavailable in AP mode\r\n" COLOR_YELLOW));
          tClient.print(F("  Switch to STA mode to use time synchronization\r\n\r\n" COLOR_WHITE));
          return true;
        }
        TME.Init();
        TME.SyncSystemTime();
        char tDate[16];
        char tTime[16];
        TME.GetDate(tDate, sizeof(tDate));
        TME.GetTime(tTime, sizeof(tTime));
        tClient.print(F("\r\n  Date: "));
        tClient.print(tDate);
        tClient.print(F("\r\n  Time: "));
        tClient.print(tTime);
        tClient.print(F("\r\n\r\n"));
        TME.End();
        return true;
      }
      const char *Help() const override {
        return "date                   - show current date and time";
      }
  };

}

#endif