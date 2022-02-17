#ifndef INSTRUCTIONS64_H
#define INSTRUCTIONS64_H

#ifdef __cplusplus
extern "C"
{
#endif
    #include <stdint.h>
    #include <include/disarm.h>
    #include <include/definitions.h>

    extern char *op_addr;

    /**************** decoders ****************/

    static inline unsigned ind(uint32_t opcode) { return opcode & 0xf; }
    static inline unsigned ind2(uint32_t opcode) { return (opcode >> 10) & 0xf; }

    static inline unsigned rd(uint32_t opcode) { return opcode & 0x1f; }
    static inline unsigned rn(uint32_t opcode) { return (opcode >> 5) & 0x1f; }
    static inline unsigned ra(uint32_t opcode) { return (opcode >> 10) & 0x1f; }
    static inline unsigned rm(uint32_t opcode) { return (opcode >> 16) & 0x1f; }

    static inline unsigned sh0(uint32_t opcode) { return (opcode >> 22) & 0x3; }
    static inline unsigned hw0(uint32_t opcode) { return (opcode >> 21) & 0x3; }

    static inline unsigned size2(uint32_t opcode) { return (opcode >> 30) & 0x3; }
    static inline unsigned size(uint32_t opcode) { return opcode >> 30; }

    static inline unsigned aBit(uint32_t opcode) { return (opcode >> 29) & 0x1; }
    static inline unsigned cBit(uint32_t opcode) { return (opcode >> 26) & 0x1; }
    static inline unsigned sBit(uint32_t opcode) { return (opcode >> 24) & 0x1; }
    static inline unsigned vBit(uint32_t opcode) { return (opcode >> 22) & 0x1; }
    static inline unsigned tBit(uint32_t opcode) { return (opcode >> 12) & 0x1; }
    static inline unsigned mBit(uint32_t opcode) { return (opcode >> 10) & 0x1; }

    static inline unsigned opd(uint32_t opcode) { return (opcode >> 31) & 0x1; }
    static inline unsigned opr(uint32_t opcode) { return (opcode >> 30) & 0x1; }
    static inline unsigned ofm(uint32_t opcode) { return (opcode >> 23) & 0x7; }
    static inline unsigned crN(uint32_t opcode) { return (opcode >> 12) & 0xf; }
    static inline unsigned crM(uint32_t opcode) { return (opcode >> 8) & 0xf; }

    static inline unsigned op7(uint32_t opcode) { return (opcode >> 21) & 0xf; }
    static inline unsigned op0(uint32_t opcode) { return (opcode >> 19) & 0x3; }
    static inline unsigned op1(uint32_t opcode) { return (opcode >> 16) & 0x7; }
    static inline unsigned op6(uint32_t opcode) { return (opcode >> 15) & 0x3f; }
    static inline unsigned op3(uint32_t opcode) { return (opcode >> 10) & 0x3; }
    static inline unsigned op2(uint32_t opcode) { return (opcode >> 5) & 0x7; }
    static inline unsigned op5(uint32_t opcode) { return (opcode >> 3) & 0x3; }

    static inline unsigned opt(uint32_t opcode) { return (opcode >> 13) & 0x7; }
    static inline unsigned opb(uint32_t opcode) { return (opcode >> 21) & 0xf; }
    static inline unsigned opc(uint32_t opcode) { return (opcode >> 29) & 0x3; }

    static inline unsigned o2(uint32_t opcode) { return (opcode >> 23) & 0x1; }
    static inline unsigned o1(uint32_t opcode) { return (opcode >> 21) & 0x1; }
    static inline unsigned o0(uint32_t opcode) { return (opcode >> 15) & 0x1; }

    static inline unsigned immr(uint32_t opcode) { return (opcode >> 16) & 0x3f; }
    static inline unsigned imms(uint32_t opcode) { return (opcode >> 10) & 0x3f; }

    static inline unsigned imm1(uint32_t opcode) { return (int64_t)((opcode >> 5) & 0x7ffff); }
    static inline unsigned imm2(uint32_t opcode) { return (int64_t)((opcode >> 5) & 0x7f); }
    static inline unsigned imm3(uint32_t opcode) { return (int64_t)((opcode >> 10) & 0x7); }
    static inline int64_t imm6(uint32_t opcode) { return (int64_t)(imms(opcode) << 26) >> 26; }
    static inline int64_t imm7(uint32_t opcode) { return ((int64_t)((opcode >> 15) & 0x7f) << 25) >> 25; }
    static inline int64_t imm9(uint32_t opcode) { return ((int64_t)((opcode >> 12) & 0x1ff) << 23) >> 23; }
    static inline unsigned imm12(uint32_t opcode) { return (int64_t)(opcode >> 10) & 0xfff; }
    static inline int imm14(uint32_t opcode) { return ((int64_t)((opcode >> 5) & 0x3fff) << 50) >> 48; }
    static inline unsigned imm16(uint32_t opcode) { return (int64_t)((opcode >> 5) & 0xffff); }
    static inline int64_t imm19(uint32_t opcode) { return ((int64_t)imm1(opcode) << 45) >> 43; }
    static inline int64_t imm26(uint32_t opcode) { return ((int64_t)(opcode & 0x3ffffff) << 38) >> 36; }

    static inline int is64Bit(uint32_t opcode) { return (opcode & 0x80000000) ? 1 : 0; }
    static inline int opnumber(uint32_t opcode) { return (size2(opcode) << 3) | (cBit(opcode) << 2) | hw0(opcode); }
    static inline int is64Bit2(uint32_t opcode) { return ((opnumber(opcode) & 0x17) == 0x02) || ((opnumber(opcode) & 0x1e) == 0x18); }

    /**************** end ****************/

    unsigned int insn_is_adr(uint32_t mem);
    unsigned int insn_is_mrs(uint32_t mem);
    unsigned int insn_is_b_l(uint32_t mem);
    unsigned int insn_is_adrp(uint32_t mem);
    unsigned int insn_is_tlbi(uint32_t mem);
    unsigned int insn_is_mem_related(uint32_t mem);
    unsigned int insn_is_msr_immediate(uint32_t mem);
    unsigned int insn_is_b_conditional(uint32_t mem);
    unsigned int insn_is_unconditional(uint32_t mem);
    unsigned int insn_is_unconditional64(uint32_t mem);
    unsigned int insn_is_add_sub_immediate(uint32_t mem);
    unsigned int insn_is_add_sub_shift_register(uint32_t mem);
    unsigned int insn_is_add_sub_extend_register(uint32_t mem);
    unsigned int insn_is_float_point_data_processing(uint32_t mem);
    unsigned int insn_is_loadstore_unsigned_immediate(uint32_t mem);
    unsigned int insn_is_float_point_int_conversion(uint32_t mem);
    unsigned int insn_is_loadstore_register_pair(uint32_t mem);
    unsigned int insn_is_bitwise_shift_register(uint32_t mem);
    unsigned int insn_is_loadstore_immediate(uint32_t mem);
    unsigned int insn_is_conditional_select(uint32_t mem);
    unsigned int insn_is_loadstore_register(uint32_t mem);
    unsigned int insn_is_bitwise_immediate(uint32_t mem);
    unsigned int insn_is_data_processing(uint32_t mem);
    unsigned int insn_is_ldr_litteral(uint32_t mem);
    unsigned int insn_is_at_dc_msr(uint32_t mem);
    unsigned int insn_is_bitfield(uint32_t mem);
    unsigned int insn_is_unknown(uint32_t mem);
    unsigned int insn_is_process(uint32_t mem);
    unsigned int insn_is_sysinc(uint32_t mem);
    unsigned int insn_is_cb_n_z(uint32_t mem);
    unsigned int insn_is_tb_n_z(uint32_t mem);
    unsigned int insn_is_hint(uint32_t mem);
    unsigned int insn_is_mov(uint32_t mem);

    static const current_insn_t available_insn64[] = {
        { insn_is_adr, NULL },
        { insn_is_mov, NULL },
        { insn_is_b_l, NULL },
        { insn_is_mrs, NULL },
        { insn_is_adrp, NULL },
        { insn_is_tlbi, NULL },
        { insn_is_hint, NULL },
        { insn_is_sysinc, NULL },
        { insn_is_cb_n_z, NULL },
        { insn_is_tb_n_z, NULL },
        { insn_is_process, NULL },
        { insn_is_bitfield, NULL },
        { insn_is_at_dc_msr, NULL },
        { insn_is_mem_related, NULL },
        { insn_is_ldr_litteral, NULL },
        { insn_is_b_conditional, NULL },
        { insn_is_msr_immediate, NULL },
        { insn_is_unconditional, NULL },
        { insn_is_bitwise_immediate, NULL },
        { insn_is_loadstore_register, NULL },
        { insn_is_conditional_select, NULL },
        { insn_is_loadstore_immediate, NULL },
        { insn_is_add_sub_shift_register, NULL },
        { insn_is_loadstore_register_pair, NULL },
        { insn_is_float_point_int_conversion, NULL },
        { insn_is_loadstore_unsigned_immediate, NULL },
        { insn_is_float_point_data_processing, NULL },
        { insn_is_add_sub_extend_register, NULL },
        { insn_is_bitwise_shift_register, NULL },
        { insn_is_add_sub_immediate, NULL },
        { insn_is_data_processing, NULL },
        { insn_is_unknown, NULL },
    };

    uint64_t find_insn64(uint8_t *img, uint64_t start, uint64_t end, eydis_callback_t progress);

#ifdef __cplusplus
}
#endif
#endif
