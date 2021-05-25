#ifndef DISARM_H
#define DISARM_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int disasm;

void subprint(uint64_t where);

int disarm(void);

#ifdef __cplusplus
}
#endif

#endif