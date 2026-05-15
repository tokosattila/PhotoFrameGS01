#ifndef RENAME_COMMAND_CLASS
#define RENAME_COMMAND_CLASS

#include <App/Telnet/Command.h>

namespace App {

  class RenameCommand_ : public Command_ {
    public:
      const char *GetName() const override {
        return "rename";
      }
      bool Execute(const char *tArguments, WiFiClient &tClient) override {
        char tFrom[256] = "";
        char tTo[256] = "";
        if (!ParseArgs(tArguments, tFrom, sizeof(tFrom), tTo, sizeof(tTo))) {
          PrintUsage(tClient);
          return true;
        }
        if (strcmp(tFrom, tTo) == 0) {
          tClient.print(F(COLOR_RED "\r\n  Error: Source and destination are the same\r\n\r\n" COLOR_WHITE));
          return true;
        }
        if (!LFS.IsMounted()) {
          tClient.print(F(COLOR_RED "\r\n  LittleFS is not mounted.\r\n\r\n" COLOR_WHITE));
          return true;
        }
        bool tSourceExists = LFS.Exists(tFrom);
        if (!tSourceExists) {
          tClient.print(F(COLOR_RED "\r\n  Error: source not found\r\n\r\n" COLOR_WHITE));
          return true;
        }
        bool tDestExists = LFS.Exists(tTo);
        if (tDestExists) {
          tClient.print(F(COLOR_RED "\r\n  Error: destination already exists\r\n\r\n" COLOR_WHITE));
          return true;
        }
        bool tOk = LFS.RenameFile(tFrom, tTo);
        if (tOk) tClient.printf(COLOR_GREEN "\r\n  Renamed: %s -> %s\r\n\r\n" COLOR_WHITE, tFrom, tTo);
        else tClient.print(F(COLOR_RED "\r\n  Error: rename failed\r\n\r\n" COLOR_WHITE));
        return true;
      }
      const char *Help() const override {
        return "rename [source] [destination]     " COLOR_YELLOW "- rename on LittleFS" COLOR_WHITE;
      }
    private:
      bool ParseArgs(const char *tInput, char *tFrom, size_t tFromSize, char *tTo, size_t tToSize) {
        if (!tInput) return false;
        const char *tP = tInput;
        while (*tP && *tP != ' ' && *tP != '\t') ++tP;
        while (*tP == ' ' || *tP == '\t') ++tP;
        const char *tS = tP;
        while (*tP && *tP != ' ' && *tP != '\t') ++tP;
        if (tP == tS) return false;
        size_t tLen = tP - tS;
        if (tLen >= tFromSize) return false;
        strncpy(tFrom, tS, tLen);
        tFrom[tLen] = '\0';
        while (*tP == ' ' || *tP == '\t') ++tP;
        tS = tP;
        while (*tP && *tP != ' ' && *tP != '\t') ++tP;
        if (tP == tS) return false;
        tLen = tP - tS;
        if (tLen >= tToSize) return false;
        strncpy(tTo, tS, tLen);
        tTo[tLen] = '\0';
        while (*tP == ' ' || *tP == '\t') ++tP;
        return *tP == '\0';
      }
      void PrintUsage(WiFiClient &tClient) {
        tClient.print(F(COLOR_YELLOW "\r\n  Usage: rename <source> <destination>\r\n"));
        tClient.print(F("  Source/Destination: path within LittleFS\r\n\r\n" COLOR_WHITE));
      }
  };

}

#endif