#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <include/utils.h>
#include <include/database.h>
#include <include/instructions32.h>
#include <include/instructions64.h>

/************** disassemble **************/

uint64_t addr = 0;
unsigned int x = 0;

void print_progress(double what) {
    printf("\r[");

    for (int i = 0; i != 50; i++) {
        printf("%c", (what > 0) ? ((--what > 0) ? '=' : '>') : ' ');
    }

    printf("] loading...");

    fflush(stdout);

    if (what == 50) {
        printf("\n");
        return;
    }
}

int disarm(void) {
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
    }
  
    if (arch_flags & EYDIS_ARM64) {
        find_insn64(image.img, start_point, end_point, print_progress);
    } else {
        find_insn32(image.img, start_point, end_point, print_progress);
    }

    if (x == 0) {
        x = 1;
        disarm();
    }

    return 0;
}
