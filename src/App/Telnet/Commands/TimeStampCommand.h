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