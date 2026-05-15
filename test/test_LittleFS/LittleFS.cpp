#include <unity.h>
#include <cstring>
#include <vector>

static const char *BaseName(const char *tPath) {
  if (!tPath) return "";
  const char *tSlash = strrchr(tPath, '/');
  return tSlash ? (tSlash + 1) : tPath;
}

static size_t ResolveNextIndex(const std::vector<const char*> &tFiles, const char *tCurrentFilename) {
  if (tFiles.empty()) return 0;
  if (!tCurrentFilename || tCurrentFilename[0] == '\0') return 0;
  const char *tSearch = BaseName(tCurrentFilename);
  for (size_t i = 0; i < tFiles.size(); ++i) {
    const char *tEntryName = BaseName(tFiles[i]);
    if (strcmp(tEntryName, tSearch) == 0) return (i + 1U) % tFiles.size();
  }
  return 0;
}

static const char *ResolveNextFile(const std::vector<const char*> &tFiles, const char *tCurrentFilename) {
  if (tFiles.empty()) return nullptr;
  return tFiles[ResolveNextIndex(tFiles, tCurrentFilename)];
}

void test_ResolveNextFile_empty_returns_null() {
  std::vector<const char*> tFiles;
  TEST_ASSERT_NULL(ResolveNextFile(tFiles, "any.jpg"));
}

void test_ResolveNextFile_no_current_returns_first() {
  std::vector<const char*> tFiles = {"/images/001.jpg", "/images/002.jpg"};
  TEST_ASSERT_EQUAL_STRING("/images/001.jpg", ResolveNextFile(tFiles, ""));
  TEST_ASSERT_EQUAL_STRING("/images/001.jpg", ResolveNextFile(tFiles, nullptr));
}

void test_ResolveNextFile_moves_to_next_when_found() {
  std::vector<const char*> tFiles = {"/images/001.jpg", "/images/002.jpg", "/images/003.jpg"};
  TEST_ASSERT_EQUAL_STRING("/images/003.jpg", ResolveNextFile(tFiles, "002.jpg"));
}

void test_ResolveNextFile_wraps_from_last_to_first() {
  std::vector<const char*> tFiles = {"/images/001.jpg", "/images/002.jpg", "/images/003.jpg"};
  TEST_ASSERT_EQUAL_STRING("/images/001.jpg", ResolveNextFile(tFiles, "003.jpg"));
}

void test_ResolveNextFile_compares_by_basename() {
  std::vector<const char*> tFiles = {"/images/a.jpg", "/images/b.jpg"};
  TEST_ASSERT_EQUAL_STRING("/images/b.jpg", ResolveNextFile(tFiles, "/images/a.jpg"));
}

void test_ResolveNextFile_not_found_returns_first() {
  std::vector<const char*> tFiles = {"/images/001.jpg", "/images/002.jpg"};
  TEST_ASSERT_EQUAL_STRING("/images/001.jpg", ResolveNextFile(tFiles, "missing.jpg"));
}

void test_ResolveNextFile_single_file_returns_same() {
  std::vector<const char*> tFiles = {"/images/only.jpg"};
  TEST_ASSERT_EQUAL_STRING("/images/only.jpg", ResolveNextFile(tFiles, "only.jpg"));
  TEST_ASSERT_EQUAL_STRING("/images/only.jpg", ResolveNextFile(tFiles, "/images/only.jpg"));
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_ResolveNextFile_empty_returns_null);
  RUN_TEST(test_ResolveNextFile_no_current_returns_first);
  RUN_TEST(test_ResolveNextFile_moves_to_next_when_found);
  RUN_TEST(test_ResolveNextFile_wraps_from_last_to_first);
  RUN_TEST(test_ResolveNextFile_compares_by_basename);
  RUN_TEST(test_ResolveNextFile_not_found_returns_first);
  RUN_TEST(test_ResolveNextFile_single_file_returns_same);
  return UNITY_END();
}
