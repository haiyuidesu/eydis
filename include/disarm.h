#ifndef DISARM_H
#define DISARM_H

#ifdef __cplusplus
extern "C"
{
#endif
    #define which_eydis "2.0"

    #define EYDIS_ARM    (1 << 0)
    #define EYDIS_ARM64  (1 << 1)

    extern long arch_flags;
    extern unsigned int disasm;
    typedef void (*eydis_callback_t)(double progress);

    void print_progress(double what);

    void subprint(uint64_t where);

    int disarm(void);
#ifdef __cplusplus
}
#endif
#endif
