#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <include/database.h>
#include <include/definitions.h>
#include <include/instructions32.h>

/*
 *  https://developer.arm.com/
 *
 *  Most of this code was taken from there (again):
 *  https://opensource.apple.com/source/JavaScriptCore/
 *  (there is too many code, I regret this support).
 *
 */

unsigned int insn_is_32bit(uint16_t mem)
{
    return (((mem & 0xe000) == 0xe000) && ((mem & 0x1800) != 0x0));
}

uint64_t find_insn32(uint8_t *img, uint64_t start, uint64_t end, eydis_callback_t progress)
{
    char *prologues[] = {"\x80\xb5", "\x90\xb5", "\xb0\xb5", "\xf0\xb5"};

    while (start <= end) {
        if (x == 0) {
            for (int i = 0x0; i != sizeof(prologues) / sizeof(prologues[0]); i++) {
                if (memcmp(&image.img[start], prologues[i], 0x2) == 0)
                {
                    check_data_existence(1, image.base + start);
                }
            }
            progress(((double)start / (double)end) * 50);
        }

        addr = image.base + start;

        uint32_t mem = *(uint32_t *)(img + start);
        uint16_t mem16 = *(uint16_t *)(img + start);

        if (x == 1) {
            print_subroutines(); // this function is guilty of the slowness of eydis...
        }
        // if you comment out these 3 previous lines, it will become a way more faster but it would not make any sense without it...

        xprintf("\033[38;5;242m%s:%llx", image.filetype, addr);

        if (x == 1) {
            current_insn_hex(start);
        }

        for (unsigned int j = 0; j != sizeof(available_insn32); j -= -1) {
            if (available_insn32[j].insn == NULL) {
                if (available_insn32[j].insn16(mem16) == 1) {
                    break;
                }
            } else if (available_insn32[j].insn(mem) == 1) {
                break;
            }
        }

        if (start < 0x81C) {
            start += 0x4;
        } else {
            start += insn_is_32bit(mem16) ? 0x4 : 0x2;
        }
    }

    return 0;
}

unsigned int insn_is_unknown_32(uint32_t mem)
{
    print_mnemonic("ukn");

    print_end_of_line();

    return 1;
}

/************** 16bit instructions **************/

void print_register_list_16(unsigned registers)
{
    unsigned numberPrinted = 0;

    print_string("{ ");

    for (unsigned i = 0; i < 0x10; i++) {
        if (registers & (1 << i)) {
            if (numberPrinted++) {
                print_separator();
            }

            print_register32(i);
        }
    }

    print_string(" }");
}

