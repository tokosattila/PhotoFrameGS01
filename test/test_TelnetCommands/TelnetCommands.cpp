#include <unity.h>
#include <cstring>
#include <cctype>
#include <cstdint>

#include "../mocks/MockString.h"
#include "../mocks/MockWiFiClient.h"

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_WHITE "\033[37m"

namespace TelnetCommandTest {

  bool ParseNoArgsCommand(const char *tInput) {
    if (!tInput) return false;
    const char *tPtr = tInput;
    while (*tPtr != '\0' && *tPtr != ' ' && *tPtr != '\t') ++tPtr;
    while (*tPtr == ' ' || *tPtr == '\t') ++tPtr;
    return *tPtr == '\0';
  }

  void EmitClear(MockWiFiClient &tClient) {
    tClient.print(F("\x1B[2J\x1B[H"));
  }

  void EmitListEmpty(MockWiFiClient &tClient, const char *tLabel) {
    tClient.printf(COLOR_GREEN "\r\n  File structure [%s]:\r\n\r\n" COLOR_WHITE, tLabel);
    tClient.print(F(COLOR_YELLOW "  File structure is empty.\r\n\r\n" COLOR_WHITE));
  }

  void EmitLogsMissing(MockWiFiClient &tClient) {
    tClient.print(F(COLOR_YELLOW "  No logs directory found in LittleFS.\r\n\r\n" COLOR_WHITE));
  }

  void EmitLogsHeader(MockWiFiClient &tClient) {
    tClient.printf(COLOR_GREEN "\r\n  File structure [logs/ directory]:\r\n\r\n" COLOR_WHITE);
  }

  void EmitFormatPrompt(MockWiFiClient &tClient, const char *tStorageName) {
    char tPrompt[64] = "";
    snprintf(tPrompt, sizeof(tPrompt), "Format %s? (y/n): ", tStorageName);
    tClient.print(tPrompt);
  }

  void EmitFormatSuccess(MockWiFiClient &tClient) {
    tClient.print(F(COLOR_GREEN "\r\n  Format complete\r\n\r\n" COLOR_WHITE));
  }

  void EmitFormatFailure(MockWiFiClient &tClient) {
    tClient.print(F(COLOR_RED "\r\n  Error: format failed\r\n\r\n" COLOR_WHITE));
  }

  bool ParseRenameArgs(const char *tInput, char *tFrom, size_t tFromSize, char *tTo, size_t tToSize) {
    if (!tInput || !tFrom || !tTo) return false;
    const char *tP = tInput;
    while (*tP != '\0' && *tP != ' ' && *tP != '\t') ++tP;
    while (*tP == ' ' || *tP == '\t') ++tP;
    const char *tS = tP;
    while (*tP != '\0' && *tP != ' ' && *tP != '\t') ++tP;
    if (tP == tS) return false;
    size_t tLen = (size_t)(tP - tS);
    if (tLen >= tFromSize) return false;
    strncpy(tFrom, tS, tLen);
    tFrom[tLen] = '\0';
    while (*tP == ' ' || *tP == '\t') ++tP;
    tS = tP;
    while (*tP != '\0' && *tP != ' ' && *tP != '\t') ++tP;
    if (tP == tS) return false;
    tLen = (size_t)(tP - tS);
    if (tLen >= tToSize) return false;
    strncpy(tTo, tS, tLen);
    tTo[tLen] = '\0';
    while (*tP == ' ' || *tP == '\t') ++tP;
    return *tP == '\0';
  }

  void EmitRenameUsage(MockWiFiClient &tClient) {
    tClient.print(F(COLOR_YELLOW "\r\n  Usage: rename <source> <destination>\r\n"));
    tClient.print(F("  Source/Destination: path within LittleFS\r\n\r\n" COLOR_WHITE));
  }

  void EmitRenameSuccess(MockWiFiClient &tClient, const char *tFrom, const char *tTo) {
    tClient.printf(COLOR_GREEN "\r\n  Renamed: %s -> %s\r\n\r\n" COLOR_WHITE, tFrom, tTo);
  }

  void EmitRenameFailure(MockWiFiClient &tClient) {
    tClient.print(F(COLOR_RED "\r\n  Error: rename failed\r\n\r\n" COLOR_WHITE));
  }

}

void test_ParseNoArgsCommand_accepts_format() {
  TEST_ASSERT_TRUE(TelnetCommandTest::ParseNoArgsCommand("format"));
  TEST_ASSERT_TRUE(TelnetCommandTest::ParseNoArgsCommand("format   "));
  TEST_ASSERT_TRUE(TelnetCommandTest::ParseNoArgsCommand("format\t"));
}

void test_ParseNoArgsCommand_rejects_extra_args() {
  TEST_ASSERT_FALSE(TelnetCommandTest::ParseNoArgsCommand("format extra"));
  TEST_ASSERT_FALSE(TelnetCommandTest::ParseNoArgsCommand("format lfs"));
}

void test_ClearCommand_emits_full_clear_sequence() {
  MockWiFiClient client;
  TelnetCommandTest::EmitClear(client);
  TEST_ASSERT_TRUE(client.contains("\x1B[2J"));
  TEST_ASSERT_TRUE(client.contains("\x1B[H"));
  TEST_ASSERT_FALSE(client.contains("\x1B[3J"));
}

void test_ClearCommand_starts_with_clear_sequence() {
  MockWiFiClient client;
  TelnetCommandTest::EmitClear(client);
  TEST_ASSERT_EQUAL_STRING("\x1B[2J\x1B[H", client.getOutput());
}

