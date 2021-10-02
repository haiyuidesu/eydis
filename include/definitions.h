#ifndef INSN64_H
#define INSN64_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <include/utils.h>

extern char *def;
extern char *name;

extern uint64_t addr;

typedef struct {
  unsigned int (*insn)(uint32_t);
  unsigned int (*insn16)(uint16_t);
} current_insn_t;

/***************** arm64 mnemonics *****************/

static char register_prefix[5] = { 'b', 'd', 'q', 's', 'h' };

static char *shift_names[4] = { "lsl", "lsr", "asl", "ror" };

static char *mov_mnemonics[4] = { "movn", NULL, "movz", "movk" };

static char *calc_mnemonics[4] = { "add", "adds", "sub", "subs" };

static char *condition_select_mnemonics[4] = { "csel", "csinc", "csinv", "csneg" };

static char *barrier_mnemonics[8] = { NULL, NULL, "clrex", NULL, "dsb", "dmb", "isb", NULL };

static char *unconditional_mnemonics[8] = { "br", "blr", "ret", NULL, "eret", "drps", NULL, NULL };

static char *data_process_mnemonics[8] = { "rbit", "rev16", "rev32", "rev", "clz", "cls", NULL, NULL };

static char *bitwise_mnemonics_register_32[16] = {
  "and", "eor", "lsl", "lsr", "asr", "adc", "sbc", "ror", "tst", "rsb", "cmp", "cmn", "orr", "mul", "bic", "mvn"
};

static char *data_load_mnemonics_32[8] = { "ldrb", "ldrh", "ldr", NULL, "ldrsb", "ldrsh" };

static char *bitwise_mnemonics_logical_32[16] = {
  "and", "bic", "orr", "orn", "eor", NULL, "pkh", NULL, "add", NULL, "adc", "sbc", NULL, "sub", "rsb", NULL
};

static char *extract_bit_names_32[16] = {
  "addw", NULL, "movw", NULL, NULL, "subw", "movt", NULL,
  "ssat", "ssat16", "sbfx", "bfi", "usat" , "usat16", "ubfx", NULL
};

static char *long_multiple_device_mnemonics_32[8] = {
  "smull", "sdiv", "umull", "udiv", "smlal", "smlsld", "umlal", NULL
};

static char *long_multiple_device_mnemonics_smlal_32[4] = {
  "smlalbb", "smlalbt", "smlaltb", "smlaltt"
};

static char *long_multiple_device_mnemonics_smlald_32[2] = {
  "smlald", "smlaldx"
};

static char *long_multiple_device_mnemonics_smlsld_32[2] = {
  "smlsld", "smlsldx"
};

static char *data_store_single_mnemonics_32[4] = { "strb", "strh", "str", NULL };

static char *loadstore_register_immediate_32[6] = { "str", "ldr", "strb",  "ldrb", "strh", "ldrh" };

static char *loadstore_unsigned_mnemonics_32[8] = { "str", "strh", "strb", "ldrsb", "ldr", "ldrh", "ldrb", "ldrsh" };

static char *extend_bit_names[8] = { "uxtb", "uxth", "uxtw", "uxtx", "sxtb", "sxth", "sxtw", "sxtx" };

static char *extend_bit_names_32[8] = { "sxth", "sxb", "uxth", "uxtb", "rev", "rev16", "revsh" };

static char *bitwise_mnemonics[8] = { "and", "bic", "orr", "orn", "eor", "eon", "ands", "bics" };

static char *extract_bit_names[3] = { "sbfx", "bfxil", "ubfx" };

static char *insert_bit_names[3] = { "sbfiz", "bfi", "ubfiz" };

static char *special_registers32[3] = { "sp", "lr", "pc" };

static char *bitmove_names[3] = { "sbfm", "bfm", "ubfm" };

static char *extend_bit_names2[3][3] = {
  { "sxtb", "sxth", "sxtw" },
  { NULL, NULL, NULL},
  { "uxtb", "uxth", "uxtw" }
};

static char *shift_mnemonics[16] = {
  NULL, NULL, "udiv", "sdiv", NULL, NULL, NULL, NULL,
  "lsl", "lsr", "asr", "ror", NULL, "pacga", NULL, NULL
};

static char *barrier_options[16] = {
  NULL, "oshld", "oshst", "osh", NULL, "nshld", "nshst", "nsh",
  NULL, "ishld", "ishst", "ish", NULL, "ld", "st", "sy"
};

