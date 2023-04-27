//
//  MemBarrier.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef MEMBARRIER_HPP
    #define MEMBARRIER_HPP
    #include "../Eydis.hpp"

    // Memory barrier instructions
    class memBarrier : virtual public Eydis {
        public:
            memBarrier(void) : Eydis() {
                this->setValidity(_mem, 0xfffff01f, 0xd503301f);
                this->setMnemonic(this->barrier[this->op2(_mem)]);
                this->_isValid = ((this->getMnemonic() == "") ? 0 : this->getValidity());
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (this->op2(_mem) & 0x2) {
                    if (this->CRm(_mem) != 0xf) {
                        std::cout << "#";
                        this->printImmediate(this->CRm(_mem), false);
                    }
                } else {
                    if (this->barrierOpt[this->CRm(_mem)] != "") {
                        this->printOption(this->barrierOpt[this->CRm(_mem)], false);
                    } else {
                        this->printImmediate(this->CRm(_mem), false);
                    }
                }
            }
    };

    //  31                       19 18      16 15     12 11   10    8 7       5 4        0
    // +---------------------------+----------+---------+-------------+---------+-----------+
    // | 1 1 0 1 0 1 0 1 0 0 0 0 0 |    op1   | 0 1 0 0 |     CRm     |   op2   | 1 1 1 1 1 | MSR immediate
    // +---------------------------+----------+---------+-------------+---------+-----------+
    class MSRImm : virtual public Eydis {
        public:
            MSRImm(void) : Eydis() {
                this->setMnemonic("msr");
                this->setValidity(_mem, 0xfff8f01f, 0xd500401f);
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printOption(this->msrReg[this->op2(_mem)], true);

                this->printImmediate(this->CRm(_mem), false);
            }
    };

#endif
