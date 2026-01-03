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
        tClient.print(F("\r\n  IP address: "));
        tClient.println(mWiFi.localIP());
        tClient.print(F("  Subnet mask: "));
        tClient.println(mWiFi.subnetMask());
        tClient.print(F("  Gateway: "));
        tClient.println(mWiFi.gatewayIP());
        tClient.print(F("  DNS 1: "));
        tClient.println(mWiFi.dnsIP(0));
        tClient.print(F("  DNS 2: "));
        tClient.println(mWiFi.dnsIP(1));
        tClient.print(F("  MAC address: "));
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