static char *b_condition[16] = {
  "eq", "ne", "hs", "cc", "mi", "pl", "vs", "vc",
  "hi", "cs", "ge", "lt", "gt", "le", "al", "nv"
};

static char *float_point_data_processing_mnemonics[16] = {
  "fmov", "fabs", "fneg", "fsqrt", "fcvt", "fcvt", NULL, "fcvt",
  "frintn", "frintp", "frintm", "frintz", "frinta", NULL, "frintx", "frinti"
};

static char *unmodified_immediate_mnemonics[16] = {
  "addw", NULL, "movw", NULL, NULL, "subw", "movt", NULL, "ssat",
  "ssat16", "sbfx", "bfi", "usat" , "usat16", "ubfx", NULL
};

static char *mem_mnemonics[16] = {
  "madd", "msub", "smaddl", "smsubl", "smulh", NULL, NULL, NULL,
  NULL, NULL, "umaddl", "umsubl", "umulh", NULL, NULL, NULL
};

static char *pseudo_mem_mnemonics[16] = {
  "mul", "mneg", "smull", "smnegl", "smulh", NULL, NULL, NULL,
  NULL, NULL, "umull", "umnegl", "umulh", NULL, NULL, NULL
};
    
static char *pac_auth_mnemonics[18] = {
  "pacia", "pacib", "pacda", "pacdb", "autia", "autib", "autda", "autdb",
  "paciza", "pacizb", "pacdza", "pacdzb", "autiza", "autizb", "autdza", "autdzb",
  "xpaci", "xpacd"
};

static char *auth_unconditional_mnemonics[20] = {
  "braaz", "brabz", "blraaz", "blrabz", "retaa", "retab", NULL, NULL,
  "eretaa", "eretab", NULL, NULL, NULL, NULL, NULL, NULL,
  "braa", "brab", "blraa", "blrab"
};

static char *un_signed_mnemonics[32] = {
  "strb", "ldrb", "ldrsb", "ldrsb", "str", "ldr", "str", "ldr",
  "strh", "ldrh", "ldrsh", "ldrsh", "str", "ldr", NULL, NULL,
  "str", "ldr", "ldrsw", NULL, "str", "ldr", NULL, NULL,
  "str", "ldr", NULL, NULL, "str", "ldr", NULL, NULL
};

static char *unprivileged_mnemonics[32] = {
  "sttrb", "ldtrb", "ldtrsb", "ldtrsb", NULL, NULL, NULL, NULL,
  "sttrh", "ldtrh", "ldtrsh", "ldtrsh", NULL, NULL, NULL, NULL,
  "sttr", "ldtr", "ldtrsw", NULL, NULL, NULL, NULL, NULL,
  "sttr", "ldtr", NULL, NULL, NULL, NULL, NULL, NULL
};

static char *float_point_int_conversion_mnemonics[32] = {
  "fcvtns", "fcvtnu", "scvtf", "ucvtf", "fcvtas", "fcvtau", "fmov", "fmov",
  "fcvtps", "fcvtpu", NULL, NULL, NULL, NULL, "fmov", "fmov",
  "fcvtms", "fcvtmu", NULL, NULL, NULL, NULL, NULL, NULL,
  "fcvtzs", "fcvtzu", NULL, NULL, NULL, NULL, NULL, NULL
};

static char *unscaled_mnemonics[32] = {
  "sturb", "ldurb", "ldursb", "ldursb", "stur", "ldur", "stur", "ldur",
  "sturh", "ldurh", "ldursh", "ldursh", "stur", "ldur", NULL, NULL,
  "stur", "ldur", "ldursw", NULL, "stur", "ldur", NULL, NULL,
  "stur", "ldur", "prfum", NULL, "stur", "ldur", NULL, NULL
};

static char *hints_mnemonics[32] = {
  "nop", "yield", "wfe", "wfi", "sev", "sevl", NULL, "xpaclri",
  "pacia1716", 0, "pacib1716", NULL, "autia1716", NULL, "autib1716",
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  "paciaz", "paciasp", "pacibz", "pacibsp", "autiaz", "autiasp", "autibz", "autibsp"
};

