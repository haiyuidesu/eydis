//
//  Hint.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef HINT_HPP
    #define HINT_HPP
    #include <include/Eydis.hpp>

    // Sorry, I was too lazy to do this one.
    class Hint : virtual public Eydis {
        public:
            Hint(void) : Eydis() {
                this->setImm(this->imm2(_mem));
                this->setMnemonic(this->hint[this->getImm()]);
                this->setValidity(_mem, 0xfffff01f, 0xd503201f);
            };

            void analyzeInsn(str_t type)
            {
                if (this->getImm() > 0x20) return;

                this->printInsnInfos(type);
            }
    };

#endif
