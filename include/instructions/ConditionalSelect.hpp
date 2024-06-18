//
//  ConditionalSelect.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef CONDITIONALSELECT_HPP
    #define CONDITIONALSELECT_HPP
    #include <include/Eydis.hpp>

    //  Conditional Select registers: these instructions can be used as an alternative to branches.
    class ConditionalSelect : virtual public Eydis {
        private:
            unsigned _opNb = 0x0;
            unsigned _condSet = 0x0;
    
        public:
            ConditionalSelect(void) : Eydis() {
                this->setValidity(_mem, 0x1fe00000, 0x1a800000);
                this->_opNb = ((this->opr(_mem) << 0x1) | ((this->op3(_mem) & 0x1)));
                this->_condSet = (this->getRn(_mem) == this->getRm(_mem)) && ((this->_opNb == 0x1) || (this->_opNb == 0x2));

                this->setMnemonic((this->_condSet ?
                        (((this->getRn(_mem) == 0x1f)
                                    ? ((this->_opNb == 1) ? "cset" : "csetm")
                                    : ((this->_opNb == 1) ? "cinc" : "cinv")))
                        : ((this->_opNb > 0x4) ? "" : this->condSelect[this->_opNb])));
                
                if ((this->getMnemonic() == "") || (this->aBit(_mem)) || (this->op3(_mem) & 0x2)) {
                    this->_isValid = 0;
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, true);

                if (this->getRn(_mem) != 0x1f) this->printRegister(this->getRn(_mem), this->is64Bit(_mem), Zero, true);

                if ((this->getRn(_mem) != 0x1f) && (this->_condSet)) {
                    this->printOption(this->condSelect[this->crN(_mem) ^ 0x1], false); // 31th register
                } else {
                    this->printRegister(this->getRm(_mem), this->is64Bit(_mem), Zero, true);
                    this->printOption(this->bCondition[this->crN(_mem)].data() + 2, false);
                }
            }
    };

#endif
