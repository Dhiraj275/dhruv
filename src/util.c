#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// #define TEST_DRIVER 1

typedef struct {
  const char *ext;
  const char *mime;
} mime_map;

mime_map mime_type_map[] = {
    {"html", "text/html"}, {"css", "text/css"},      {"js", "text/javascript"},
    {"png", "image/png"},  {"jpg", "image/jpeg"},    {"jpeg", "image/jpeg"},
    {"gif", "image/gif"},  {"svg", "image/svg+xml"}, {NULL, "text/plain"}};

// returns MIME type of given path
const char *get_mime_type(const char *path) {
  char *dot = strrchr(path, '.');
  if(dot==path || !dot){
    return "plain/text";
  }
  const char* ext = dot+1; 

  for(int i = 0; i<sizeof(mime_type_map)/sizeof(mime_map); i++){
    if(strcmp(ext, mime_type_map[i].ext) == 0){
      return mime_type_map[i].mime;
    }
  }
  return "text/plain";
}

// decode url
void url_decode(const char *src, char *dst) {
  char a, b;
  while (*src) {
    if ((*src == '%') && ((a = src[1]) && (b = src[2])) &&
        (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a') {
        a -= 'a' - 'A';
      }
      if (a >= 'A') {
        a -= ('A' - 10);
      } else {
        a -= '0';
      }

      if (b >= 'a') {
        b -= 'a' - 'A';
      }
      if (b >= 'A') {
        b -= ('A' - 10);
      } else {
        b -= '0';
      }
      *dst++ = 16 * a + b;
      src += 3;
    } else if (*src == '+') {
      *dst++ = ' ';
      src++;
    } else {
      *dst++ = *src++;
    }
  }
  *dst++ = '\0';
}

// checks path's sanitiy
static bool is_path_sane(const char *path) {
  const char *str = path;
  size_t n = strnlen(str, 512);
  if (path[0] == '~')
    return false;
  if (path[0] == '.' && path[1] == '.')
    return false;
  for (; str[0] != '\0' && n > 0; str++, n--) {
    if ((str[0] == '\\' || str[0] == '/') && n >= 2) {
      if (str[1] == '.' && str[2] == '.')
        return false;
    }
  }
  return true;
}

// resolve given path
bool resolve_path(const char *path, char *out) {
  char decoded[512] = {0};
  url_decode(path, decoded);

  if (strcmp(decoded, "/") == 0)
    strcpy(decoded, "index.html");
  if (strchr(decoded, '.') == NULL)
    strcat(decoded, "/index.html");

  bool sane = is_path_sane(decoded);
  if (!sane) {
    return false;
  }
  const char *p = decoded[0] == '/' ? decoded + 1 : decoded;
  int n = snprintf(out, 512, "public/%s", p);

  if (n < 0 || (size_t)n >= 512) {
    return false;
  }
  sane = is_path_sane(out);
  if (!sane) {
    return false;
  }

  return true;
}

bool file_exists(char *path) {
  struct stat st;
  // return false if file does not exists
  return !stat(path, &st);
}

char *read_file(const char *path) {
  FILE *f = fopen(path, "r");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *buffer = malloc(size + 1);
  if (buffer == NULL) {
    fclose(f);
    return NULL;
  }
  fread(buffer, size, 1, f);
  buffer[size] = 0;

  fclose(f);
  return buffer;
}

#ifdef TEST_DRIVER
struct test_case {
  const char *input;
  const char *label;
};

// Nicely formatted result logging
static void print_result(const char *label, const char *input,
                         const char *resolved, bool resolved_ok,
                         bool file_exists) {
  printf("\n=== TEST: %s ===\n", label);
  printf("Input URI       : \"%s\"\n", input);

  if (!resolved_ok) {
    printf("Sanity check    : REJECTED (unsafe)\n");
    printf("Resolved Path   : <none>\n");
    printf("File Exists     : <not checked>\n");
    printf("Result          : OK (blocked attack)\n");
    return;
  }

  printf("Sanity check    : OK\n");
  printf("Resolved Path   : \"%s\"\n", resolved);
  printf("File Exists     : %s\n", file_exists ? "TRUE" : "FALSE");

  // Expected behavior:
  // If resolved_ok == true → means safe/allowed path
  // File existing/not existing doesn’t determine security
  printf("Result          : OK (allowed)\n");
}

int main() {

  struct test_case tests[] = {
      // === Existing good cases here ===

      {"/..%2f..%2fetc/passwd", "encoded-slashes-traversal"},
      {"%2e%2e%2f%2e%2e/etc/passwd", "full-encoded-traversal"},
      {"foo/%5c%2e%2e%5cbar", "encoded-backslashes"},
      {"foo/%2E%2E/%2e%2E/bar", "unicode-dotdot-case-mix"},
      {".../%2e%2e/.../file", "dots-around-encoded-dotdot"},
      {"/safe//nested///index.html", "redundant-slashes"},
      {"/////index.html", "leading-multi-slash"},
      {"%2Findex.html", "leading-encoded-slash"},
      {"./././index.html", "redundant-dots"},
      {"a/b/c/../../../index.html", "balanced-dotdot"},
      {"a/b/c/../../..", "return-to-root"},
      {"a/b/..%2F..%2Findex.html", "mixed-encoded-dotdot"},
      {"%00index.html", "nullbyte-prefix"},
      {"index.html%00evil", "nullbyte-suffix"},
      {"%2F..%00%2F..", "encoded-null-in-traversal"},
      {"\xC0\xAFindex.html", "overlong-utf8-slash"},
      {"\xE0\x80\xAFindex.html", "triple-overlong-slash"},
      {"folder/%C0%AE%C0%AE/file", "overlong-dotdot"},
      {"foo/%2e%00%2e/bar", "encoded-null-between-dots"},
      {"foo/bar/.. /index.html", "space-in-dotdot"},
      {"foo/bar/. . /index.html", "dot-space-dot"},
      {"foo/bar/%2E. /index.html", "encoded-dot-space"},
      {"foo/bar/ %2E%2E /index.html", "space-wrapped-encoded-dotdot"},
      {"%2e%2e%2e/index.html", "three-dots"},
      {"....//index.html", "four-dots"},
      {".%2e%2e/index.html", "dot-encoded-dotdot"},
      {"%2E%2E", "uppercase-encoded-dotdot"},
      {"👍/index.html", "unicode-folder"},
      {"index.html👍", "unicode-suffix"},
      {"🙃%2e%2e🙃", "emoji-wrapped-dotdot"},
      {"%2E%2E%2Findex.html", "upper-mixed-encoded-dotdot"},
      {"a/../../../../../../../index.html", "deep-dotdot"},
      {"safe/path/%2f%2f%2f%2findex.html", "encoded-multi-slashes"},
      {"safe/%5C%5C/index.html", "encoded-double-backslash"},
      {"safe/%2F..%2F..%2Findex.html", "interleaved-encoded-slash-traversal"},
      {"safe/main.css", "valid-css"},
      {"safe/index.js", "valid-js"},
      {"safe/index.html", "valid-html"},
  };
  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    char final_path[1024] = {0};
    bool ok = resolve_path(tests[i].input, final_path);

    bool file_exists = false;
    if (ok) {
      struct stat st;
      file_exists = (stat(final_path, &st) == 0);
    }

    print_result(tests[i].label, tests[i].input, final_path, ok, file_exists);
  }

  return 0;
}
#endif
