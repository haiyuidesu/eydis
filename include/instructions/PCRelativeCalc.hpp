//
//  PCRelativeCalc.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef PCRELATIVECALC_HPP
    #define PCRELATIVECALC_HPP
    #include <include/Eydis.hpp>

    //   31 30      29 28       24 23                              5 4       0
    // +---+----------+-----------+---------------------------------+----------+
    // | 0 |   immlo  | 1 0 0 0 0 |                immhi            |    Rd    | ADR
    // | 1 |   immlo  | 1 0 0 0 0 |                immhi            |    Rd    | ADRP
    // +---+----------+-----------+---------------------------------+----------+
    //  op
    class PCRelativeCalc : virtual public Eydis {
        private:
            bool _isAdrp = 0x0;
            unsigned _op = 0x0;

        public:
            PCRelativeCalc(void) : Eydis() {
                this->setValidity(_mem, 0x1f000000, 0x10000000);
                _isAdrp = (((_mem & 0x9f000000) == 0x90000000) ? true : false);
                this->setMnemonic((this->_isAdrp == true) ? "adrp" : "adr");
                _op = ((int64_t)((this->imm1(_mem) << 0x2) | this->opc(_mem)) << 0x2b) >> ((this->_isAdrp == true) ? 0x1f : 0x2b);
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
                this->printRegister(this->getRd(_mem), true, None, true);
                this->printImmediate(((this->_isAdrp == true) ? (_addr & 0xfffffffffffff) : _addr) + this->_op, false);
            }
    };

#endif
