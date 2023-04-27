//
//  Calc.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef CALC_HPP
    #define CALC_HPP
    #include "../Eydis.hpp"

    //  31   30  29  28       24 23   22 21                     10 9         5 4      0
    // +----+---+---+-----------+-------+-------------------------+-----------+-------+
    // | sf | 0 | 0 | 1 0 0 0 1 | shift |          imm12          |    Rn     |   Rd  | ADD  imm
    // | sf | 0 | 1 | 1 0 0 0 1 | shift |          imm12          |    Rn     |   Rd  | ADDS imm
    // | sf | 1 | 0 | 1 0 0 0 1 | shift |          imm12          |    Rn     |   Rd  | SUB  imm
    // | sf | 1 | 1 | 1 0 0 0 1 | shift |          imm12          |    Rn     |   Rd  | SUBS imm
    // +----+---+---+-----------+-------+-------------------------+-----------+-------+
    //       op   S
    class CalcImm : virtual public Eydis {
        private:
            bool _isMov = 0x0;
            bool _isCmp = 0x0;

        public:
            CalcImm(void) : Eydis() {
                this->setImm(this->imm12(_mem));
                this->setValidity(_mem, 0x1f000000, 0x11000000);

                this->_isCmp = (this->aBit(_mem)) && (this->getRd(_mem) == 0x1f);
                this->_isMov = (!this->opc(_mem) && !this->imm12(_mem)
                                && ((this->getRd(_mem) == 0x1f) || this->getRn(_mem) == 0x1f));

                if (this->_isCmp) {
                    this->setMnemonic(this->opr(_mem) ? "cmp" : "cmn");
                } else {
                    this->setMnemonic((this->_isMov ? "mov" : this->calc[this->opc(_mem)]));
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (!this->_isCmp) this->printRegister(this->getRd(_mem), this->is64Bit(_mem), SPtr, true);

                this->printRegister(this->getRn(_mem), this->is64Bit(_mem), SPtr, ((!this->_isMov) ? true : false));

                if (!this->_isMov) {
                    this->printImmediate(this->getImm(), (this->sh0(_mem) ? true : false));

                    if (this->sh0(_mem)) {
                        this->printShift(this->sh0(_mem), false);
                        this->printShift(this->hw0(_mem) * 0x6, true);
                    }
                }
            }
    };

    class CalcExtendRegister : virtual public Eydis {
        private:
            bool _isCmp = 0x0;

        public:
            CalcExtendRegister(void) : Eydis() {
                this->setImm(this->imm3(_mem));
                this->setValidity(_mem, 0x1fe00000, 0x0b200000);

                this->_isCmp = (this->aBit(_mem) && this->getRd(_mem) == 1);

                if (this->_isCmp) {
                    this->setMnemonic(this->opr(_mem) ? "cmp" : "cmn");
                } else {
                     this->setMnemonic((this->getImm() > 0x4) ? "" : calc[this->opc(_mem)]);
                }
            }

            void analyzeInsn(str_t type)
            {
                if (this->getMnemonic() == "") return;

                this->printInsnInfos(type);

                if (!this->_isCmp) {
                    this->printRegister(this->getRd(_mem), this->is64Bit(_mem), SPtr, true);
                }

                this->printRegister(this->getRn(_mem), this->is64Bit(_mem), SPtr, true);

                this->printRegister(this->getRm(_mem), this->is64Bit(_mem) && ((this->opt(_mem) & 0x3) == 0x3), Zero, true);

                if (this->opt(_mem) == 0x2 && ((this->getRd(_mem) == 0x1f) || (this->getRn(_mem) == 0x1f))) {
                    this->printOption("lsl", false);
                } else {
                    this->printOption(extendBit[this->opt(_mem)], false);
                }

                if (this->getImm()) {
                    this->printImmediate(this->getImm(), false);
                }
            }
    };

    class CalcShiftRegister : virtual public Eydis {
        private:
            bool _isCmp = 0x0;
            bool _isCmp2 = 0x0;

        public:
            CalcShiftRegister(void) : Eydis() {
                this->setImm(this->imm6(_mem));
                this->setValidity(_mem, 0x1f200000, 0x0b000000);

                this->_isCmp = (this->aBit(_mem) && this->getRd(_mem) == 0x1f);
                this->_isCmp2 = (this->opr(_mem) && this->getRn(_mem) == 0x1f);

                if (this->_isCmp || this->_isCmp2) {
                    this->setMnemonic(this->opr(_mem) ? "cmp" : "cmn");
                } else {
                     this->setMnemonic((this->getImm() > 0x4) ? "" : calc[this->opc(_mem)]);
                }
            }

            void analyzeInsn(str_t type)
            {
                if ((this->getMnemonic() == "") || (!this->is64Bit(_mem) && this->imm6(_mem) & 0x20) || (this->sh0(_mem) == 0x3)) return;

                this->printInsnInfos(type);

                if (!this->_isCmp) {
                    this->printRegister(this->getRd(_mem), this->is64Bit(_mem), SPtr, true);
                }

                if (!this->_isCmp2) {
                    this->printRegister(this->getRn(_mem), this->is64Bit(_mem), None, true);
                }

                this->printRegister(this->getRm(_mem), this->is64Bit(_mem), Zero, ((this->getImm()) ? true : false));

                if (this->getImm()) {
                    this->printShift(this->sh0(_mem), false);
                    this->printShift(this->getImm(), true);
                }
            }
    };

#endif
