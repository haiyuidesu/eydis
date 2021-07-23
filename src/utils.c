#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <include/shell.h>
#include <include/utils.h>
#include <include/database.h>
#include <include/definitions.h>

/************** utils **************/

void xprintf(const char *what, ...) {
  if (x == 1) {
    va_list va;

    va_start(va, what);

    vfprintf(stdout, what, va);

    va_end(va);
  }
}

int save_file(void) {
  FILE *fd = NULL;
  char *file = NULL;

  file = strcat(image.filename, ".patched");

  fd = fopen(file, "wb+");

  if (!fd) { 
    printf("\n[%s]: unable to open %s!\n", __func__, file);
    return -1;
  }

  printf("\n[%s]: writing %s...\n", __func__, file);

  fwrite(image.img, image.length, 1, fd);

  printf("[%s]: done !\n", __func__);

  fclose(fd);

  return 0;
}

int patch_image(uint64_t address, uint64_t patch) {
  printf("\n[%s]: \033[0;33mbase_addr\033[30;0m = \e[38;5;196m0x%llx\033[30;0m\n", __func__, image.base);

  if (address > (uint64_t)image.length) {
    if (address > (uint64_t)(image.length + image.base)) {
      printf("[%s]: the inputted address is wrong\n", __func__);
      return -1;
    }
  }

  if (address > (uint64_t)image.length) {
    address = address - image.base;
  }

  *(uint32_t *)(image.img + address) = bswap32(patch);

  printf("[%s]: patched = \e[38;5;196m0x%llx\033[30;0m (\033[0;36m0x%llx\033[30;0m)\n", __func__, address + image.base, patch);

  return 0;
}

int exit_eydis(void) {
  char *choose = NULL;

  if (access(eydis_database, F_OK) != -1) {
    while (1) {
      choose = readline("\e[0m\n[eydis]: saving eydis database [y | n] ? > ");

      if (!strcasecmp(choose, "n")) {
        remove(eydis_database);

        printf("\n[%s]: database removed !\n", __func__);

        break;
      } else if (!strcasecmp(choose, "y")) {
        printf("\n[%s]: database saved !\n", __func__);

        break;
      } else {
        printf("\n[%s]: unrecognized choice, the database is saved anyways.\n", __func__);

        break;
      }
    }
  }

  printf("\n[%s]: process is now safely ended !\n", __func__);

  free(choose);
  free(image.img);

  exit(0);
}

int value_is_printable(unsigned char c) {
  if ((c >= 0x20) && (c <= 0x7e || c == 0x09)) {
    return 1;
  }

  return 0;
}

// https://stackoverflow.com/questions/51389969/implementing-my-own-strings-tool-missing-sequences-gnu-strings-finds

int print_file_strings(uint64_t address) {
  unsigned char c;
  FILE *fd = NULL;
  char buffer[300];
  char *p = buffer;

  fd = fopen(image.filename, "rb");

  if (fd <= 0) return -1;

  fseek(fd, address, SEEK_SET);

  while (1 == fread(&c, 0x1, 1, fd)) {
    if ((value_is_printable(c) == 1) && (p - buffer < sizeof(buffer) - 3)) {
      *p++ = c;
    } else {
      if (p - buffer >= 4) {
        *p++ = '\0';

        // these strings should not be presents
        char *str_to_avoid[] = { "160D", "A@qA", "@y `@y", "Qli(", "E@9(", "M@97",
                                 "'A)*)", "'A)*","Q)|@", "R*A@", "Rh&@", "*QK}@", "I58`" };

        for (int i = 0; i != 0xD; i++) {
          if (!strcmp(buffer, str_to_avoid[i])) {
            goto end;
          }
        }

        xprintf("; \033[38;5;242m\"%s\"\033[30;0m", buffer);

        fclose(fd);

        return 0;
      }

      p = buffer;
    }
  }

end:
  fclose(fd);

  return -1;
}