static char *load_atomics_mnemonics[64] = {
  "ldaddb", "ldaddlb", "ldaddab", "ldaddalb",
  "ldaddh", "ldaddlh", "ldaddah", "ldaddalh",
  "ldadd", "ldaddl", "ldadda", "ldaddal",
  "ldadd", "ldaddl", "ldadda", "ldaddal",

  "ldclrb", "ldclrlb", "ldclrab", "ldclralb",
  "ldclrh", "ldclrlh", "ldclrah", "ldclralh",
  "ldclr", "ldclrl", "ldclra", "ldclral",
  "ldclr", "ldclrl", "ldclra", "ldclral",

  "ldeorb", "ldeorlb", "ldeorab", "ldeoralb",
  "ldeorh", "ldeorlh", "ldeorah", "ldeoralh",
  "ldeor", "ldeorl", "ldeora", "ldeoral",
  "ldeor", "ldeorl", "ldeora", "ldeoral",

  "ldsetb", "ldsetlb", "ldsetab", "ldsetalb",
  "ldseth", "ldsetlh", "ldsetah", "ldsetalh",
  "ldset", "ldsetl", "ldseta", "ldsetal",
  "ldset", "ldsetl", "ldseta", "ldsetal",
};

static char *atomics_swap_mnemonics[16] = {
  "swpb", "swplb", "swpab", "swpalb",
  "swph", "swplh", "swpah", "swpalh",
  "swp", "swpl", "swpa", "swpal",
  "swp", "swpl", "swpa", "swpal",
};

/***************** prints *****************/

static inline void print_separator(void) {
  xprintf(", ");
}

static inline void print_end_of_line(void) {
  xprintf("\n");
}

static inline void print_character(const char c) {
  xprintf("%c", c);
}

static inline void print_string(const char *string) {
  xprintf("%s", string);
}

static inline void print_options(const char *string) {
  xprintf("\033[38;5;242m%s\033[30;0m", string);
}

static inline void print_mnemonic(const char *mnemonic) {
  xprintf("\033[30;0m\t\t%s\t\t", mnemonic);
}

static inline void print_shift_type(unsigned shift) {
  xprintf("\033[38;5;242m%s\033[30;0m", shift_names[shift & 0x3]);
}

static inline void print_shift_value(unsigned immediate) {
  xprintf("\033[38;5;242m#%u\033[30;0m", immediate);
}

static inline void print_signed_immediate(int immediate) {
  xprintf("#\033[38;5;196m0x%x\033[30;0m", immediate);
}

static inline void print_signed_immediate64(int64_t immediate) {
  xprintf("#\033[38;5;196m0x%llx\033[30;0m", immediate);
}

static inline void print_unsigned_immediate(unsigned immediate) {
  xprintf("#\033[38;5;196m0x%x\033[30;0m", immediate);
}
    
static inline void print_unsigned_immediate64(uint64_t immediate) {
  xprintf("#\033[38;5;196m0x%llx\033[30;0m", immediate);
}

static inline void print_register_sub(char *where) {
  xprintf("\033[38;5;227m%s\033[30;0m", where);
}

// about immediates : https://alisdair.mcdiarmid.org/arm-immediate-value-encoding/

static inline void print_register(unsigned reg, unsigned is64Bit) {
  xprintf("\033[0;36m%c%u\033[30;0m", is64Bit ? 'x' : 'w', reg);
}

static inline void print_register32(unsigned reg) {
  reg &= 0xf;

  if (reg > 0xC) {
    xprintf("\033[0;36m%s\033[30;0m", special_registers32[reg - 0xD]);

    return;
  }

  xprintf("\033[0;36mr%u\033[30;0m", reg);
}

static inline void print_selected_register(unsigned reg, unsigned selected) {
  xprintf("\033[0;36m%c%u\033[30;0m", register_prefix[selected], reg);
}

static inline void print_sys_register(char *name, char *definition) {
  xprintf("\033[30;0m ; \033[38;5;177m[>] %s | %s", name, definition);
}

static inline void print_register_sp(unsigned reg, unsigned is64Bit) {
  if (reg == 31) {
    xprintf("\033[0;36m%s\033[30;0m", is64Bit ? "sp" : "wsp");
    return;
  }

  print_register(reg, is64Bit);
}

static inline void print_register_zr(unsigned reg, unsigned is64Bit) {
  if (reg == 31) {
    xprintf("\033[0;36m%s\033[30;0m", is64Bit ? "xzr" : "wzr");
    return;
  }

  print_register(reg, is64Bit);
}

#ifdef __cplusplus
}
#endif

#endif