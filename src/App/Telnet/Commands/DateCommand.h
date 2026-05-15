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
        const char *tPtr = tArguments ? tArguments : "";
        while (*tPtr == ' ' || *tPtr == '\t') ++tPtr;
        while (*tPtr != '\0' && *tPtr != ' ' && *tPtr != '\t') ++tPtr;
        while (*tPtr == ' ' || *tPtr == '\t') ++tPtr;
        const char *tSubStart = tPtr;
        while (*tPtr != '\0' && *tPtr != ' ' && *tPtr != '\t') ++tPtr;
        size_t tSubLen = tPtr - tSubStart;
        while (*tPtr == ' ' || *tPtr == '\t') ++tPtr;
        if (tSubLen == 0) return HandleSystemDateTime(tClient);
        if (tSubLen == 3 && strncasecmp(tSubStart, "set", 3) == 0) return HandleSet(tPtr, tClient);
        tClient.print(F(COLOR_RED "\r\n  Error: unknown subcommand\r\n" COLOR_YELLOW));
        tClient.print(F("  Usage: date [set YYYY.MM.DD HH:MM:SS]\r\n\r\n" COLOR_WHITE));
        return true;
      }
      const char *Help() const override {
        return "date                             " COLOR_YELLOW " - show system local date and time" COLOR_WHITE "\r\n  "
               "date set YYYY.MM.DD HH:MM:SS     " COLOR_YELLOW " - set system local date and time" COLOR_WHITE;
      }
    private:
      static time_t ToLocalTime(time_t tUtc) {
        const SNTPConfig tCfg = CFG.Get<SNTPConfig>();
        return (time_t)((unsigned long)tUtc + (unsigned long)tCfg.GMTOffset);
      }
      static time_t MakeUtcEpoch(int tY, int tMo, int tD, int tH, int tMi, int tS) {
        static const int kDaysBeforeMonth[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
        long tYears = tY - 1970;
        long tLeaps = 0;
        for (int y = 1970; y < tY; ++y) {
          if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ++tLeaps;
        }
        long tDays = tYears * 365 + tLeaps + kDaysBeforeMonth[tMo - 1] + (tD - 1);
        bool tIsLeap = ((tY % 4 == 0 && tY % 100 != 0) || tY % 400 == 0);
        if (tIsLeap && tMo > 2) tDays += 1;
        return (time_t)(tDays * 86400L + tH * 3600L + tMi * 60L + tS);
      }
      bool HandleSet(const char *tArgs, WiFiClient &tClient) {
        int tY = 0, tMo = 0, tD = 0, tH = 0, tMi = 0, tS = 0;
        int tParsed = sscanf(tArgs, "%d.%d.%d %d:%d:%d", &tY, &tMo, &tD, &tH, &tMi, &tS);
        if (tParsed != 6 || tY < 2025 || tY > 2099 || tMo < 1 || tMo > 12 || tD < 1 || tD > 31 ||
            tH < 0 || tH > 23 || tMi < 0 || tMi > 59 || tS < 0 || tS > 59) {
          tClient.print(F(COLOR_RED "\r\n  Error: invalid date/time format\r\n" COLOR_YELLOW));
          tClient.print(F("  Usage: date set YYYY.MM.DD HH:MM:SS\r\n\r\n" COLOR_WHITE));
          return true;
        }
        time_t tLocalAsIfUtc = MakeUtcEpoch(tY, tMo, tD, tH, tMi, tS);
        const SNTPConfig tCfg = CFG.Get<SNTPConfig>();
        time_t tUtc = (time_t)((long)tLocalAsIfUtc - (long)tCfg.GMTOffset);
        struct timeval tTv = { .tv_sec = tUtc, .tv_usec = 0 };
        if (settimeofday(&tTv, nullptr) != 0) {
          tClient.print(F(COLOR_RED "\r\n  Error: settimeofday failed\r\n\r\n" COLOR_WHITE));
          return true;
        }
        tClient.print(F(COLOR_GREEN "\r\n  System time set (local).\r\n\r\n" COLOR_WHITE));
        return HandleSystemDateTime(tClient);
      }
      bool HandleSystemDateTime(WiFiClient &tClient) {
        time_t tNowUtc;
        time(&tNowUtc);
        time_t tNowLocal = ToLocalTime(tNowUtc);
        struct tm tTimeInfo;
        gmtime_r(&tNowLocal, &tTimeInfo);
        tClient.print(F(COLOR_GREEN "\r\n  System DateTime (local):\r\n" COLOR_WHITE));
        char tBuffer[16];
        strftime(tBuffer, sizeof(tBuffer), "%Y.%m.%d", &tTimeInfo);
        tClient.print(F("\r\n  Date: "));
        tClient.print(tBuffer);
        strftime(tBuffer, sizeof(tBuffer), "%H:%M:%S", &tTimeInfo);
        tClient.print(F("\r\n  Time: "));
        tClient.print(tBuffer);
        tClient.print(F("\r\n\r\n"));
        return true;
      }
  };

}

#endif