unsigned int insn_is_add_register_t2(uint16_t mem)
{
    if ((mem & 0xff00) == 0x4400) {
        if (x == 0) return 1;

        print_mnemonic("add");

        print_register32(((mem >> 0x4) & 0x8) | (mem & 0x7));

        print_separator();

        print_register32((mem >> 0x3) & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_add_sp_immediate_32(uint16_t mem)
{
    if ((mem & 0xf800) == 0xc800) {
        if (x == 0) return 1;

        print_mnemonic("add");

        print_register32((mem >> 8) & 0x7);

        print_separator();

        print_register32(0xD);

        print_separator();

        print_unsigned_immediate(mem & 0xff);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_add_substract_t1(uint16_t mem)
{
    if (((mem & 0xfc00) == 0x1800) || ((mem & 0xfc00) == 0x1c00)) {
        if (x == 0) return 1;

        print_mnemonic(((mem >> 9) & 0x1) ? "add" : "sub");

        print_register32(mem & 0x7); // (mem >> 8) & 0x7 ?

        print_separator();

        print_register32((mem >> 0x3) & 0x7);

        print_separator();

        print_register32((mem >> 0x6) & 0x7);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_add_substract_immediate8(uint16_t mem)
{
    if (((mem & 0xf000) == 0x3000)) {
        if (x == 0) return 1;

        print_mnemonic(((mem >> 0xB) & 0x1) ? "add" : "sub");

        print_register32((mem >> 0x8) & 0x7); // (mem >> 8) & 0x7 ?

        print_separator();

        print_register32(mem & 0xff);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_branch_conditional_t1(uint16_t mem)
{
    char mne[4];

    if (((mem & 0xf000) == 0xd000)) {
        switch ((mem >> 0x8) & 0xf) {
        case 0xE:
            return 0;
        case 0xf:
            print_mnemonic("svc");

            print_unsigned_immediate(mem & 0xff);

            print_end_of_line();

            return 1;
        default:
            break;
        }

        sprintf(mne, "b%s", b_condition[(mem >> 0x8) & 0xf]);

        print_mnemonic(mne);

        if (x == 0) {
            check_data_existence(0, addr + ((int32_t)(mem & 0xff) + 0x2));
        } else {
            print_operand_address(addr + ((int32_t)(mem & 0xff) + 0x2));

            print_end_of_line();
        }

        return 1;
    }

    return 0;
}

unsigned int insn_is_branch_exchange(uint16_t mem)
{
    if (((mem & 0xff00) == 0x4700)) {
        if (x == 0) return 1;

        print_mnemonic((mem & 0x80) ? "blx" : "bx");

        print_register32((mem >> 0x3) & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_branch_t2(uint16_t mem)
{
    if (((mem & 0xf800) == 0xe000)) {
        print_mnemonic("b");

        if (x == 0) {
            check_data_existence(0, addr + ((int32_t)(mem & 0x7ff) + 2));
        } else {
            print_operand_address(addr + ((int32_t)(mem & 0x7ff) + 2));

            print_end_of_line();
        }

        return 1;
    }

    return 0;
}

unsigned int insn_is_cmp_immediate_t1(uint16_t mem)
{
    if (((mem & 0xf800) == 0x2800)) {
        if (x == 0) return 1;

        print_mnemonic("cmp");

        print_register32((mem >> 0x8) & 0x3);

        print_separator();

        print_unsigned_immediate(mem & 0xff);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_cmp_register_t1(uint16_t mem)
{
    if (((mem & 0xffc0) == 0x4280)) {
        if (x == 0) return 1;

        print_mnemonic("cmp");

        print_register32(mem & 0x7);

        print_separator();

        print_register32((mem >> 0x3) & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_cmp_register_t2(uint16_t mem)
{
    if (((mem & 0xff00) == 0x4500)) {
        if (x == 0) return 1;

        print_mnemonic("cmp");

        print_register32(((mem >> 0x4) & 0x8) | (mem & 0x7));

        print_separator();

        print_register32((mem >> 0x3) & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_data_processing_register_t1(uint16_t mem)
{
    if ((mem & 0xfc00) == 0x4000) {
        if (x == 0) return 1;

        print_mnemonic(bitwise_mnemonics_register_32[(mem >> 0x6) & 0xf]);

        print_register32(mem & 0x7);

        print_separator();

        print_register32((mem >> 0x3) & 0x7);

        switch ((mem >> 0x6) & 0xf) {
        case 0x9:
            print_unsigned_immediate(0x0);
        case 0xD:
            print_separator();

            print_register32(mem & 0x7);
        default:
            break;
        }

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_pc_relative_address(uint16_t mem)
{
    if (((mem & 0xf800) == 0xa000) || ((mem & 0xf800) == 0x4800)) {
        if (x == 0) return 1;

        print_mnemonic((mem & 0xf800) == 0xa000 ? "adr" : "ldr"); // or load from literal pool

        print_register32((mem >> 0x8) & 0x7);

        print_separator();

        print_unsigned_immediate(addr + (int32_t)(mem & 0xff));

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned scale(uint16_t mem)
{
    switch (((mem >> 11) & 0x1f) - 0xC) {
    case 0x0:
    case 0x1:
        return 2;
    case 0x4:
    case 0x5:
        return 1;
    default:
        break;
    }

    return 0;
}

unsigned int insn_is_load_register_immediate(uint16_t mem)
{
    if (((mem & 0xe000) == 0x6000) || ((mem & 0xf800) == 0x8000) || ((mem & 0xf800) == 0x8800)) {
        if (x == 0) return 1;

        char *mne = loadstore_register_immediate_32[((mem >> 11) & 0x1f) - 0xC];

        if (!mne) return 0;

        print_mnemonic(mne);

        print_register32(mem & 0x7);

        print_separator();

        print_character('[');

        print_register32((mem >> 3) & 0x7);

        if ((mem >> 6) & 0x1f) {
            print_separator();

            print_unsigned_immediate(((mem >> 6) & 0x1f) << scale(mem));
        }

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_load_register_offset_t1(uint16_t mem)
{
    if ((mem & 0xf000) == 0x5000)
    {
        if (x == 0) return 1;

        char *mne = loadstore_unsigned_mnemonics_32[(mem >> 9) & 0x7];

        print_mnemonic(mne);

        print_register32(mem & 0x7);

        print_separator();

        print_character('[');

        print_register32((mem >> 3) & 0x7);

        print_separator();

        print_register32((mem >> 6) & 0x7);

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_load_store_register_sp_relative(uint16_t mem)
{
    if ((mem & 0xf000) == 0x9000) {
        if (x == 0) return 1;

        print_mnemonic((mem >> 11) & 0x1 ? "ldr" : "str");

        print_register32((mem >> 8) & 0x7);

        print_separator();

        print_character('[');

        print_register32(0xD);

        if (mem & 0xff) {
            print_separator();

            print_unsigned_immediate((mem & 0xff) << 0x2);
        }

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_logical_immediate_t1(uint16_t mem)
{
    if ((mem & 0xe000) == 0x0000)
    {
        if (x == 0) return 1;

        if (!((mem >> 12) & 0x3) && !((mem >> 6) & 0x1f)) {
            print_mnemonic("movs");
        } else {
            print_mnemonic(shift_names[(mem >> 12) & 0x3]);
        }

        print_register32((mem >> 3) & 0x7);

        print_separator();

        print_register32(mem & 0x7);

        if (((mem >> 12) & 0x3) && ((mem >> 6) & 0x1f)) {
            print_separator();

            print_unsigned_immediate((((mem >> 12) & 0x3) && !((mem >> 6) & 0x1f)) ? 0x20 : ((mem >> 6) & 0x1f));
        }

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_add_sub_sp_misc(uint16_t mem)
{
    if ((mem & 0xe000) == 0x0) {
        if (x == 0) return 1;

        print_mnemonic((mem >> 0x7) & 0x1 ? "sub" : "add");

        print_register32(0xD);

        print_separator();

        print_register32(0xD);

        print_separator();

        print_unsigned_immediate(mem & 0x7f);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_misc_byte_word(uint16_t mem)
{
    if ((mem & 0xf700) == 0xb200) {
        if (x == 0) return 1;

        print_mnemonic(extend_bit_names_32[((mem >> 9) & 0x4) || ((mem >> 6) & 0x3)]);

        print_register32(mem & 0x7);

        print_separator();

        print_register32((mem >> 3) & 0x7);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_misc_compare_and_branch(uint16_t mem)
{
    if ((mem & 0xf500) == 0xb100) {
        print_mnemonic(((mem >> 11) & 0x1) ? "cbnz" : "cbz");

        if (x == 0) {
            check_data_existence(0, addr + (((mem >> 4) & 0x20) | ((mem >> 3) & 0x1f)) + 0x2);
        } else {
            print_operand_address(addr + (((mem >> 4) & 0x20) | ((mem >> 3) & 0x1f)) + 0x2);

            print_end_of_line();
        }

        return 1;
    }

    return 0;
}

unsigned int insn_is_misc_push_or_pop(uint16_t mem)
{
    if ((mem & 0xf600) == 0xb400) {
        if (x == 0) return 1;

        print_mnemonic(((mem >> 11) & 0x1) ? "pop" : "push");

        print_register_list_16(((mem << 6) & 0x4000) | (mem & 0xff));

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_mov_immediate_t1(uint16_t mem)
{
    if ((mem & 0xf600) == 0xb400) {
        if (x == 0) return 1;

        print_mnemonic("mov");

        print_register32((mem >> 8) & 0x3);

        print_separator();

        print_unsigned_immediate(mem & 0xff);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_mov_register_t1(uint16_t mem)
{
    if ((mem & 0xff00) == 0x4600) {
        if (x == 0) return 1;

        print_mnemonic("mov");

        print_register32(((mem >> 4) & 0x8) | (mem & 0x7));

        print_separator();

        print_register32((mem >> 3) & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

/************** 32bit instructions **************/

void print_register_list_32(uint32_t mem)
{
    unsigned regs = (mem & 0xffff);

    print_string("{ ");

    int separator = 0;

    for (unsigned i = 0; i < 0x10; i++) {
        if (regs & (1 << i)) {
            if (separator) {
                print_separator();
            }

            print_register32(i);

            separator = 1;
        }
    }

    print_string(" }");
}

unsigned int insn_is_push_or_pop_multiple_32(uint32_t mem)
{
    char *what = "pop";
    char which[32];

    if ((mem & 0x0fff0000) != 0x08bd0000) {
        // 'POP' insn not found, trying for a 'PUSH' one

        if ((mem & 0xfe7f0000) != 0xe82d0000)
            return 0;

        what = "push";
    }

    if (x == 0) return 1;

    if ((mem >> 28) != 0xE) {
        sprintf(which, "%s.%s", what, b_condition[(mem >> 28)]);
    } else {
        sprintf(which, "%s", what);
    }

    print_mnemonic(which);

    print_register_list_32(mem);

    print_end_of_line();

    return 1;
}

unsigned int insn_is_conditional_branch_t3(uint32_t mem)
{
    char mne[4];

    int32_t offset = (((int32_t)(((mem >> 26) & 0x1) << 31)) >> 12) |
                     (int32_t)((((mem >> 13) & 0x1) << 18) |
                               (((mem >> 11) & 0x1) << 17) |
                               (((mem >> 16) & 0x3f) << 11) |
                               (mem & 0x7ff));

    if ((mem & 0xf800d000) == 0xf0008000)
    {
        if (((mem >> 22) & 0xf) < 0xE) {
            sprintf(mne, "b%s", b_condition[(mem >> 22) & 0xf]);

            print_mnemonic(mne);
        } else {
            print_mnemonic("b");
        }

        if (x == 0){
            check_data_existence(0, addr + offset + 0x2);
        } else {
            print_operand_address(addr + offset + 0x2);

            print_end_of_line();
        }

        return 1;
    }

    return 0;
}

unsigned int insn_is_branch_or_branch_link(uint32_t mem)
{
    int32_t offset = (((int32_t)(((mem >> 26) & 0x1) << 31)) >> 8) |
                     (int32_t)((~(((mem >> 13) & 0x1) ^ ((mem >> 26) & 0x1)) << 22) |
                               (~(((mem >> 11) & 0x1) ^ ((mem >> 26) & 0x1)) << 21) |
                               (((mem >> 16) & 0x3ff) << 11) |
                               (mem & 0x7ff));

    if ((mem & 0xf8009000) == 0xf0009000) {
        print_mnemonic(((mem >> 14) & 0x1) ? "bl" : "b");

        if (x == 0) {
            check_data_existence(((mem >> 14) & 0x1) ? 1 : 0, addr + offset + 0x2);
        } else {
            print_operand_address(addr + offset + 0x2);
        }

        print_end_of_line();

        return 1;
    }

    return 0;
}

void print_modified_immediate(unsigned imm)
{
    if (!(imm & 0xc00)) {
        unsigned immediate = 0;
        unsigned low_than_8_bits = imm & 0xff;

        switch ((imm >> 8) & 3) {
        case 0:
            immediate = low_than_8_bits;
            break;
        case 1:
            immediate = (low_than_8_bits << 16) | low_than_8_bits;
            break;
        case 2:
            immediate = (low_than_8_bits << 24) | (low_than_8_bits << 8);
            break;
        case 3:
            immediate = (low_than_8_bits << 24) | (low_than_8_bits << 16) | (low_than_8_bits << 8) | low_than_8_bits;
            break;
        }

        print_unsigned_immediate(immediate);

        return;
    }

    print_unsigned_immediate((0x80 | (imm & 0x7f)) << (0x20 - ((imm >> 7) & 0x1f))); // imm | shift
}

unsigned int insn_is_data_processing_modified_immediate(uint32_t mem)
{
    if ((mem & 0xfa008000) == 0xf0000000) {
        if (x == 0) return 1;

        switch ((mem >> 21) & 0xf) {
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x9:
        case 0xC:
        case 0xf:
            return 0;
        default:
            break;
        }

        char *mne = bitwise_mnemonics_logical_32[(mem >> 21) & 0xf];

        if (((mem >> 16) & 0xf) == 0xf) {
            if (((mem >> 21) & 0xf) == 2) {
                mne = ((mem >> 20) & 0x1) ? "movs" : "mov";

                print_mnemonic(mne);

                print_register32((mem >> 8) & 0xf);

                print_separator();

                print_modified_immediate(((mem >> 15) & 0x800) | ((mem >> 4) & 0x700) | (mem & 0xff));

                print_end_of_line();

                return 1;
            }

            if (((mem >> 21) & 0xf) == 3) {
                mne = ((mem >> 20) & 0x1) ? "mvns" : "mvn";

                print_mnemonic(mne);

                print_register32((mem >> 8) & 0xf);

                print_separator();

                print_modified_immediate(((mem >> 15) & 0x800) | ((mem >> 4) & 0x700) | (mem & 0xff));

                print_end_of_line();

                return 1;
            }
        }

        if (((mem >> 8) & 0xf) == 0xf) {
            if ((mem >> 20) & 0x1) {
                int test_or_cmp = 0;

                switch ((mem >> 21) & 0xf) {
                case 0x0:
                    mne = "tst";
                    test_or_cmp = 1;
                    break;
                case 0x4:
                    mne = "teq";
                    test_or_cmp = 1;
                    break;
                case 0x8:
                    mne = "cmn";
                    test_or_cmp = 1;
                    break;
                case 0xd:
                    mne = "cmp";
                    test_or_cmp = 1;
                    break;
                }

                if (test_or_cmp) {
                    print_mnemonic(mne);

                    print_register32((mem >> 16) & 0xf);

                    print_separator();

                    print_modified_immediate(((mem >> 15) & 0x800) | ((mem >> 4) & 0x700) | (mem & 0xff));

                    print_end_of_line();

                    return 1;
                }
            }
        }

        print_mnemonic(mne);

        print_register32((mem >> 8) & 0xf);

        print_separator();

        print_register32((mem >> 16) & 0xf);

        print_separator();

        print_modified_immediate(((mem >> 15) & 0x800) | ((mem >> 4) & 0x700) | (mem & 0xff));

        print_end_of_line();

        return 1;
    }

    return 0;
}

void print_immediate_shift(unsigned type, unsigned imm)
{
    if (type || imm) {
        print_separator();

        if (!imm) {
            switch (type) {
            case 0x1:
            case 0x2:
                imm = 0x20;
                break;
            case 0x3:
                print_string("rrx");
                return;
            }
        }

        print_shift_type(type);

        print_unsigned_immediate(imm);
    }
}

unsigned int insn_is_data_processing_shifted_register(uint32_t mem)
{
    if ((mem & 0xfe000000) == 0xea000000) {
        if (x == 0) return 1;

        switch ((mem >> 21) & 0xf) {
        case 0x5:
        case 0x7:
        case 0x9:
        case 0xC:
        case 0xf:
            return 0;
        default:
            break;
        }

        if (((mem >> 21) & 0xf) == 0x6) {
            if (((mem >> 20) & 0x1) || ((mem >> 4) & 0x1))
                return 0;

            if ((mem >> 5) & 0x1) {
                print_mnemonic("pkhtb");
           } else {
                print_mnemonic("pkhbt");
            }

            print_register32((mem >> 8) & 0xf);

            print_separator();

            print_register32((mem >> 16) & 0xf);

            print_separator();

            print_register32(mem & 0xf);

            print_immediate_shift(((mem >> 5) & 0x1) << 1, ((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3));

            print_end_of_line();

            return 1;
        }

        char *mne = bitwise_mnemonics_logical_32[(mem >> 21) & 0xf];

        if (((mem >> 16) & 0xf) == 0xf) {
            if (((mem >> 21) & 0xf) == 0x2) {
                if (!((mem >> 4) & 0x3) && !(((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3))) {
                    mne = ((mem >> 20) & 0x1) ? "movs" : "mov";

                    print_mnemonic(mne);

                    print_register32((mem >> 8) & 0xf);

                    print_separator();

                    print_register32(mem & 0xf);

                    print_end_of_line();

                    return 1;
                }

                if ((((mem >> 4) & 0x3) == 3) && !(((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3))) {
                    mne = ((mem >> 20) & 0x1) ? "rrx" : "rrx";

                    print_mnemonic(mne);

                    print_register32((mem >> 8) & 0xf);

                    print_separator();

                    print_register32(mem & 0xf);

                    print_end_of_line();

                    return 1;
                }

                mne = shift_names[(mem >> 4) & 0x3];

                if ((mem >> 20) & 0x1) {
                    strcat(mne, "s");
                }

                print_mnemonic(mne);

                print_register32((mem >> 8) & 0xf);

                print_separator();

                print_register32(mem & 0xf);

                print_separator();

                print_unsigned_immediate(((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3));

                print_end_of_line();

                return 1;
            }

            if (((mem >> 21) & 0xf) == 0x3) {
                mne = ((mem >> 20) & 0x1) ? "mvns" : "mvn";

                print_mnemonic(mne);

                print_register32((mem >> 8) & 0xf);

                print_separator();

                print_register32(mem & 0xf);

                print_immediate_shift((mem >> 4) & 0x3, ((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3));

                print_end_of_line();

                return 1;
            }
        }

        if (((mem >> 8) & 0xf) == 0xf) {
            if ((mem >> 20) & 0x1) {
                int test_or_cmp = 0;

                switch ((mem >> 21) & 0xf) {
                case 0x0:
                    mne = "tst";
                    test_or_cmp = 1;
                    break;
                case 0x4:
                    mne = "teq";
                    test_or_cmp = 1;
                    break;
                case 0x8:
                    mne = "cmn";
                    test_or_cmp = 1;
                    break;
                case 0xd:
                    mne = "cmp";
                    test_or_cmp = 1;
                    break;
                }

                if (test_or_cmp) {
                    print_mnemonic(mne);

                    print_register32((mem >> 16) & 0xf);

                    print_separator();

                    print_register32(mem & 0xf);

                    print_immediate_shift((mem >> 4) & 0x3, ((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3));

                    print_end_of_line();

                    return 1;
                }
            }
        }

        print_mnemonic(mne);

        print_register32((mem >> 8) & 0xf);

        print_separator();

        print_register32((mem >> 16) & 0xf);

        print_separator();

        print_register32(mem & 0xf);

        print_immediate_shift((mem >> 4) & 0x3, ((mem >> 10) & 0x1C) | ((mem >> 6) & 0x3));

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_data_processing_shift_register(uint32_t mem)
{
    if ((mem & 0xffe0f0f0) == 0xfa00f000) {
        if (x == 0) return 1;

        print_mnemonic(shift_names[(((mem >> 20) & 0xf) >> 1) & 0x3]);

        print_register32((mem >> 8) & 0xf);

        print_separator();

        print_register32((mem >> 16) & 0xf);

        print_separator();

        print_register32(mem & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_load_register(uint32_t mem)
{
    if ((mem & 0xfe900800) == 0xf8100000) {
        if (x == 0) return 1;

        print_mnemonic(data_load_mnemonics_32[((mem >> 22) & 0x4) | ((mem >> 21) & 0x3)]);

        print_register32((mem >> 12) & 0xf);

        print_separator();

        print_character('[');

        print_register32((mem >> 16) & 0xf);

        print_separator();

        print_register32(mem & 0xf);

        if ((mem >> 4) & 0x3) {
            print_separator();

            print_unsigned_immediate((mem >> 4) & 0x3);
        }

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_load_signed_immediate(uint32_t mem)
{
    if ((mem & 0xfe900800) == 0xf8100800) {
        if (x == 0) return 1;

        print_mnemonic(data_load_mnemonics_32[((mem >> 22) & 0x4) | ((mem >> 21) & 0x3)]);

        print_register32((mem >> 12) & 0xf);

        print_separator();

        print_character('[');

        print_register32((mem >> 16) & 0xf);

        if ((mem >> 10) & 0x1) {
            if (((mem >> 8) & 0x1) || (mem & 0xff)) {
                print_separator();

                if ((mem >> 9) & 0x1) {
                    print_unsigned_immediate(mem & 0xff);
               } else {
                    print_signed_immediate(-(int)(mem & 0xff));
                }
            }

            print_character(']');

            if ((mem >> 8) & 0x1)
                print_character('!');
        } else {
            print_character(']');

            print_separator();

            if ((mem >> 9) & 0x1) {
                print_unsigned_immediate(mem & 0xff);
           } else {
                print_signed_immediate(-(int)(mem & 0xff));
            }
        }

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_load_unsigned_immediate(uint32_t mem)
{
    if ((mem & 0xfe900000) == 0xf8900000) {
        if (x == 0) return 1;

        print_mnemonic(data_load_mnemonics_32[((mem >> 22) & 0x4) | ((mem >> 21) & 0x3)]);

        print_register32((mem >> 12) & 0xf);

        print_separator();

        print_character('[');

        print_register32((mem >> 16) & 0xf);

        if (mem & 0xfff) {
            print_separator();

            print_unsigned_immediate(mem & 0xfff);
        }

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_pop_or_push_single(uint32_t mem)
{
    if ((mem & 0xffef0fff) == 0xf84d0d04) {
        if (x == 0) return 1;

        print_mnemonic((mem >> 20) & 0x1 ? "pop" : "push");

        print_register32((mem >> 12) & 0xf);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_pop_multiple(uint32_t mem)
{
    char mne[8];

    if ((mem & 0x0fff0000) == 0x08bd0000) {
        if (x == 0)
            return 1;

        if ((mem >> 28) != 0xE) {
            sprintf(mne, "pop%s", b_condition[mem >> 28]);

            print_mnemonic(mne);
        } else {
            print_mnemonic("pop");
        }

        print_register_list_32(mem);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_push_multiple(uint32_t mem)
{
    char mne[8];

    if ((mem & 0xfe7f0000) == 0xe82d0000) {
        if (x == 0) return 1;

        if ((mem >> 28) != 0xE) {
            sprintf(mne, "push%s", b_condition[mem >> 28]);

            print_mnemonic(mne);
        } else {
            print_mnemonic("push");
        }

        print_register_list_32(mem);

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_store_single_immediate12(uint32_t mem)
{
    if ((mem & 0xfff00000) == 0xf8c00000) {
        if (x == 0) return 1;

        print_mnemonic(data_store_single_mnemonics_32[(mem >> 21) & 0x3]);

        print_register32((mem >> 12) & 0xf);

        print_separator();

        print_character('[');

        print_register32((mem >> 16) & 0xf);

        if (mem & 0xfff) {
            print_separator();

            print_unsigned_immediate(mem & 0xfff);
        }

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_store_single_immediate8(uint32_t mem)
{
    if ((mem & 0xfff00800) == 0xf8400800)
    {
        if (x == 0) return 1;

        if (((mem >> 10) & 0x1) && ((mem >> 9) & 0x1) && !((mem >> 8) & 0x1))
            return 0;

        if ((((mem >> 16) & 0xf) == 0xf) || (!((mem >> 10) & 0x1) && !((mem >> 8) & 0x1)))
            return 0;

        print_mnemonic(data_store_single_mnemonics_32[(mem >> 21) & 0x3]);

        print_register32((mem >> 12) & 0xf);

        print_separator();

        print_character('[');

        print_register32((mem >> 16) & 0xf);

        if (!((mem >> 10) & 0x1)) {
            print_character(']');

            print_separator();

            print_signed_immediate(((mem >> 9) & 0x1) ? (int32_t)(mem & 0xff) : -(int32_t)(mem & 0xff));

            print_end_of_line();
            return 1;
        }

        if (mem & 0xff) {
            print_separator();

            print_signed_immediate(((mem >> 9) & 0x1) ? (int32_t)(mem & 0xff) : -(int32_t)(mem & 0xff));
        }

        print_character(']');

        if ((mem >> 8) & 0x1) print_character('!');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_store_single_register(uint32_t mem)
{
    if ((mem & 0xfff00fc0) == 0xf8400000) {
        if (x == 0) return 1;

        print_mnemonic(data_store_single_mnemonics_32[(mem >> 21) & 0x3]);

        print_register32((mem >> 12) & 0xf);

        print_separator();

        print_character('[');

        print_register32((mem >> 16) & 0xf);

        print_separator();

        print_register32(mem & 0xf);

        if ((mem >> 4) & 0x3) {
            print_separator();

            print_string("lsl");

            print_unsigned_immediate((mem >> 4) & 0x3);
        }

        print_character(']');

        print_end_of_line();

        return 1;
    }

    return 0;
}

unsigned int insn_is_unmodified_immediate(uint32_t mem)
{
    if ((mem & 0xfa008000) == 0xf2000000) {
        if (x == 0) return 1;

        char *mne = extract_bit_names_32[((mem >> 20) & 0x1f) >> 0x1];

        switch (((mem >> 20) & 0x1f) >> 0x1) {
        case 0x0:
        case 0x5:
            if (((mem >> 16) & 0xf) == 0xf) mne = "adr";
            break;
        case 0x9:
            if (((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3)) mne = "ssat";
            break;
        case 0xB:
            if (((mem >> 16) & 0xf) == 0xf) mne = "bfc";
            break;
        case 0xD:
            if (((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3)) mne = "usat";
            break;
        }

        if (!mne) return 0;

        print_mnemonic(mne);

        print_register32((mem >> 8) & 0xf);

        print_separator();

        if ((((mem >> 20) & 0x1f) & 0x17) == 0x4) {
            print_unsigned_immediate(((mem >> 4) & 0xf000) | ((mem >> 15) & 0x800) | ((mem >> 4) & 0x700) | (mem & 0xff));

            print_end_of_line();

            return 1;
        }

        if (!((mem >> 20) & 0x1f) || (((mem >> 20) & 0x1f) == 0xa)) {
            if (((mem >> 16) & 0xf) == 0xf) {
                int32_t offset;

                if ((((mem >> 20) & 0x1f) == 0xa) && (((mem >> 16) & 0xf) == 0xf)) {
                    offset = -(int32_t)((((mem >> 15) & 0x0800) | ((mem >> 4) & 0x700) | (mem & 0xff)));
               } else {
                    offset = (int32_t)((((mem >> 15) & 0x0800) | ((mem >> 4) & 0x700) | (mem & 0xff)));
                }

                print_unsigned_immediate(addr + offset);

                print_end_of_line();

                return 1;
            }

            print_register32((mem >> 16) & 0xf);

            print_separator();

            print_unsigned_immediate(((mem >> 15) & 0x800) | ((mem >> 4) & 0x700) | (mem & 0xff));

            print_end_of_line();

            return 1;
        }

        if (((((mem >> 20) & 0x1f) & 0x15) == 0x10) || (((((mem >> 20) & 0x1f) & 0x17) == 0x12) &&
                                                        (((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3))))
        {
            print_unsigned_immediate((mem & 0x1f) + 1);

            print_separator();

            print_register32((mem >> 16) & 0xf);

            if (((mem >> 21) & 0x1) || ((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3)) {
                print_separator();

                print_shift_type(((mem >> 21) & 0x1) << 1);

                print_unsigned_immediate(((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3));
            }

            print_end_of_line();

            return 1;
        }

        if (((mem >> 20) & 0x1f) == 0x16) {
            int width = (int)(mem & 0x1f) - (int)(((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3)) + 1;

            if (width < 0) return 0;

            if (((mem >> 16) & 0xf) != 0xf) {
                print_register32((mem >> 16) & 0xf);

                print_separator();
            }

            print_unsigned_immediate(((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3));

            print_separator();

            print_signed_immediate(width);

            print_end_of_line();

            return 1;
        }

        print_register32((mem >> 16) & 0xf);

        print_separator();

        print_unsigned_immediate(((mem >> 9) & 0x1c) | ((mem >> 6) & 0x3));

        print_separator();

        print_unsigned_immediate((mem & 0x1f) + 1);

        print_end_of_line();

        return 1;
    }

    return 0;
}
