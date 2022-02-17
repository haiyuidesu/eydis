#ifndef INSTRUCTIONS32_H
#define INSTRUCTIONS32_H

#ifdef __cplusplus
extern "C"
{
#endif
    #include <stdint.h>
    #include <include/disarm.h>
    #include <include/definitions.h>

    /*
     * there is no immediates or xBits declared here
     * (since it would require to alternate between 32 and 16bit insn)
     * but only functions declarations.
     */

    /**************** 16bit instructions ****************/

    unsigned int insn_is_branch_t2(uint16_t mem);
    unsigned int insn_is_add_register_t2(uint16_t mem);
    unsigned int insn_is_branch_exchange(uint16_t mem);
    unsigned int insn_is_cmp_register_t1(uint16_t mem);
    unsigned int insn_is_cmp_register_t2(uint16_t mem);
    unsigned int insn_is_add_substract_t1(uint16_t mem);
    unsigned int insn_is_cmp_immediate_t1(uint16_t mem);
    unsigned int insn_is_add_sp_immediate_32(uint16_t mem);
    unsigned int insn_is_pc_relative_address(uint16_t mem);
    unsigned int insn_is_branch_conditional_t1(uint16_t mem);
    unsigned int insn_is_add_substract_immediate8(uint16_t mem);
    unsigned int insn_is_data_processing_register_t1(uint16_t mem);
    unsigned int insn_is_load_store_register_sp_relative(uint16_t mem);
    unsigned int insn_is_misc_compare_and_branch(uint16_t mem);
    unsigned int insn_is_load_register_immediate(uint16_t mem);
    unsigned int insn_is_load_register_offset_t1(uint16_t mem);
    unsigned int insn_is_misc_compare_and_branch(uint16_t mem);
    unsigned int insn_is_logical_immediate_t1(uint16_t mem);
    unsigned int insn_is_pc_relative_address(uint16_t mem);
    unsigned int insn_is_misc_push_or_pop(uint16_t mem);
    unsigned int insn_is_mov_immediate_t1(uint16_t mem);
    unsigned int insn_is_add_sub_sp_misc(uint16_t mem);
    unsigned int insn_is_mov_register_t1(uint16_t mem);
    unsigned int insn_is_misc_byte_word(uint16_t mem);

    /**************** 32bit instructions ****************/

    unsigned int insn_is_conditional_branch_t3(uint32_t mem);
    unsigned int insn_is_branch_or_branch_link(uint32_t mem);
    unsigned int insn_is_data_processing_shift_register(uint32_t mem);
    unsigned int insn_is_data_processing_modified_immediate(uint32_t mem);
    unsigned int insn_is_data_processing_shifted_register(uint32_t mem);
    unsigned int insn_is_store_single_immediate12(uint32_t mem);
    unsigned int insn_is_store_single_immediate8(uint32_t mem);
    unsigned int insn_is_load_unsigned_immediate(uint32_t mem);
    unsigned int insn_is_load_signed_immediate(uint32_t mem);
    unsigned int insn_is_store_single_register(uint32_t mem);
    unsigned int insn_is_unmodified_immediate(uint32_t mem);
    unsigned int insn_is_pop_or_push_single(uint32_t mem);
    unsigned int insn_is_load_register(uint32_t mem);
    unsigned int insn_is_push_multiple(uint32_t mem);
    unsigned int insn_is_pop_multiple(uint32_t mem);
    unsigned int insn_is_unknown_32(uint32_t mem);

    /**************** list ****************/

    static const current_insn_t available_insn32[] = {
        { NULL, insn_is_branch_t2 },
        { NULL, insn_is_pc_relative_address },
        { NULL, insn_is_add_register_t2 },
        { NULL, insn_is_branch_exchange },
        { NULL, insn_is_cmp_register_t1 },
        { NULL, insn_is_cmp_register_t2 },
        { NULL, insn_is_add_substract_t1 },
        { NULL, insn_is_cmp_immediate_t1 },
        { NULL, insn_is_add_sp_immediate_32 },
        { NULL, insn_is_branch_conditional_t1 },
        { NULL, insn_is_add_substract_immediate8 },
        { NULL, insn_is_data_processing_register_t1 },
        { NULL, insn_is_load_store_register_sp_relative },
        { NULL, insn_is_misc_compare_and_branch },
        { NULL, insn_is_misc_compare_and_branch },
        { NULL, insn_is_load_register_immediate },
        { NULL, insn_is_load_register_offset_t1 },
        { NULL, insn_is_logical_immediate_t1 },
        { NULL, insn_is_misc_push_or_pop },
        { NULL, insn_is_mov_immediate_t1 },
        { NULL, insn_is_mov_register_t1 },
        { NULL, insn_is_add_sub_sp_misc },
        { NULL, insn_is_misc_byte_word },
        { insn_is_unmodified_immediate, NULL },
        { insn_is_conditional_branch_t3, NULL },
        { insn_is_branch_or_branch_link, NULL },
        { insn_is_store_single_register, NULL },
        { insn_is_data_processing_shift_register, NULL },
        { insn_is_store_single_immediate12, NULL },
        { insn_is_store_single_immediate8, NULL },
        { insn_is_load_unsigned_immediate, NULL },
        { insn_is_load_signed_immediate, NULL },
        { insn_is_pop_or_push_single, NULL },
        { insn_is_load_register, NULL },
        { insn_is_push_multiple, NULL },
        { insn_is_pop_multiple, NULL },
        { insn_is_data_processing_modified_immediate, NULL },
        { insn_is_data_processing_shifted_register, NULL },
        { insn_is_unknown_32, NULL },
    };
    uint64_t find_insn32(uint8_t *img, uint64_t start, uint64_t end, eydis_callback_t progress);
#ifdef __cplusplus
}
#endif
#endif
