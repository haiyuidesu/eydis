#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include <readline/readline.h>

#define bswap32(x) __builtin_bswap32(x)

typedef struct {
  void *img;
  int length;
  uint64_t base;
  char *filetype;
  char *filename;
  int start;
  int end;
} config_t;

extern unsigned int x;

extern char *function;

extern config_t image;

int hexdump(void);

int save_file(void);

int exit_eydis(void);

void xprintf(const char *what, ...);

int print_file_strings(uint64_t address);

int patch_image(uint64_t address, uint64_t patch);

#ifdef __cplusplus
}
#endif

#endif