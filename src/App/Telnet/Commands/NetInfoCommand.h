#ifndef NETINFO_COMMAND_CLASS
#define NETINFO_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class NetInfoCommand_ : public Command_ {
    public:
      const char *GetName() const override { 
        return "netinfo"; 
      }
      bool Execute(const char *tArguments, WiFiClient &tClient) override {
        tClient.print(F("\r\n  " COLOR_YELLOW "IP address:" COLOR_WHITE " "));
        tClient.println(mWiFi.localIP());
        tClient.print(F("  " COLOR_YELLOW "Subnet mask:" COLOR_WHITE " "));
        tClient.println(mWiFi.subnetMask());
        tClient.print(F("  " COLOR_YELLOW "Gateway:" COLOR_WHITE " "));
        tClient.println(mWiFi.gatewayIP());
        tClient.print(F("  " COLOR_YELLOW "DNS 1:" COLOR_WHITE " "));
        tClient.println(mWiFi.dnsIP(0));
        tClient.print(F("  " COLOR_YELLOW "DNS 2:" COLOR_WHITE " "));
        tClient.println(mWiFi.dnsIP(1));
        tClient.print(F("  " COLOR_YELLOW "MAC address:" COLOR_WHITE " "));
        tClient.println(mWiFi.macAddress());
        tClient.print(F("\r\n"));
        return true;
      }
      const char *Help() const override {
        return "netinfo                - show network info";
      }
    private:
      WiFiClass mWiFi;
  };

}

#endif
