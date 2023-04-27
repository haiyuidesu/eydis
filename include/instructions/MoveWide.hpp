//
//  MoveWide.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef MOVEWIDE_HPP
    #define MOVEWIDE_HPP
    #include "../Eydis.hpp"

    //  31   30 29 28         23 22 21 20                              5 4        0
    // +----+-----+-------------+------+---------------------------------+---------+
    // | sf | 1 1 | 1 0 0 1 0 1 |  hw  |              imm16              |    Rd   | MOVK
    // | sf | 0 0 | 1 0 0 1 0 1 |  hw  |              imm16              |    Rd   | MOVN
    // | sf | 1 0 | 1 0 0 1 0 1 |  hw  |              imm16              |    Rd   | MOVZ
    // +----+-----+-------------+------+---------------------------------+---------+
    //        opc
    class MoveWide : virtual public Eydis {
        private:
            unsigned _op = 0x0;

        public:
            MoveWide(void) : Eydis() {
                this->setImm(this->imm16(_mem));
                this->setValidity(_mem, 0x1f800000, 0x12800000);
                this->_op = (!this->opc(_mem) && (!this->imm16(_mem) || !this->hw0(_mem))
                                    && (this->is64Bit(_mem) || (this->imm16(_mem) != 0xffff)));

                this->setMnemonic((this->_op ? "mov" : this->mov[this->opc(_mem)]));

                if ((this->getMnemonic() == "") || (this->opc(_mem) == 1) || ((!this->is64Bit(_mem) && this->hw0(_mem) >= 0x2))) {
                    this->_isValid = 0;
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, true);

                if (this->_op) {
                    if (this->is64Bit(_mem)) {
                        this->printImmediate(~((int64_t)(this->getImm() << (this->hw0(_mem) * 0x10))), false);
                    } else {
                        this->printImmediate(~((int32_t)(this->getImm() << (this->hw0(_mem) * 0x10))), false);
                    }
                } else {
                    this->printImmediate(this->getImm(), ((this->hw0(_mem) ? true : false)));

                    if (this->hw0(_mem)) {
                        this->printShift(0x0, false);
                        this->printShift(this->hw0(_mem) * 0x10, true);
                    }
                }
            }
    };

#endif
