//
//  Multiply.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef MULTIPLY_HPP
    #define MULTIPLY_HPP
    #include "../Eydis.hpp"

    //  Multiply instructions.
    class Multiply : virtual public Eydis {
        private:
            unsigned _op = 0x0;
            unsigned _isReg64 = 0x0;

        public:
            Multiply(void) : Eydis() {
                this->setValidity(_mem, 0x1f000000, 0x1b000000);
                this->_isReg64 = (this->is64Bit(_mem) & !(this->_op & 0x2));
                this->_op = ((_mem >> 25) & 0x30) | ((_mem >> 20) & 0xe) | ((_mem >> 15) & 0x1);

                this->setMnemonic(((this->getRa(_mem) == 0x1f)
                                    ? this->calcMultiplyNext[this->_op & 0xf]
                                    : this->calcMultiply[this->_op & 0xf]));

                this->_isValid = (((this->getMnemonic() == "") || (this->opc(_mem)) || (this->_op > 0xC)
                                    || (!this->is64Bit(_mem)) || ((this->_op & 0x4) && (this->getRa(_mem) != 0x1f)))
                                    ? 0 : this->getValidity());
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), Zero, true);

                this->printRegister(this->getRn(_mem), this->_isReg64, Zero, true);

                this->printRegister(this->getRm(_mem), this->_isReg64, Zero, (this->getRa(_mem) == 0x1f) ? false : true);

                if (this->getRa(_mem)) this->printRegister(this->getRa(_mem), this->is64Bit(_mem), None, false);
            }
    };

#endif
