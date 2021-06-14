#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <include/utils.h>
#include <include/database.h>
#include <include/registers.h>
#include <include/definitions.h>
#include <include/instructions64.h>

char *def = NULL;
char *name = NULL;
char *op_addr = NULL;

/*
 *  https://github.com/evilpenguin/ARMRef
 *
 *  Most of this code was taken from there:
 *  https://opensource.apple.com/source/JavaScriptCore/
 *  I just added some code to get a 'more (not fully) complete' disassembler
 *
 *  (It was a pain to understand everything but I did learned a lot!)
 */

unsigned highestBitSet(unsigned value) {
  unsigned result = 0;

  while (value >>= 1) result++;

  return result;
}

uint64_t rotateRight(uint64_t value, unsigned width, unsigned shift) {
  uint64_t result = value;

  if (shift) result = (value >> (shift % width)) | (value << (width - shift));

  return result;
}

uint64_t replicate(uint64_t value, unsigned width) {
  uint64_t result = 0;

  for (unsigned totalBits = 0; totalBits < 0x40; totalBits += width)
    result = (result << width) | value;

  return result;
}

int LogicalImmediate(unsigned vbit, unsigned imm_s, unsigned imm_r, uint64_t *shift) {
  unsigned len = highestBitSet((vbit << 6) | (imm_s ^ 0x3f));

  if (len < 1) return -1;

  unsigned levels = (1 << len) - 1;

  if (1 && (imm_s & levels) == levels) return -1;

  unsigned r = imm_r & levels;
  unsigned s = imm_s & levels;

  unsigned esize = 1 << len;

  *shift = replicate(rotateRight(((uint64_t)1 << (s + 1)) - 1, esize, r), esize);

  return 0;
}

unsigned int insn_is_hint(uint32_t mem) {
  if ((mem & 0xfffff01f) == 0xd503201f) {
    if (imm2(mem) > 0x20) return 0;

    print_mnemonic(hints_mnemonics[imm2(mem)]);

    print_end_of_line();

    return 1;
  }

  return 0;
}

// unknown instruction (or not implemented yet)
unsigned int insn_is_unknown(uint32_t mem) {
  print_mnemonic("ukn");

  print_end_of_line();

  return 1;
}

unsigned int insn_is_add_sub_immediate(uint32_t mem) {
  if ((mem & 0x1f000000) == 0x11000000) {
    if (aBit(mem) && rd(mem) == 0x1f) {
      print_mnemonic(opr(mem) ? "cmp" : "cmn");
    } else {
      if (!opr(mem) && !imm12(mem) && ((rd(mem) == 0x1f) || rn(mem) == 0x1f)) {
        print_mnemonic("mov");
      } else {
        print_mnemonic(calc_mnemonics[opc(mem)]);
      }

      print_register_sp(rd(mem), is64Bit(mem));

      print_separator();
    }

    print_register_sp(rn(mem), is64Bit(mem));

    if (!(!opc(mem) && !imm12(mem) && ((rd(mem) == 0x1f) || rn(mem) == 0x1f))) {
      print_separator();

      print_unsigned_immediate(imm12(mem));

      if (sh0(mem)) {
        print_separator();

        print_shift_type(sh0(mem));

        print_shift_value(hw0(mem) * 0x6);
      }
    }

    print_end_of_line();

    return 1;
	}

  return 0;
}

