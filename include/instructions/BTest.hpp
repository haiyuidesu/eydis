//
//  BTest.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef BTEST_HPP
    #define BTEST_HPP
    #include <include/Eydis.hpp>

    //  31   30         25 24  23                                    5 4          0
    // +----+-------------+---+---------------------------------------+-----------+
    // | b5 | 0 1 1 0 1 1 | 0 |                 imm14                 |    Rt     | TBZ
    // | b5 | 0 1 1 0 1 1 | 1 |                 imm14                 |    Rt     | TBNZ
    // +----+-------------+---+---------------------------------------+-----------+
    //                     op
    class BTest : virtual public Eydis {
        public:
            BTest(void) : Eydis() {
                this->setMnemonic(this->cBit(_mem) ? "tbnz" : "tbz");
                this->setValidity(_mem, 0x7e000000, 0x36000000);
                this->setImm(this->imm14(_mem));
                this->setRet(0);
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, true);

                this->printImmediate(((_mem >> 0x1a) & 0x20) | ((_mem >> 0x13) & 0x1f), true);

                std::cout << "\033[38;5;227mloc_" << _addr + this->getImm() << "\033[30;0m";
            }
    };

#endif
