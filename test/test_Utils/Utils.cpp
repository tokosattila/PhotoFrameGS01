/**
 * @file test_utils.cpp
 * @brief Unit tests for Utils functions (pure C++ logic, no hardware)
 */

#include <unity.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cmath>

// ============================================================================
// Standalone implementations for testing (extracted from Utils.cpp)
// ============================================================================

bool SecureStrcmp(const char *tA, const char *tB) {
  if (!tA || !tB) return false;
  size_t tLenA = strlen(tA);
  size_t tLenB = strlen(tB);
  volatile uint8_t tDiff = static_cast<uint8_t>(tLenA ^ tLenB);
  size_t tMinLen = (tLenA < tLenB) ? tLenA : tLenB;
  for (size_t i = 0; i < tMinLen; i++) {
    tDiff |= static_cast<uint8_t>(tA[i] ^ tB[i]);
  }
  return tDiff == 0;
}

uint32_t SafeAtoul(const char *tStr, uint32_t tMinVal, uint32_t tMaxVal, uint32_t tDefaultVal) {
  if (!tStr || *tStr == '\0' || *tStr == ' ' || *tStr == '\t') return tDefaultVal;
  char *tEndPtr = nullptr;
  errno = 0;
  unsigned long tVal = strtoul(tStr, &tEndPtr, 10);
  if (errno == ERANGE || tEndPtr == tStr || *tEndPtr != '\0') return tDefaultVal;
  if (tVal < tMinVal || tVal > tMaxVal) return tDefaultVal;
  return static_cast<uint32_t>(tVal);
}

void ByteToReadableSize(uint32_t tBytes, char *tBuffer, size_t tLength) {
  if (tBytes < 1024) snprintf(tBuffer, tLength, "%u B", tBytes);
  else if (tBytes < 1024UL * 1024UL) {
    float tSizeKB = tBytes / 1024.0f;
    if (fabs(tSizeKB - (int)tSizeKB) < 0.01f) snprintf(tBuffer, tLength, "%d KB", (int)tSizeKB);
    else snprintf(tBuffer, tLength, "%.2f KB", tSizeKB);
  } else {
    float tSizeMB = tBytes / (1024.0f * 1024.0f);
    if (fabs(tSizeMB - (int)tSizeMB) < 0.01f) snprintf(tBuffer, tLength, "%d MB", (int)tSizeMB);
    else snprintf(tBuffer, tLength, "%.2f MB", tSizeMB);
  }
}

// ============================================================================
// SecureStrcmp Tests
// ============================================================================

void test_SecureStrcmp_identical_strings() {
  TEST_ASSERT_TRUE(SecureStrcmp("hello", "hello"));
  TEST_ASSERT_TRUE(SecureStrcmp("password123", "password123"));
  TEST_ASSERT_TRUE(SecureStrcmp("", ""));
}

void test_SecureStrcmp_different_strings() {
  TEST_ASSERT_FALSE(SecureStrcmp("hello", "world"));
  TEST_ASSERT_FALSE(SecureStrcmp("password", "password1"));
  TEST_ASSERT_FALSE(SecureStrcmp("abc", "abd"));
}

void test_SecureStrcmp_different_lengths() {
  TEST_ASSERT_FALSE(SecureStrcmp("short", "longer_string"));
  TEST_ASSERT_FALSE(SecureStrcmp("test", "tes"));
  TEST_ASSERT_FALSE(SecureStrcmp("a", "aa"));
}

void test_SecureStrcmp_null_inputs() {
  TEST_ASSERT_FALSE(SecureStrcmp(nullptr, "hello"));
  TEST_ASSERT_FALSE(SecureStrcmp("hello", nullptr));
  TEST_ASSERT_FALSE(SecureStrcmp(nullptr, nullptr));
}

void test_SecureStrcmp_case_sensitive() {
  TEST_ASSERT_FALSE(SecureStrcmp("Hello", "hello"));
  TEST_ASSERT_FALSE(SecureStrcmp("ADMIN", "admin"));
}

