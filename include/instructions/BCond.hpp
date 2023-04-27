//
//  BCond.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef BCOND_HPP
    #define BCOND_HPP
    #include "../Eydis.hpp"

    //  31             24 23                                       4  3         0
    // +-----------------+---------------------------------------+---+----------+
    // | 0 1 0 1 0 1 0 0 |                   imm19               | 0 |   cond   | B.cond
    // +-----------------+---------------------------------------+---+----------+
    class BCond : virtual public Eydis {
        public:
            BCond(void) : Eydis() {
                this->setMnemonic(this->bCondition[_mem & 0xf]);
                this->setValidity(_mem, 0xff000010, 0x54000000);
                this->setImm(this->imm19(_mem));
                this->setRet(0);
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                std::cout << "\033[38;5;227mloc_" << _addr + this->getImm() << "\033[30;0m";
            }
    };

#endif
