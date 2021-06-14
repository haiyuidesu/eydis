#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include <readline/readline.h>

#define bswap32(x) __builtin_bswap32(x)

typedef struct {
  void *img;       // file data
  int length;      // variable used to precise the file length
  uint64_t base;   // variable used to precise the file base address
  char *filetype;  // variable used to precise to actual type of file
  char *filename;  // variable used to precise to actual name of the file
  
  int analyze;     /*
                    * variable used to precise if eydis should analyze the whole file or not,
                    * in this case, the user will not be able to modify the limits above the currents one
                    */

  int start;       // variable used to precise (if any) the offset where the disassemblage will starts
  int end;         // variable used to precise (if any) the offset where the disassemblage will ends 
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