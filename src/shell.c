#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <include/shell.h>
#include <include/utils.h>
#include <include/disarm.h>

/************** shell **************/

int patch(char *current_addr, char *patch_insn)
{
    uint64_t addr = strtol(current_addr, NULL, 0x10);
    uint64_t patch = strtol(patch_insn, NULL, 0x10);

    if (addr == 0 || patch == 0) {
        printf("\n[%s]: missing or wrong addresses.\n", __func__);
        shell_help();
        return -1;
    }

    patch_image(addr, patch);

    return 0;
}

int set_lim(char *begin_addr, char *end_addr)
{
    image.end = strtol(end_addr, NULL, 0x10);
    image.start = strtol(begin_addr, NULL, 0x10);

    if (image.start < 0 || image.end <= 0) {
        printf("\n[%s]: missing or wrong addresses.\n", __func__);
        shell_help();
        return -1;
    }

    if (image.analyze == 1 && (image.end > image.length)) {
        printf("\n[%s]: refusing to set the limits above than the former ones since the aera is not fully analyzed.\n", __func__);
        return -1;
    }

    printf("\n[%s]: new limits set : start = 0x%lx, end = 0x%lx\n", __func__, image.start, image.end);

    return 0;
}

int shell_help(void)
{
    printf("\n[%s]: exit\t\t\t\texit safely eydis,\n", __func__);
    printf("[%s]: save\t\t\t\tsave a patched file,\n", __func__);
    printf("[%s]: reload \t\t\t\treload the actual disassemblage,\n", __func__);

    printf("\n[%s]: patch [addr] [hex] \t\tpatch a certain instruction of the image,\n", __func__);
    printf("[%s]: name [old_name] [new_name]\trename an existing subroutine,\n", __func__);
    printf("[%s]: set [start_addr] [end_addr]\tset new disassembling limits.\n", __func__);

    return 0;
}

int eydish(void)
{
    char *new = NULL;
    char *current = NULL;
    char *command = NULL;

    for (int o = 0; ;) {
        command = readline("\n\033[30;0meydis > ");

        strtok(command, " ");

        for (o = 0; eydis_cmds[o].cmd != NULL; o++) {
            if (!strcasecmp(eydis_cmds[o].cmd, command)) {
                if (eydis_cmds[o].first != NULL) {
                    eydis_cmds[o].first();
                    break;
                }

                current = strtok(NULL, " ");

                new = strtok(NULL, "");

                if (current == NULL || new == NULL) {
                    printf("\n[%s]: an argument is missing or wrong for the requested command.\n", __func__);
                    break;
                }

                eydis_cmds[o].second(current, new);

                break;
            }
        }

        if (eydis_cmds[o].cmd == NULL) {
            printf("\n[%s]: unrecognized command, enter 'help' to see the available ones.\n", __func__);
        }

        free(command);

        continue;
    }

    return 0;
}
