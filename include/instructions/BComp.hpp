//
//  BComp.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef BCOMP_HPP
    #define BCOMP_HPP
    #include "../Eydis.hpp"

    //  31   30         25 24  23                                    5 4          0
    // +----+-------------+---+---------------------------------------+----------+
    // | sf | 0 1 1 0 1 0 | 0 |                 imm19                 |    Rt    | CBZ
    // | sf | 0 1 1 0 1 0 | 1 |                 imm19                 |    Rt    | CBNZ
    // +----+-------------+---+---------------------------------------+----------+
    //                     op
    class BComp : virtual public Eydis {
        public:
            BComp(void) : Eydis() {
                this->setMnemonic(this->cBit(_mem) ? "cbnz" : "cbz");
                this->setValidity(_mem, 0x7e000000, 0x34000000);
                this->setImm(this->imm19(_mem));
                this->setRet(0);
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, true);

                std::cout << "\033[38;5;227mloc_" << _addr + this->getImm() << "\033[30;0m";
            }
    };

#endif
