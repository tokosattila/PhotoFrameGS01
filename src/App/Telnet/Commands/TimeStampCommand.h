#ifndef TIMESTAMP_COMMAND_CLASS
#define TIMESTAMP_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class TimeStampCommand_ : public Command_ {
    public:
      const char *GetName() const override { 
        return "timestamp"; 
      }
      bool Execute(const char *tArguments, WiFiClient &tClient) override {
        if (!TME.IsInternetAvailable()) {
          tClient.print(F(COLOR_RED "\r\n  Error: NTP unavailable in AP mode\r\n" COLOR_YELLOW));
          tClient.print(F("  Switch to STA mode to use time synchronization\r\n\r\n" COLOR_WHITE));
          return true;
        }
        TME.Init();
        TME.SyncSystemTime();
        char tTimeStamp[16];
        tClient.print(F("\r\n  Timestamp: "));
        tClient.print(TME.GetCurrentEpoch());
        tClient.print(F("\r\n\r\n"));
        TME.End();
        return true;
      }
      const char *Help() const override {
        return "timestamp              - show current timestamp";
      }
  };

}

#endif