unsigned int insn_is_add_sub_extend_register(uint32_t mem) {
  if ((mem & 0x1fe00000) == 0x0b200000) {
    if (imm3(mem) > 4) return 0;

    if (aBit(mem) && rd(mem) == 1) {
      print_mnemonic(opr(mem) ? "cmp" : "cmn");
    } else {
      print_mnemonic(calc_mnemonics[opc(mem)]);

      print_register_sp(rd(mem), is64Bit(mem));

      print_separator();
    }

    print_register_sp(rn(mem), is64Bit(mem));

    print_separator();

    print_register_zr(rm(mem), is64Bit(mem) && ((opt(mem) & 0x3) == 0x3));

    print_separator();

    if (opt(mem) == 0x2 && ((rd(mem) == 0x1f) || (rn(mem) == 0x1f))) {
      print_options("lsl");
    } else {
      print_options(extend_bit_names[opt(mem)]);
    }

    if (imm3(mem)) {
      print_character(' ');

      print_unsigned_immediate(imm3(mem));
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_add_sub_shift_register(uint32_t mem) {
  if ((mem & 0x1f200000) == 0x0b000000) {
    if (!is64Bit(mem) && imm6(mem) & 0x20) return 0;

    if (sh0(mem) == 0x3) return 0;

    if (aBit(mem) && rd(mem) == 0x1f) {
      print_mnemonic(opr(mem) ? "cmp" : "cmn");
    } else {
      if (opr(mem) && rn(mem) == 0x1f) {
        print_mnemonic(opr(mem) ? "cmp" : "cmn");
      } else {
        print_mnemonic(calc_mnemonics[opc(mem)]);
      }

      print_register_sp(rd(mem), is64Bit(mem));

      print_separator();
    }

    if (!(opr(mem) && rn(mem) == 0x1f)) {
      print_register(rn(mem), is64Bit(mem));

      print_separator();
    }

    print_register_zr(rm(mem), is64Bit(mem));

    if (imm6(mem)) {
      print_separator();

      print_shift_type(sh0(mem));

      print_shift_value(imm6(mem));
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_cb_n_z(uint32_t mem) {
  if ((mem & 0x7e000000) == 0x34000000) {
    print_mnemonic(sBit(mem) ? "cbnz" : "cbz");

    print_register(rd(mem), is64Bit(mem));

    print_separator();

    if (x == 0) {
      check_data_existence(0, addr + imm19(mem));
    } else {
      print_operand_address(addr + imm19(mem));

      print_end_of_line();
    }

    return 1;
  }

  return 0;
}

unsigned int insn_is_tb_n_z(uint32_t mem) {
  if ((mem & 0x7e000000) == 0x36000000) {
    print_mnemonic(sBit(mem) ? "tbnz" : "tbz");

    print_register(rd(mem), is64Bit(mem));

    print_separator();

    print_unsigned_immediate(((mem >> 0x1a) & 0x20) | ((mem >> 0x13) & 0x1f));

    print_separator();

    if (x == 0) {
      check_data_existence(0, addr + imm14(mem));
    } else {
      print_operand_address(addr + imm14(mem));

      print_end_of_line();
    }

    return 1;
  }

  return 0;
}

unsigned int insn_is_mov(uint32_t mem) {
  if ((mem & 0x1f800000) == 0x12800000) {
    if (opc(mem) == 1) return 0;

    if (!is64Bit(mem) && hw0(mem) >= 0x2) return 0;

    if (!opc(mem) && (!imm16(mem) || !hw0(mem)) && (is64Bit(mem) || imm16(mem) != 0xffff)) {
      print_mnemonic("mov");

      print_register(rd(mem), is64Bit(mem));

      print_separator();

      if (is64Bit(mem)) {
        int64_t amount = imm16(mem) << (hw0(mem) * 0x10);
        amount = ~amount;
        print_signed_immediate64(amount);
      } else {
        int32_t amount = imm16(mem) << (hw0(mem) * 0x10);
        amount = ~amount;
        print_signed_immediate(amount);
      }
    } else {
      print_mnemonic(mov_mnemonics[opc(mem)]);

      print_register(rd(mem), is64Bit(mem));

      print_separator();

      print_unsigned_immediate(imm16(mem));

      if (hw0(mem)) {
        print_separator();

        print_shift_type(0);

        print_shift_value(hw0(mem) * 0x10);
      }
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_b_l(uint32_t mem) {
  char *which = NULL;

  if ((mem & 0x7c000000) == 0x14000000) {
    which = opd(mem) ? "bl" : "b";

    print_mnemonic(which);

    if (x == 0) {
      check_data_existence(opd(mem) ? 1 : 0, addr + imm26(mem));
    } else {
      print_operand_address(addr + imm26(mem));

      print_end_of_line();
    }

    return 1;
  }

  return 0;
}

unsigned int insn_is_b_conditional(uint32_t mem) {
  char which[32];

  if ((mem & 0xff000010) == 0x54000000) {
    sprintf(which, "b.%s", b_condition[ind(mem)]);

    print_mnemonic(which);

    if (x == 0) {
      check_data_existence(0, addr + imm19(mem));
    } else {
      print_operand_address(addr + imm19(mem));

      print_end_of_line();
    }

    return 1;
  }

  return 0;
}

unsigned int insn_is_unconditional(uint32_t mem) {
  if ((mem & 0xfe1f0000) == 0xd61f0000) {
    unsigned mnemonic = (op7(mem) << 1) | mBit(mem);

    if (rm(mem) == 0x1f && (imms(mem) & 0x3e) == 0x2) {
      if (mnemonic >= 20) return 0;

      if ((rn(mem) != 0x1f) && (op7(mem) == 0x2 || op7(mem) == 0x4)) return 0;

      print_mnemonic(auth_unconditional_mnemonics[mnemonic]);

      if ((op7(mem) & 0x7) <= 0x1) {
        print_register(rn(mem), 1);
      }

      if (op7(mem) & 0x8) {
        print_separator();

        print_register(rm(mem), 1);
      }

      print_end_of_line();

      return 1;
    }

    if (op7(mem) == 0x3 || op7(mem) > 0x5) return 0;

    if (((op7(mem) & 0xe) == 0x4) && rn(mem) != 0x1f) return 0;

    print_mnemonic(unconditional_mnemonics[op7(mem)]);

    if (op7(mem) <= 2 && strcmp(unconditional_mnemonics[op7(mem)], "ret")) {
      print_register(rn(mem), 1);
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_conditional_select(uint32_t mem) {
  if ((mem & 0x1fe00000) == 0x1a800000) {
    unsigned opnb = (opr(mem) << 0x1 | (op3(mem) & 0x1));

    if (aBit(mem)) return 0;

    if (op3(mem) & 0x2) return 0;

    if (rn(mem) == rm(mem) && (opnb == 0x1 || opnb == 0x2)) {
      if (rn(mem) == 0x1f) {
          print_mnemonic((opnb == 1) ? "cset" : "csetm");

          print_register(rd(mem), is64Bit(mem));
        } else {
          print_mnemonic((opnb == 1) ? "cinc" : "cinv");

          print_register(rd(mem), is64Bit(mem));

          print_separator();

          print_register_zr(rn(mem), is64Bit(mem));
        }

      print_separator();

      print_options(b_condition[crN(mem) ^ 0x1]);

      print_end_of_line();

      return 1;
    }
    
    print_mnemonic(condition_select_mnemonics[opnb]);

    print_register(rd(mem), is64Bit(mem));

    print_separator();

    print_register(rn(mem), is64Bit(mem));

    print_separator();

    print_register(rm(mem), is64Bit(mem));

    print_separator();

    print_options(b_condition[crN(mem)]);

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_data_processing(uint32_t mem) {
  if ((mem & 0x5fe00000) == 0x5ac00000) {
    if (aBit(mem)) return 0;

    if (rm(mem) == 1 && is64Bit(mem) && imms(mem) <= 0x1001) {
      if (imms(mem) <= 0x00111 || rd(mem) == 0x11111) {
        print_mnemonic(pac_auth_mnemonics[imms(mem)]);

        print_register_zr(rd(mem), is64Bit(mem));

        if (imms(mem) <= 0x00111) {
          print_separator();

          print_register_zr(rn(mem), is64Bit(mem));
        }

        print_end_of_line();

        return 1;
      }

      return 0;
    }

    if (rm(mem)) return 0;

    if (imms(mem) & 0x38) return 0;

    if ((imms(mem) & 0x3e) == 0x6) return 0;

    if (is64Bit(mem) && imms(mem) == 0x3) return 0;

    if (!is64Bit(mem) && imms(mem) == 0x2) {
      print_mnemonic("rev");
    } else {
      print_mnemonic(data_process_mnemonics[imms(mem) & 0x7]);
    }

    print_register_zr(rd(mem), is64Bit(mem));

    print_separator();

    print_register_zr(rn(mem), is64Bit(mem));

    print_end_of_line();

    return 1;
  }
    
  return 0;
}

unsigned int insn_is_bitwise_shift_register(uint32_t mem) {
  unsigned opn = opc(mem) << 1;

  if ((mem & 0x1f000000) == 0x0a000000) {
    if (!is64Bit(mem) && imm6(mem) & 0x20) return 0;

    if ((opn == 0x3) && (rd(mem) == 0x1f)) {
      print_mnemonic("tst");
    } else {
      if ((opn == 0x2) && (rn(mem) == 0x1f)) {
        print_mnemonic(vBit(mem) ? "mvn" : "mov");
      } else {
        print_mnemonic(bitwise_mnemonics[opn | vBit(mem)]);
      }

      print_register_zr(rd(mem), is64Bit(mem));

      print_separator();
    }

    if (!((opn == 0x2) && (rn(mem) == 0x1f))) {
      print_register_zr(rn(mem), is64Bit(mem));

      print_separator();
    }

    print_register_zr(rm(mem), is64Bit(mem));

    if (imm6(mem)) {
      print_separator();

      print_shift_type(sh0(mem));

      print_shift_value(imm6(mem));
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_bitwise_immediate(uint32_t mem) {
  if ((mem & 0x1f800000) == 0x12000000) {
    if (!is64Bit(mem) && vBit(mem)) return 0;

    unsigned len = highestBitSet(vBit(mem) << 0x6 | (imms(mem) ^ 0x3f));
    unsigned levels = (0x1 << len) - 0x1; // len number of 1 bits starting at LSB

    if ((imms(mem) & levels) == levels) return 0;

    unsigned r = immr(mem) & levels;
    unsigned s = imms(mem) & levels;

    unsigned eSize = 0x1 << len;

    uint64_t pattern = rotateRight(((uint64_t)1 << (s + 0x1)) - 0x1, eSize, r);

    uint64_t immediate = replicate(pattern, eSize);

    if (!is64Bit(mem)) immediate &= (uint64_t)0xffffffff;

    if (((opc(mem) << 1) == 0x6) && (rd(mem) == 0x1f)) {
      print_mnemonic("tst");
    } else {
      if (((opc(mem) << 1) == 0x2) && (rn(mem) == 0x1f)) {
        print_mnemonic("mov");
      } else {
        print_mnemonic(bitwise_mnemonics[(opc(mem) << 1)]);
      }

      print_register(rd(mem), is64Bit(mem));

      print_separator();
    }

    if (!(((opc(mem) << 0x1) == 0x2) && (rn(mem) == 0x1f))) {
      print_register_zr(rn(mem), is64Bit(mem));

      print_separator(); // mov
    }

    print_unsigned_immediate64(immediate);

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_loadstore_register_pair(uint32_t mem) {
  unsigned shifted;

  const char *mnemonic = NULL;

  if ((mem & 0x3a000000) == 0x28000000) {
    if (!cBit(mem) && vBit(mem) && size2(mem) == 0x1) {
      mnemonic = "ldpsw";
    } else if (!cBit(mem) && vBit(mem) && !sBit(mem) && !o2(mem)) {
      mnemonic = "ldnp";
    } else if (!vBit(mem) && !sBit(mem) && !o2(mem)) {
      mnemonic = "stnp";
    } else if (vBit(mem)) {
      mnemonic = "ldp";
    } else {
      mnemonic = "stp";
    }
  }

  if (mnemonic != NULL) {
    print_mnemonic(mnemonic);

    if (cBit(mem)) {
      print_selected_register(rd(mem), size(mem));

      print_separator();

      print_selected_register(ra(mem), size(mem));

      shifted = size(mem) + 0x2;
    } else {
      if (!vBit(mem)) {
        print_register_zr(rd(mem), is64Bit(mem));
      } else {
        print_register(rd(mem), is64Bit(mem));
      }

      print_separator();

      if (!vBit(mem)) {
        print_register_zr(ra(mem), is64Bit(mem));
      } else {
        print_register(ra(mem), is64Bit(mem));
      }

      shifted = (size2(mem) >> 0x1) + 0x2;
    }

    print_separator();

    print_character('[');

    print_register_sp(rn(mem), 0x1);

    int offset = imm7(mem) << shifted;

    if (offset == 0x0) {
      print_character(']');
    } else if (ofm(mem) == 1) {
      print_character(']');

      print_separator();

      print_signed_immediate(offset);
    } else {
      print_separator();

      print_signed_immediate(offset);

      print_character(']');

      if (ofm(mem) == 0x3) print_character('!');
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_loadstore_unsigned_immediate(uint32_t mem) {
  unsigned scale;

  if ((mem & 0x3b000000) == 0x39000000) {
    unsigned opnb = (size2(mem) << 0x3) | (cBit(mem) << 0x2) | sh0(mem);

    print_mnemonic(un_signed_mnemonics[opnb]);

    if (cBit(mem)) {
      print_selected_register(rd(mem), size2(mem) + 0x2);

      scale = ((sh0(mem) & 0x2) << 0x1) | size2(mem);
    } else {
      if (!sh0(mem)) {
        print_register_zr(rd(mem), is64Bit2(mem));
      } else {
        print_register(rd(mem), is64Bit2(mem));
      }

      scale = size2(mem);
    }

    print_separator();

    print_character('[');

    print_register_sp(rn(mem), 1);

    if (imm12(mem)) {
      print_separator();

      print_unsigned_immediate(imm12(mem) << scale);
    }

    print_character(']');

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_loadstore_register(uint32_t mem) {
  unsigned scale;

  if ((mem & 0x3b200c00) == 0x38200800) {
    unsigned opnb = (size2(mem) << 0x3) | (cBit(mem) << 0x2) | sh0(mem);

    print_mnemonic(un_signed_mnemonics[opnb]);

    if (cBit(mem)) {
      print_selected_register(rd(mem), size2(mem));

      scale = ((sh0(mem) & 0x2) << 0x1) | size2(mem);
    } else {
      if (!sh0(mem)) {
        print_register_zr(rd(mem), is64Bit2(mem));
      } else {
        print_register(rd(mem), is64Bit2(mem));
      }

      scale = size2(mem);
    }

    print_separator();

    print_character('[');

    print_register_sp(rn(mem), 1);

    if (rm(mem) != 0x1f) {
      print_separator();

      print_register(rm(mem), (opt(mem) & 0x3) == 0x3);

      unsigned shift = tBit(mem) ? scale : 0;

      if (opt(mem) == 0x3) {
        if (shift) {
          print_separator();

          print_options("lsl");

          print_shift_value(shift);
        }
      } else {
        print_separator();

        print_options(extend_bit_names[opt(mem)]);

        if (shift) print_unsigned_immediate(shift);
      }
    }

    print_character(']');

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_loadstore_immediate(uint32_t mem) {
  const char *mnemonic = NULL;

  unsigned opnb = (size2(mem) << 0x3) | (cBit(mem) << 0x2) | sh0(mem);

  if ((mem & 0x3b200000) == 0x38000000) {
    if (op3(mem) & 0x1) {
      mnemonic = un_signed_mnemonics[opnb];
    } else if (!op3(mem)) {
      mnemonic = unscaled_mnemonics[opnb];
    } else {
      mnemonic = unprivileged_mnemonics[opnb];
    }

    if (mnemonic == NULL) return 0;

    print_mnemonic(mnemonic);

    if (cBit(mem)) {
      print_register(rd(mem), 1);
    } else if (!sh0(mem)) {
      print_register_zr(rd(mem), is64Bit2(mem));
    } else {
      print_register(rd(mem), is64Bit2(mem));
    }

    print_separator();

    print_character('[');

    print_register_sp(rn(mem), 1);

    switch (op3(mem)) {
      case 0: // unscaled immediate
        if (imm9(mem)) {
          print_separator();
          print_signed_immediate(imm9(mem));
        }

        print_character(']');

        break;
      case 1: // immediate post-indexed
        print_character(']');

        if (imm9(mem)) {
          print_separator();
          print_signed_immediate(imm9(mem));
        }

        break;
      case 2: // unprivileged
        if (imm9(mem)) {
          print_separator();
          print_signed_immediate(imm9(mem));
        }

        print_character(']');

        break;
      case 3: // immediate pre-indexed
        if (imm9(mem)) {
          print_separator();
          print_signed_immediate(imm9(mem));
        }

        print_string("]!");

        break;
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_sysinc(uint32_t mem) {
  if ((mem & 0xfffff01f) == 0xd503301f) {
    if (barrier_mnemonics[op2(mem)] == NULL) return 0;

    print_mnemonic(barrier_mnemonics[op2(mem)]);

    if (op2(mem) & 0x2) {
      if (crM(mem) != 0xf) {
        print_character('#');

        print_unsigned_immediate(crM(mem));
      }
    } else {
      if (barrier_options[crM(mem)] != NULL) {
        print_options(barrier_options[crM(mem)]);
      } else {
        print_unsigned_immediate(crM(mem));
      }
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_msr_immediate(uint32_t mem) {
  char *statefield = NULL;

  if ((mem & 0xfff8f01f) == 0xd500401f) {
    switch (op2(mem)) {
      case 3: statefield = "uao"; break;
      case 4: statefield = "pan"; break;
      case 5: statefield = "spsel"; break;
      case 6: statefield = "daifset"; break;
      case 7: statefield = "daifclr"; break;
      default: return 0;
    }

    print_mnemonic("msr");

    print_options(statefield);

    print_separator();

    print_unsigned_immediate(crM(mem));

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int find_sys_reg(uint32_t mem, uint64_t mask, uint64_t value, char *which) {
  char reg[255];

  if ((mem & mask) == value) {
    print_mnemonic(which);

    sprintf(reg, "s%u_%u_c%u_c%u_%u", op0(mem), op1(mem), crN(mem), crM(mem), op2(mem));

    for (unsigned int a = 0; system_registers_common[a].reg; a++) {
      if (!strcmp(reg, system_registers_common[a].reg)) {
        name = system_registers_common[a].name;

        def = system_registers_common[a].definition;

        break;
      }
    }

    return 1;
  }

  return 0;
}

unsigned int insn_is_at_dc_msr(uint32_t mem) {
  if (find_sys_reg(mem, 0xfff8ff00, 0xd5087800, "at") == 0) {
    if (find_sys_reg(mem, 0xfff8f000, 0xd5087000, "dc") == 0) {
      if (find_sys_reg(mem, 0xfff00000, 0xd5100000, "msr") == 0) {
        return 0;
      }
    }
  }

  if (name != NULL && def != NULL) {
    print_options(name);

    if (rd(mem) != 0x1f) {
      print_separator();

      print_register(rd(mem), 1);
    }

    print_sys_register(name, def);

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_mrs(uint32_t mem) {
  if (find_sys_reg(mem, 0xfff00000, 0xd5300000, "mrs") == 1) {
    if (name == NULL && def == NULL) return 0;

    print_register(rd(mem), 1);

    print_separator();

    print_options(name);

    print_sys_register(name, def);

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_tlbi(uint32_t mem) {
  if (find_sys_reg(mem, 0xfff8f000, 0xd5088000, "tlbi") == 1) {
    if (name == NULL && def == NULL) return 0;

    print_options(name);

    // I need to write the tlbi part here

    print_sys_register(name, def);

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_process(uint32_t mem) {
  if ((mem & 0x5fe00000) == 0x1ac00000) {
    if (aBit(mem)) return 0;

    if (!(imms(mem) & 0x3e)) return 0;

    if (imms(mem) & 0x30) return 0;

    if ((imms(mem) & 0x3c) == 0x4) return 0;

    print_mnemonic(shift_mnemonics[ind2(mem)]);

    print_register_zr(rd(mem), is64Bit(mem));

    print_separator();

    print_register_zr(rn(mem), is64Bit(mem));

    print_separator();

    print_register_zr(rm(mem), is64Bit(mem));

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_mem_related(uint32_t mem) {
  unsigned opnb = ((mem >> 25) & 0x30) | ((mem >> 20) & 0xe) | ((mem >> 15) & 0x1);

  if ((mem & 0x1f000000) == 0x1b000000) {
    if (opc(mem)) return 0;

    if (opnb > 0xC) return 0;

    if (!is64Bit(mem) && opnb > 1) return 0;

    if ((opnb & 0x4) && (ra(mem) != 0x1f)) return 0;

    if (ra(mem) == 0x1f) {
      print_mnemonic(pseudo_mem_mnemonics[opnb & 0xf]);
    } else {
      print_mnemonic(mem_mnemonics[opnb & 0xf]);
    }

    print_register_zr(rd(mem), is64Bit(mem));

    print_separator();

    int bit_registers64 = is64Bit(mem) & !(opnb & 0x2);

    print_register_zr(rn(mem), bit_registers64);

    print_separator();

    print_register_zr(rm(mem), bit_registers64);

    if (ra(mem) != 0x1f) {
      print_separator();

      print_register(ra(mem), is64Bit(mem));
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_bitfield(uint32_t mem) {
  int is_stx = 0;

  if ((mem & 0x1f800000) == 0x13000000) {
    if (opc(mem) == 0x3) return 0;

    if (is64Bit(mem) != vBit(mem)) return 0;

    if (!is64Bit(mem) && ((immr(mem) & 0x20) || (imms(mem) & 0x20))) return 0;

    if (!(opc(mem) & 0x1) && !immr(mem)) {
      if (imms(mem) == 0x7) {
        print_mnemonic(extend_bit_names2[opc(mem)][0]);
        is_stx = 1;
      } else if (imms(mem) == 0xf) {
        print_mnemonic(extend_bit_names2[opc(mem)][1]);
        is_stx = 1;
      } else if (imms(mem) == 0x1f && is64Bit(mem) && !opc(mem)) {
        print_mnemonic(extend_bit_names2[opc(mem)][2]);
        is_stx = 1;
      }

      if (is_stx == 1) {
        print_register(rd(mem), is64Bit(mem));

        print_separator();

        print_register(rn(mem), 0);

        print_end_of_line();

        return 1;
      }
    }

    if (!(opc(mem) & 0x1) && ((imms(mem) & 0x1f) == 0x1f) && (is64Bit(mem) == (imms(mem) >> 0x5))) {
      print_mnemonic(!opc(mem) ? "asr" : "lsr");

      print_register(rd(mem), is64Bit(mem));

      print_separator();

      print_register(rn(mem), is64Bit(mem));

      print_separator();

      print_unsigned_immediate(immr(mem));

      print_end_of_line();

      return 1;
    }

    if (opc(mem) == 0x2 && (imms(mem) + 1) == immr(mem)) {
      print_mnemonic("lsl");

      print_register(rd(mem), is64Bit(mem));

      print_separator();

      print_register(rn(mem), is64Bit(mem));

      print_separator();

      print_unsigned_immediate((is64Bit(mem) ? 0x40 : 0x20) - immr(mem));

      print_end_of_line();

      return 1;
    }

    if (imms(mem) < immr(mem)) {
      if (opc(mem) != 1 || rn(mem) != 0x1f) {
        print_mnemonic(insert_bit_names[opc(mem)]);

        print_register(rd(mem), is64Bit(mem));

        print_separator();

        print_register(rn(mem), is64Bit(mem));

        print_separator();

        print_unsigned_immediate((is64Bit(mem) ? 0x40 : 0x20) - immr(mem));

        print_separator();

        print_unsigned_immediate(imms(mem) + 1);

        print_end_of_line();

        return 1;
      }

      print_mnemonic(bitmove_names[opc(mem)]);

      print_register(rd(mem), is64Bit(mem));

      print_separator();

      print_register(rn(mem), is64Bit(mem));

      print_separator();

      print_unsigned_immediate(immr(mem));

      print_separator();

      print_unsigned_immediate(imms(mem));

      print_end_of_line();

      return 1;
    }

    print_mnemonic(extract_bit_names[opc(mem)]);

    print_register(rd(mem), is64Bit(mem));

    print_separator();

    print_register(rn(mem), is64Bit(mem));

    print_separator();

    print_unsigned_immediate(immr(mem));

    print_separator();

    print_unsigned_immediate(imms(mem) - immr(mem) + 1);

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_float_point_int_conversion(uint32_t mem) {
  if ((mem & 0x5f20fc00) == 0x1e200000) {
    if (aBit(mem)) return 0;

    if (sh0(mem) == 0x3) return 0;

    if (((op0(mem) & 0x1) || (op0(mem) & 0x2)) && (((op1(mem) & 0x6) == 0x2) || ((op1(mem) & 0x6) == 0x4))) return 0;

    if ((sh0(mem) == 0x2) && (!(op1(mem) & 0x4) || ((op1(mem) & 0x6) == 0x4))) return 0;

    if (!sh0(mem) && (op0(mem) & 0x1) && ((op1(mem) & 0x6) == 0x6)) return 0;

    if (is64Bit(mem) && sh0(mem) == 0x2 && ((rm(mem) & 0xe) == 0x6)) return 0;

    if ((rm(mem) & 0x1e) == 0xe) {
      if (!is64Bit(mem) || (sh0(mem) != 0x2)) return 0;

      print_mnemonic(float_point_int_conversion_mnemonics[rm(mem)]);

      if (op1(mem) & 0x1) {
        printf("V%u.D[1]", rd(mem));

        print_separator();

        print_register_zr(rn(mem), 1);
      } else {
        print_register_zr(rd(mem), 1);

        print_separator();

        xprintf("V%u.D[1]", rn(mem));
      }

      return 0;
    }

    print_mnemonic(float_point_int_conversion_mnemonics[rm(mem)]);

    if ((rm(mem) == 0x2) || (rm(mem) == 0x3) || (rm(mem) == 0x7)) {
      print_selected_register(rd(mem), sh0(mem));

      print_separator();

      print_register_zr(rn(mem), is64Bit(mem));
    } else {
      print_register_zr(rd(mem), is64Bit(mem));

      print_separator();

      print_selected_register(rn(mem), sh0(mem));
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_float_point_data_processing(uint32_t mem) {
  if ((mem & 0x5f207c00) == 0x1e204000) {
    if (opd(mem)) return 0;

    if (aBit(mem)) return 0;

    if (op6(mem) > 0x10) return 0;

    print_mnemonic(float_point_data_processing_mnemonics[op6(mem)]);

    if ((op6(mem) >= 0x4) && (op6(mem) <= 0x7)) {
      print_selected_register(rd(mem), (op6(mem) & 0x3) ^ 0x2);

      print_separator();

      print_selected_register(rn(mem), sh0(mem) ^ 0x2);
    } else {
      print_selected_register(rd(mem), sh0(mem));

      print_separator();

      print_selected_register(rn(mem), sh0(mem));
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

// https://raw.githubusercontent.com/Siguza/misc/master/xref.c

unsigned int insn_is_adrp(uint32_t mem) {
  unsigned opnb = ((int64_t)((imm1(mem) << 0x2) | opc(mem)) << 0x2b) >> 0x1f;

  if ((mem & 0x9f000000) == 0x90000000) {
    print_mnemonic("adrp");

    print_register(rd(mem), 1);

    print_separator();

    print_unsigned_immediate64((addr & 0xfffffffffffff) + opnb);

    if (x == 1) {
      print_file_strings(((addr & 0xfffffffffffff) + opnb) - image.base);
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_adr(uint32_t mem) {
  unsigned opnb = ((int64_t)((imm1(mem) << 0x2) | opc(mem)) << 0x2b) >> 0x2b;

  if ((mem & 0x9f000000) == 0x10000000) {
    print_mnemonic("adr");

    print_register(rd(mem), 1);

    print_separator();

    print_unsigned_immediate64(addr + opnb);

    if (x == 1) {
      print_file_strings((addr + opnb) - image.base);
    }

    print_end_of_line();

    return 1;
  }

  return 0;
}

unsigned int insn_is_ldr_litteral(uint32_t mem) {
  bool is_ldrsw = ((mem & 0xff000000) == 0x98000000);
  bool is_64bit = ((mem & 0x40000000) != 0) && !is_ldrsw;

  if ((mem & 0x3f000000) == 0x18000000) {
    print_mnemonic(is_ldrsw ? "ldrsw" : "ldr");

    print_register(rd(mem), is_64bit ? 1 : 0);

    print_separator();

    print_unsigned_immediate64(addr + imm19(mem));

    print_end_of_line();

    return 1;
  }

  return 0;
}
