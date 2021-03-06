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

void subprint(uint64_t where)
{
    xprintf("\033[38;5;242m%s:%llx  ; =================== S U B R O U T I N E =======================\n"
            "\033[38;5;242m%s:%llx\n",
            image.filetype, where, image.filetype, where);
}

void current_insn_hex(unsigned int where)
{
    printf("\t\t");

    for (unsigned int i = where; i <= where + 0x3; i++) {
        printf("\033[30;0m%02x ", ((unsigned char *)image.img)[i]);
    }
}

void xprintf(const char *what, ...)
{
    if (x == 1) {
        va_list va;
        va_start(va, what);
        vfprintf(stdout, what, va);
        va_end(va);
    }
}

int save_file(void)
{
    FILE *fd = NULL;
    char *file = strcat(image.filename, ".patched");

    if (!(fd = fopen(file, "wb+"))) {
        printf("\n[%s]: unable to open %s!\n", __func__, file);
        return -1;
    }

    printf("\n[%s]: writing %s...\n", __func__, file);

    fwrite(image.img, image.length, 1, fd);

    printf("[%s]: done !\n", __func__);

    fclose(fd);

    return 0;
}

int patch_image(uint64_t address, uint64_t patch)
{
    printf("\n[%s]: \033[0;33mbase_addr\033[30;0m = \033[38;5;196m0x%llx\033[30;0m\n", __func__, image.base);

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

    printf("[%s]: patched = \033[38;5;196m0x%llx\033[30;0m (\033[0;36m0x%llx\033[30;0m)\n", __func__, address + image.base, patch);

    return 0;
}

int exit_eydis(void)
{
    char *choose = NULL;

    if (access(eydis_database, F_OK) != -1) {
        while (1) {
            choose = readline("\n\033[0m\n[eydis]: saving eydis database [y | n] ? > ");

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

int value_is_printable(unsigned char c)
{
    return ((c >= 0x20) && (c <= 0x7e || c == 0x09) ? 1 : 0);
}

// https://stackoverflow.com/questions/51389969/implementing-my-own-strings-tool-missing-sequences-gnu-strings-finds

int print_file_strings(uint64_t address)
{
    unsigned char c;
    FILE *fd = NULL;
    char buffer[300];
    char *ptr = buffer;

    if (!(fd = fopen(image.filename, "rb"))) {
        return -1;
    }

    fseek(fd, address, SEEK_SET);

    while (fread(&c, 1, 1, fd) == 1) {
        if ((value_is_printable(c) == 1) && ((ptr - buffer) < sizeof(buffer) - 3)) {
            *ptr++ = c;
        } else {
            if ((ptr - buffer) >= 4) {
                *ptr++ = '\0';
                xprintf("; \033[38;5;242m\"%s\"\033[30;0m", buffer);
                fclose(fd);
                return 0;
            }
            ptr = buffer;
        }
    }

    fclose(fd);

    return -1;
}
