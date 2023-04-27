//
//  BL.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef BL_HPP
    #define BL_HPP
    #include "../Eydis.hpp"

    //  31  30       26 25                                                   0
    // +---+-----------+-----------------------------------------------------+
    // | 0 | 0 0 1 0 1 |                        imm26                        | B
    // | 1 | 0 0 1 0 1 |                        imm26                        | BL
    // +---+-----------+-----------------------------------------------------+
    //  op
    class BL : virtual public Eydis {
        public:
            BL(void) : Eydis() {
                this->setValidity(_mem, 0x7c000000, 0x14000000);
                this->setMnemonic(this->opd(_mem) ? "bl" : "b");
                this->setRet(this->opd(_mem) ? 1 : 0);
                this->setImm(this->imm26(_mem));
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                std::cout << "\033[38;5;227m" << ((this->getRet() == 1) ? "sub_" : "loc_") << _addr + this->getImm() << "\033[30;0m";
            }
    };

#endif
