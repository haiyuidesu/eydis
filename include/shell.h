#ifndef SHELL_H
#define SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <include/utils.h>
#include <include/disarm.h>
#include <include/database.h>

typedef struct {
  char *cmd;
  int (*first)(void);
  int (*second)(char *, char *);
} commands_t;

int eydish(void);

int exit_eydis(void);
int shell_help(void);

int set_lim(char *begin_addr, char *end_addr);
int patch(char *current_insn, char *patch_insn);

static const commands_t eydis_cmds[] = {
  { "exit",      exit_eydis,   NULL         },
  { "save",      save_file,    NULL         },
  { "reload",    disarm,       NULL         },
  { "help",      shell_help,   NULL         },
  { "patch",     NULL,         patch        },
  { "name",      NULL,         update_names },
  { "set",       NULL,         set_lim      },
  { NULL,        NULL,         NULL         }
};

#ifdef __cplusplus
}
#endif

#endif