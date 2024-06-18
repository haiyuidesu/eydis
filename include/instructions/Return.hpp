//
//  Return.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef RETURN_HPP
    #define RETURN_HPP
    #include <include/Eydis.hpp>

    //  31           25  24  23  22  21                12  11  10  9     5  4   3   0
    // +---------------+---+---+---+---------------------+---+---+--------+---+-------+
    // | 1 1 0 1 0 1 1 | Z | 0 | 0 | 1 1 1 1 1 1 0 0 0 0 | 1 | M |   Rn   |     Rm    | BLRAA, BLRAAZ, BLRAB, BLRABZ
    // | 1 1 0 1 0 1 1 | Z | 0 | 0 | 0 1 1 1 1 1 0 0 0 0 | 1 | M |   Rn   |     Rm    | BRAA, BRAAZ, BRAB, BRABZ
    // |---------------+---+---+---+---------------------+---+---+--------+---+-------|
    // | 1 1 0 1 0 1 1 | 0 | 0 | 1 | 0 1 1 1 1 1 0 0 0 0 | 0 | 0 |   Rn   | 0 | 0 0 0 | RET
    // | 1 1 0 1 0 1 1 | 0 | 0 | 0 | 1 1 1 1 1 1 0 0 0 0 | 0 | 0 |   Rn   | 0 | 0 0 0 | BLR
    // | 1 1 0 1 0 1 1 | 0 | 0 | 0 | 0 1 1 1 1 1 0 0 0 0 | 0 | 0 |   Rn   | 0 | 0 0 0 | BR
    // +---------------+---+---+---+---------------------+---+---+--------+---+-------+
    //                   Z       op                        A   M            Rm
    class Return : virtual public Eydis {
        private:
            bool _isPac = 0x0;
            unsigned _authValue = 0x0;

        public:
            Return(void) : Eydis() {
                this->_isPac = (this->getRm(_mem) == 0x1f && (this->imms(_mem) & 0x3e) == 0x2);
                this->_authValue = (this->op7(_mem) << 0x1) | this->gBit(_mem);

                this->setValidity(_mem, 0xfe1f0000, 0xd61f0000);

                this->setMnemonic((this->_isPac)
                                ? this->pacAuthUnconditional[this->_authValue]
                                : this->unconditionalRet[this->op7(_mem)]);


                this->_isValid = (!this->_isPac && (this->op7(_mem) == 0x3 || this->op7(_mem) > 0x5))
                                || (!this->_isPac && ((this->op7(_mem) & 0xe) == 0x4) && this->getRn(_mem) != 0x1f)
                                || ((this->_isPac) && ((this->getRn(_mem) != 0x1f) && (this->op7(_mem) == 0x2 || this->op7(_mem) == 0x4)))
                                || ((this->_isPac) && (this->_authValue >= 20))
                                || (this->getMnemonic() == "")
                                ? 0 : this->getValidity(); // at this point, I need help.
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (this->getMnemonic().find("ret") != std::string::npos) return;
    
                if (((this->_isPac) && ((this->op7(_mem) & 0x7) <= 0x1)) || (this->op7(_mem) <= 0x2)) {
                    this->printRegister(this->getRn(_mem), true, None, (((this->_isPac) && ((this->op7(_mem) & 0x8)) ? true : false)));
                }

                if ((this->_isPac) && ((this->op7(_mem) & 0x8))) {
                    this->printRegister(this->getRm(_mem), true, None, false);
                }
            }
    };

#endif