void test_ListEmpty_emits_green_header_and_empty_message() {
  MockWiFiClient client;
  TelnetCommandTest::EmitListEmpty(client, "LittleFS");
  TEST_ASSERT_TRUE(client.contains(COLOR_GREEN));
  TEST_ASSERT_TRUE(client.contains("File structure [LittleFS]:"));
  TEST_ASSERT_TRUE(client.contains(COLOR_YELLOW));
  TEST_ASSERT_TRUE(client.contains("File structure is empty."));
}

void test_ListEmpty_output_has_no_extra_blank_line() {
  MockWiFiClient client;
  TelnetCommandTest::EmitListEmpty(client, "LittleFS");
  TEST_ASSERT_FALSE(strstr(client.getOutput(), "\r\n\r\n\r\n"));
}

void test_LogsMissing_is_yellow_and_has_no_header() {
  MockWiFiClient client;
  TelnetCommandTest::EmitLogsMissing(client);
  TEST_ASSERT_TRUE(client.contains(COLOR_YELLOW));
  TEST_ASSERT_TRUE(client.contains("No logs directory found in LittleFS."));
  TEST_ASSERT_FALSE(client.contains("File structure [logs/ directory]:"));
}

void test_LogsHeader_present_when_logs_exist() {
  MockWiFiClient client;
  TelnetCommandTest::EmitLogsHeader(client);
  TEST_ASSERT_TRUE(client.contains(COLOR_GREEN));
  TEST_ASSERT_TRUE(client.contains("File structure [logs/ directory]:"));
}

void test_FormatPrompt_contains_storage_name() {
  MockWiFiClient client;
  TelnetCommandTest::EmitFormatPrompt(client, "LittleFS");
  TEST_ASSERT_TRUE(client.contains("Format LittleFS? (y/n): "));
}

void test_FormatSuccess_message_is_green() {
  MockWiFiClient client;
  TelnetCommandTest::EmitFormatSuccess(client);
  TEST_ASSERT_TRUE(client.contains(COLOR_GREEN));
  TEST_ASSERT_TRUE(client.contains("Format complete"));
}

void test_FormatFailure_message_is_red() {
  MockWiFiClient client;
  TelnetCommandTest::EmitFormatFailure(client);
  TEST_ASSERT_TRUE(client.contains(COLOR_RED));
  TEST_ASSERT_TRUE(client.contains("Error: format failed"));
}

void test_ParseRenameArgs_simple_paths() {
  char from[256] = "";
  char to[256] = "";
  TEST_ASSERT_TRUE(TelnetCommandTest::ParseRenameArgs("rename /images/a.jpg /images/b.jpg", from, sizeof(from), to, sizeof(to)));
  TEST_ASSERT_EQUAL_STRING("/images/a.jpg", from);
  TEST_ASSERT_EQUAL_STRING("/images/b.jpg", to);
}

void test_ParseRenameArgs_rejects_too_many_args() {
  char from[256] = "";
  char to[256] = "";
  TEST_ASSERT_FALSE(TelnetCommandTest::ParseRenameArgs("rename a.txt b.txt extra", from, sizeof(from), to, sizeof(to)));
}

void test_RenameUsage_mentions_source_and_destination() {
  MockWiFiClient client;
  TelnetCommandTest::EmitRenameUsage(client);
  TEST_ASSERT_TRUE(client.contains("Usage: rename <source> <destination>"));
  TEST_ASSERT_TRUE(client.contains("Source/Destination: path within LittleFS"));
}

void test_RenameSuccess_message_is_green() {
  MockWiFiClient client;
  TelnetCommandTest::EmitRenameSuccess(client, "/images/a.jpg", "/images/b.jpg");
  TEST_ASSERT_TRUE(client.contains(COLOR_GREEN));
  TEST_ASSERT_TRUE(client.contains("Renamed: /images/a.jpg -> /images/b.jpg"));
}

void test_RenameFailure_message_is_red() {
  MockWiFiClient client;
  TelnetCommandTest::EmitRenameFailure(client);
  TEST_ASSERT_TRUE(client.contains(COLOR_RED));
  TEST_ASSERT_TRUE(client.contains("Error: rename failed"));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_ParseNoArgsCommand_accepts_format);
  RUN_TEST(test_ParseNoArgsCommand_rejects_extra_args);
  RUN_TEST(test_ClearCommand_emits_full_clear_sequence);
  RUN_TEST(test_ClearCommand_starts_with_clear_sequence);
  RUN_TEST(test_ListEmpty_emits_green_header_and_empty_message);
  RUN_TEST(test_ListEmpty_output_has_no_extra_blank_line);
  RUN_TEST(test_LogsMissing_is_yellow_and_has_no_header);
  RUN_TEST(test_LogsHeader_present_when_logs_exist);
  RUN_TEST(test_FormatPrompt_contains_storage_name);
  RUN_TEST(test_FormatSuccess_message_is_green);
  RUN_TEST(test_FormatFailure_message_is_red);
  RUN_TEST(test_ParseRenameArgs_simple_paths);
  RUN_TEST(test_ParseRenameArgs_rejects_too_many_args);
  RUN_TEST(test_RenameUsage_mentions_source_and_destination);
  RUN_TEST(test_RenameSuccess_message_is_green);
  RUN_TEST(test_RenameFailure_message_is_red);
  return UNITY_END();
}
