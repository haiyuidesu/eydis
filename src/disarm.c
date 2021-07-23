#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <include/utils.h>
#include <include/database.h>
#include <include/instructions64.h>

/************** disassemble **************/

uint64_t addr = 0;

unsigned int x = 0;

void subprint(uint64_t where) {
  xprintf("\033[38;5;242m%s:%llx  ; =================== S U B R O U T I N E =======================\n"
          "\033[38;5;242m%s:%llx\n",
          image.filetype, where, image.filetype, where);
}

void current_insn_hex(unsigned int where) {
  printf("\t\t");

  for (unsigned int i = where; i <= where + 0x3; i++) {
    printf("\033[30;0m%02x ", ((unsigned char *)image.img)[i]);
  }
}

int disarm(void) {
  uint32_t mem = 0;

  int start_point = 0;
  int end_point = 0;

  xprintf("\033c\n");

  if (image.start == 0) {
    subprint(image.base);

    xprintf("\033[38;5;242m%s:%llx   ", image.filetype, image.base);

    print_register_sub("_start");

    xprintf("\n\033[38;5;242m%s:%llx\n", image.filetype, image.base);
  }

   // it will performs a full analysis in case of the user set the new limits above than the former ones

  start_point = image.start;
  end_point = image.end;

  if (x == 0) {
    start_point = 0x0;
    end_point = image.length;

    char *prologues[] = { "\xf6\x57\xbd\xa9", "\xfd\x7b\xbf\xa9", "\xfc\x6f\xba\xa9", "\x7f\x23\x03\xd5" }; // 64bit prologues

    for (int i = 0; i != 0x4; i++) {
      for (int j = 0; j <= image.length; j += 0x4) {
        if (memcmp(&image.img[j], prologues[i], 0x4) == 0) {
          check_data_existence(1, image.base + j);
        }
      }
    }
  }
  
  while (start_point <= end_point) {
    unsigned int j = 0;

    addr = image.base + start_point;
    mem = *(uint32_t *)(image.img + start_point);

    if (x == 1) print_subroutines(); // this function is guilty of the slowness of eydis...
    // if you comment out this line, it will become a way more faster but it would not make any sense without it...

    xprintf("\033[38;5;242m%s:%llx", image.filetype, addr);

    if (x == 1) current_insn_hex(start_point);

    while (j != sizeof(available_insn64)) {
      if (available_insn64[j].insn(mem) == 1) {
        break;
      }

      j++;
    }

    start_point += 0x4;
  }

  if (x == 0) {
    x = 1;

    disarm();
  }

  return 0;
}
