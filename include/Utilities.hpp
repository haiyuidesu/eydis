//
//  utilities.hpp
//  eydis
//
//  Created by haiyuidesu on 04.01.23.
//  Copyright © haiyuidesu. All rights reserved.
//

#ifndef UTILITIES_HPP
    #define UTILITIES_HPP
    #include <array>
    #include <iostream>
    #include <stdint.h>
    #include <string_view>
    #include <include/IEydis.hpp>

    class Utilities {
        public:
            /* ============= extra ============= */
            bool is64Bit(uint32_t opc)  { return (opc & 0x80000000); }

            bool is64Bit2(uint32_t opc) {
                unsigned _opNb = (this->size2(opc) << 0x3) | (this->cBit(opc) << 0x2) | this->sh0(opc);

                return ((_opNb & 0x17) == 0x02) || ((_opNb & 0x1e) == 0x18);
            }

            /* ============= registers ============= */

            unsigned getRd(uint32_t opc) { return opc & 0x1f; }
            unsigned getRn(uint32_t opc) { return (opc >> 5) & 0x1f; }
            unsigned getRa(uint32_t opc) { return (opc >> 10) & 0x1f; }
            unsigned getRm(uint32_t opc) { return (opc >> 16) & 0x1f; }

            /* ============= shift ============= */

            unsigned sh0(uint32_t opc)   { return (opc >> 22) & 0x3; }
            unsigned hw0(uint32_t opc)   { return (opc >> 21) & 0x3; }

            /* ============= size ============= */

            unsigned size(uint32_t opc)  { return opc >> 30; }
            unsigned size2(uint32_t opc) { return (opc >> 30) & 0x3; }

            /* ============= bits extraction ============= */

            unsigned aBit(uint32_t opc)  { return (opc >> 29) & 0x1; }
            unsigned bBit(uint32_t opc)  { return (opc >> 26) & 0x1; }
            unsigned cBit(uint32_t opc)  { return (opc >> 24) & 0x1; }
            unsigned dBit(uint32_t opc)  { return (opc >> 22) & 0x1; }
            unsigned eBit(uint32_t opc)  { return (opc >> 21) & 0x1; }
            unsigned fBit(uint32_t opc)  { return (opc >> 12) & 0x1; }
            unsigned gBit(uint32_t opc)  { return (opc >> 10) & 0x1; }

            unsigned crN(uint32_t opc)   { return (opc >> 12) & 0xf; }
            unsigned CRm(uint32_t opc)   { return (opc >> 8) & 0xf; }

            unsigned op0(uint32_t opc)   { return (opc >> 19) & 0x3; }
            unsigned op1(uint32_t opc)   { return (opc >> 16) & 0x7; }
            unsigned op2(uint32_t opc)   { return (opc >> 5) & 0x7; }
            unsigned op3(uint32_t opc)   { return (opc >> 10) & 0x3; }
            unsigned op5(uint32_t opc)   { return (opc >> 3) & 0x3; }
            unsigned op6(uint32_t opc)   { return (opc >> 15) & 0x3f; }
            unsigned op7(uint32_t opc)   { return (opc >> 21) & 0xf; }
            unsigned opd(uint32_t opc)   { return (opc >> 31) & 0x1; }
            unsigned opr(uint32_t opc)   { return (opc >> 30) & 0x1; }
            unsigned ofm(uint32_t opc)   { return (opc >> 23) & 0x7; }
            unsigned opt(uint32_t opc)   { return (opc >> 13) & 0x7; }
            unsigned opc(uint32_t opc)   { return (opc >> 29) & 0x3; }
            unsigned o2(uint32_t opc)    { return (opc >> 23) & 0x1; }
            unsigned o0(uint32_t opc)    { return (opc >> 15) & 0x1; }

            unsigned immlo(uint32_t opc) { return (opc >> 3) & 0x1ffffc; }

            /* ============= immediates ============= */

            unsigned immr(uint32_t opc)  { return (opc >> 16) & 0x3f; }
            unsigned imms(uint32_t opc)  { return (opc >> 10) & 0x3f; }
            unsigned imm2(uint32_t opc)  { return (int64_t)((opc >> 5) & 0x7f); }
            unsigned imm3(uint32_t opc)  { return (int64_t)((opc >> 10) & 0x7); }
            unsigned imm12(uint32_t opc) { return (int64_t)(opc >> 10) & 0xfff; }
            unsigned imm16(uint32_t opc) { return (int64_t)((opc >> 5) & 0xffff); }
            unsigned imm1(uint32_t opc)  { return (int64_t)((opc >> 5) & 0x7ffff); }
            int64_t  imm6(uint32_t opc)  { return (int64_t)(this->imms(opc) << 26) >> 26; }
            int64_t  imm19(uint32_t opc) { return ((int64_t)this->imm1(opc) << 45) >> 43; }
            int64_t  imm26(uint32_t opc) { return ((int64_t)(opc & 0x3ffffff) << 38) >> 36; }
            int64_t  imm7(uint32_t opc)  { return ((int64_t)((opc >> 15) & 0x7f) << 25) >> 25; }
            int64_t  imm9(uint32_t opc)  { return ((int64_t)((opc >> 12) & 0x1ff) << 23) >> 23; }
            unsigned imm14(uint32_t opc) { return ((int64_t)((opc >> 5) & 0x3fff) << 50) >> 48; }

            /* ============= mnemonics ============= */

            std::array<char, 0x5> regPrefix = { 'b', 'd', 'q', 's', 'h' };

            std::array<str_t, 0x4> mov = { "movn", "", "movz", "movk" };

            std::array<str_t, 0x4> calc = { "add", "adds", "sub", "subs" };

            std::array<str_t, 0x4> condSelect = { "csel", "csinc", "csinv", "csneg" };

            std::array<str_t, 0x8> barrier = { "", "", "clrex", "", "dsb", "dmb", "isb", "" };
    
            std::array<str_t, 0x8> dataProcess = { "rbit", "rev16", "rev32", "rev", "clz", "cls", "", "" };

            std::array<str_t, 0x8> extendBit = { "uxtb", "uxth", "uxtw", "uxtx", "sxtb", "sxth", "sxtw", "sxtx" };

            std::array<str_t, 0x8> bitwise = { "and", "bic", "orr", "orn", "eor", "eon", "ands", "bics" };

            std::array<str_t, 0x8> unconditionalRet = { "br", "blr", "ret", "", "eret", "drps", "", "" };

            std::array<str_t, 0x3> extractBit = { "sbfx", "bfxil", "ubfx" };

            std::array<str_t, 0x3> insertBit = { "sbfiz", "bfi", "ubfiz" };

            std::array<str_t, 0x3> moveBit = { "sbfm", "bfm", "ubfm" };

            std::array<str_t, 0x10> shiftNext = {
                "",  "",  "udiv", "sdiv", "", "",    "", "",
                "lsl", "lsr", "asr",  "ror",  "", "pacga", "", ""
            };

            std::array<str_t, 0x10> barrierOpt = {
                "", "oshld", "oshst", "osh", "", "nshld", "nshst", "nsh",
                "", "ishld", "ishst", "ish", "", "ld",    "st",    "sy"
            };

            std::array<str_t, 0x8> msrReg = {
                "", "", "", "uao", "pan", "spsel", "daifset", "daifclr"
            };

            std::array<str_t, 0x10> floatPointProcess = {
                "fmov",   "fabs",   "fneg",   "fsqrt",  "fcvt",  "fcvt",  "",     "fcvt",
                "frintn", "frintp", "frintm", "frintz", "frinta", "",   "frintx", "frinti"
            };

            std::array<str_t, 0x20> uLoadStore = {
                "strb", "ldrb", "ldrsb", "ldrsb", "str", "ldr", "str", "ldr",
                "strh", "ldrh", "ldrsh", "ldrsh", "str", "ldr", "", "",
                "str", "ldr", "ldrsw", "", "str", "ldr", "", "",
                "str", "ldr", "", "", "str", "ldr", "", ""
            };

            std::array<str_t, 0x10> unmodifiedImm = {
                "addw",  "",    "movw", "",  "",   "subw",   "movt", "",
                "ssat", "ssat16", "sbfx", "bfi", "usat", "usat16", "ubfx", ""
            };

            std::array<str_t, 0x10> calcMultiply = {
                "madd", "msub", "smaddl", "smsubl", "smulh", "", "",
                "", "", "", "umaddl", "umsubl", "umulh", "", "", ""
            };

            std::array<str_t, 0x10> calcMultiplyNext = {
                "mul", "mneg", "smull", "smnegl", "smulh", "", "", "",
                "",   "",  "umull", "umnegl", "umulh", "", "", ""
            };

            std::array<str_t, 0x10> bCondition = {
                "b.eq", "b.ne", "b.hs", "b.cc", "b.mi", "b.pl", "b.vs", "b.vc",
                "b.hi", "b.cs", "b.ge", "b.lt", "b.gt", "b.le", "b.al", "b.nv"
            };

            std::array<str_t, 0x20> hint = {
                "nop", "yield", "wfe", "wfi", "sev", "sevl", "", "xpaclri",
                "pacia1716", "",  "pacib1716", "", "autia1716", "", "autib1716",
                "", "",  "",  "", "", "", "",  "", "", "paciaz",
                "paciasp", "pacibz", "pacibsp",   "autiaz",  "autiasp",   "autibz", "autibsp"
            };

            std::array<str_t, 0x12> pacAuth = {
                "pacia",  "pacib",  "pacda",  "pacdb",  "autia",  "autib",  "autda",  "autdb",
                "paciza", "pacizb", "pacdza", "pacdzb", "autiza", "autizb", "autdza", "autdzb",
                "xpaci",  "xpacd"
            };

            std::array<str_t, 0x14> pacAuthUnconditional = {
                "braaz", "brabz", "blraaz", "blrabz", "retaa", "retab",
                "",    "",    "eretaa", "eretab", "",     "",    "",
                "",    "",    "",     "braa",   "brab",   "blraa",  "blrab"
            };

            std::array<str_t, 0x40> atomicLoad = {
                "ldaddb",   "ldaddlb", "ldaddab",  "ldaddalb", "ldaddh",   "ldaddlh",  "ldaddah",
                "ldaddalh", "ldadd",   "ldaddl",   "ldadda",   "ldaddal",  "ldadd",    "ldaddl",
                "ldadda",   "ldaddal", "ldclrb",   "ldclrlb",  "ldclrab",  "ldclralb", "ldclrh",
                "ldclrlh",  "ldclrah", "ldclralh", "ldclr",    "ldclrl",   "ldclra",   "ldclral",
                "ldclr",    "ldclrl",  "ldclra",   "ldclral",  "ldeorb",   "ldeorlb",  "ldeorab", 
                "ldeoralb", "ldeorh",  "ldeorlh",  "ldeorah",  "ldeoralh", "ldeor",    "ldeorl",
                "ldeora",   "ldeoral", "ldeor",    "ldeorl",   "ldeora",   "ldeoral",  "ldsetb",
                "ldsetlb",  "ldsetab", "ldsetalb", "ldseth",   "ldsetlh",  "ldsetah",  "ldsetalh",
                "ldset",    "ldsetl",  "ldseta",   "ldsetal",  "ldset",    "ldsetl",   "ldseta", "ldsetal"
            };

            std::array<str_t, 0x10> atomicSwap = {
                "swpb",  "swplb", "swpab",  "swpalb", "swph",
                "swplh", "swpah", "swpalh", "swp",    "swpl",
                "swpa",  "swpal", "swp",    "swpl",   "swpa", "swpal"
            };
    };

#endif
