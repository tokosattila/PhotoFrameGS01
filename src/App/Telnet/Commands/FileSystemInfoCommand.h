#ifndef FILESYSTEMINFO_COMMAND_CLASS
#define FILESYSTEMINFO_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class FileSystemInfoCommand_ : public Command_ {
    public:
      const char *GetName() const override {
        return "fsinfo";
      }
      bool Execute(const char *tArguments, WiFiClient& tClient) override {
        char tText[48] = "";
        char tUsedBuffer[16] = "";
        char tTotalBuffer[16] = "";
        tClient.print(F(COLOR_WHITE "\r\n"));
        UTL.ByteToReadableSize(LFS.UsedBytes(), tUsedBuffer, sizeof(tUsedBuffer));
        UTL.ByteToReadableSize(LFS.TotalBytes(), tTotalBuffer, sizeof(tTotalBuffer));
        snprintf(tText, sizeof(tText), "  LittleFS: %s / %s", tUsedBuffer, tTotalBuffer);
        tClient.println(tText);
        tClient.print(F("\r\n"));
        return true;
      }
      const char *Help() const override {
        return "fsinfo                 - show filesystem usage info";
      }
    };

}
#endif