// ============================================================================
// SafeAtoul Tests
// ============================================================================

void test_SafeAtoul_valid_numbers() {
  TEST_ASSERT_EQUAL_UINT32(100, SafeAtoul("100", 0, 1000, 0));
  TEST_ASSERT_EQUAL_UINT32(0, SafeAtoul("0", 0, 100, 50));
  TEST_ASSERT_EQUAL_UINT32(999, SafeAtoul("999", 0, 1000, 0));
}

void test_SafeAtoul_boundary_values() {
  TEST_ASSERT_EQUAL_UINT32(10, SafeAtoul("10", 10, 100, 50));  // min value
  TEST_ASSERT_EQUAL_UINT32(100, SafeAtoul("100", 10, 100, 50)); // max value
}

void test_SafeAtoul_out_of_range() {
  TEST_ASSERT_EQUAL_UINT32(50, SafeAtoul("5", 10, 100, 50));   // below min
  TEST_ASSERT_EQUAL_UINT32(50, SafeAtoul("200", 10, 100, 50)); // above max
}

void test_SafeAtoul_invalid_input() {
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul("", 0, 100, 99));       // empty string
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul(nullptr, 0, 100, 99));  // null
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul("abc", 0, 100, 99));    // non-numeric
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul("12abc", 0, 100, 99));  // partial numeric
}

void test_SafeAtoul_whitespace() {
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul(" 50", 0, 100, 99));   // leading space - rejected
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul("50 ", 0, 100, 99));   // trailing space - rejected
  TEST_ASSERT_EQUAL_UINT32(99, SafeAtoul("\t50", 0, 100, 99));  // leading tab - rejected
}

// ============================================================================
// ByteToReadableSize Tests
// ============================================================================

void test_ByteToReadableSize_bytes() {
  char buffer[32];
  
  ByteToReadableSize(0, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("0 B", buffer);
  
  ByteToReadableSize(512, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("512 B", buffer);
  
  ByteToReadableSize(1023, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("1023 B", buffer);
}

void test_ByteToReadableSize_kilobytes() {
  char buffer[32];
  
  ByteToReadableSize(1024, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("1 KB", buffer);
  
  ByteToReadableSize(2048, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("2 KB", buffer);
  
  ByteToReadableSize(1536, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("1.50 KB", buffer);
}

void test_ByteToReadableSize_megabytes() {
  char buffer[32];
  
  ByteToReadableSize(1024 * 1024, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("1 MB", buffer);
  
  ByteToReadableSize(2 * 1024 * 1024, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("2 MB", buffer);
  
  ByteToReadableSize(1536 * 1024, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("1.50 MB", buffer);
}

void test_ByteToReadableSize_large_values() {
  char buffer[32];
  
  ByteToReadableSize(16 * 1024 * 1024, buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING("16 MB", buffer);
}

// ============================================================================
// Test Runner
// ============================================================================

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  
  // SecureStrcmp tests
  RUN_TEST(test_SecureStrcmp_identical_strings);
  RUN_TEST(test_SecureStrcmp_different_strings);
  RUN_TEST(test_SecureStrcmp_different_lengths);
  RUN_TEST(test_SecureStrcmp_null_inputs);
  RUN_TEST(test_SecureStrcmp_case_sensitive);
  
  // SafeAtoul tests
  RUN_TEST(test_SafeAtoul_valid_numbers);
  RUN_TEST(test_SafeAtoul_boundary_values);
  RUN_TEST(test_SafeAtoul_out_of_range);
  RUN_TEST(test_SafeAtoul_invalid_input);
  RUN_TEST(test_SafeAtoul_whitespace);
  
  // ByteToReadableSize tests
  RUN_TEST(test_ByteToReadableSize_bytes);
  RUN_TEST(test_ByteToReadableSize_kilobytes);
  RUN_TEST(test_ByteToReadableSize_megabytes);
  RUN_TEST(test_ByteToReadableSize_large_values);
  
  return UNITY_END();
}
