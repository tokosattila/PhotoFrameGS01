#ifndef FORMAT_COMMAND_CLASS
#define FORMAT_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class FormatCommand_ : public Command_ {
    public:
      const char *GetName() const override {
        return "format";
      }
      bool Execute(const char *tArguments, WiFiClient &tClient) override {
        if (!LFS.IsMounted()) {
          tClient.print(F(COLOR_RED "\r\n  LittleFS is not mounted.\r\n\r\n" COLOR_WHITE));
          return true;
        }
        tClient.printf(COLOR_YELLOW "\r\n  Warning:" COLOR_WHITE " this will erase all files and directories on %s\r\n", LFS.GetName());
        tClient.print(F("\r\n  This operation cannot be undone.\r\n\r\n"));
        char tPrompt[64] = "";
        snprintf(tPrompt, sizeof(tPrompt), "Format %s? (y/n): ", LFS.GetName());
        TLN.RequestConfirmation(tPrompt, [](bool tConfirmed, WiFiClient &tConfirmClient) {
          if (!tConfirmed) {
            tConfirmClient.print(F("\r\n  Cancelled\r\n\r\n"));
            return;
          }
          bool tOk = LFS.Format();
          if (tOk) {
            if (!CFG.SaveImageName("")) {
              tConfirmClient.print(F(COLOR_YELLOW "\r\n  Warning: failed to clear image name after format.\r\n" COLOR_WHITE));
            }
          }
          if (!tOk) tConfirmClient.print(F(COLOR_RED "\r\n  Error: format failed\r\n\r\n" COLOR_WHITE));
          else tConfirmClient.print(F(COLOR_GREEN "\r\n  Format complete\r\n\r\n" COLOR_WHITE));
        });
        return true;
      }
      const char *Help() const override {
        return "format                            " COLOR_YELLOW "- erase all content on LittleFS (asks y/n)" COLOR_WHITE;
      }
    private:
      void PrintUsage(WiFiClient &tClient) {
        tClient.print(F(COLOR_YELLOW "\r\n  Usage: format\r\n\r\n" COLOR_WHITE));
      }
  };

}

#endif