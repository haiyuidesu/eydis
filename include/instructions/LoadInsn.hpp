//
//  LoadInsn.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef LOADINSN_HPP
    #define LOADINSN_HPP
    #include "../Eydis.hpp"

    //   31 30             23 22                  15 14        10 9       5 4          0
    // +---+-----------------+---+------------------+-----------+----------+----------+
    // | x | 0 1 0 1 0 0 0 1 | 0 |       imm7       |    Rt2    |    Rn    |    Rt    | STP
    // | x | 0 1 0 1 0 0 0 0 | 0 |       imm7       |    Rt2    |    Rn    |    Rt    | STNP
    // | x | 0 1 0 1 0 0 0 1 | 1 |       imm7       |    Rt2    |    Rn    |    Rt    | LDP
    // | x | 0 1 0 1 0 0 0 0 | 1 |       imm7       |    Rt2    |    Rn    |    Rt    | LDNP
    // | x | 1 1 0 1 0 0 0 0 | 1 |       imm7       |    Rt2    |    Rn    |    Rt    | LDPSW
    // +---+-----------------+---+------------------+-----------+----------+----------+
    //  opc                    L
    class LoadStorePairRegister : virtual public Eydis {
        private:
            int offset = 0x0;
            unsigned int shifted = 0x0;

        public:
            LoadStorePairRegister(void) : Eydis() {
                this->setImm(this->imm7(_mem));

                this->setValidity(_mem, 0x3a000000, 0x28000000);

                if (!this->bBit(_mem) && this->dBit(_mem) && (this->size2(_mem) == 0x1)) {
                    this->setMnemonic("ldpsw");
                } else if (!this->bBit(_mem) && this->dBit(_mem) && !this->cBit(_mem) && !this->o2(_mem)) {
                    this->setMnemonic("ldnp");
                } else if (!this->dBit(_mem) && !this->cBit(_mem) && !o2(_mem)) {
                    this->setMnemonic("stnp");
                } else if (this->dBit(_mem)) {
                    this->setMnemonic("ldp");
                } else {
                    this->setMnemonic("stp");
                }
            }

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (this->bBit(_mem)) {
                    this->printRegisterPrefix(this->getRd(_mem), this->size(_mem), true);
                    this->printRegisterPrefix(this->getRa(_mem), this->size(_mem), true);
                    this->shifted = this->size(_mem) + 0x2;
                } else {
                    this->printRegister(this->getRd(_mem), this->is64Bit(_mem), (!this->dBit(_mem)) ? Zero : None, true);
                    this->printRegister(this->getRa(_mem), this->is64Bit(_mem), (!this->dBit(_mem)) ? Zero : None, true);
                    this->shifted = (this->size2(_mem) >> 0x1) + 0x2;
                }

                std::cout << "[";
                
                this->printRegister(this->getRn(_mem), 0x1, SPtr, false);

                this->offset = this->getImm() << this->shifted;

                if (this->offset == 0x0) {
                    std::cout << "]";
                } else if (this->ofm(_mem) == 0x1) {
                    std::cout << "] ";

                    this->printImmediate(this->offset, false);
                } else {
                    std::cout << " ";

                    this->printImmediate(this->offset, false);

                    std::cout << "]" << (this->ofm(_mem) ? "!" : "");
                }
            }
    };

    //  31 30 29   27 26  25 24 23                                    5 4          0
    // +-----+-------+---+-----+---------------------------------------+-----------+
    // | 0 x | 0 1 1 | 0 | 0 0 |                 imm19                 |    Rt     | LDR   literal
    // | 1 0 | 0 1 1 | 0 | 0 0 |                 imm19                 |    Rt     | LDRSW literal
    // +-----+-------+---+-----+---------------------------------------+-----------+
    //   opc
    class LDRLiteral : virtual public Eydis {
        private:
            bool _isLdrsw = 0x0;

        public:
            LDRLiteral(void) : Eydis() {
                this->setValidity(_mem, 0x3f000000, 0x18000000);
                this->_isLdrsw = ((_mem & 0xff000000) == 0x98000000);
                this->setMnemonic(this->_isLdrsw ? "ldrsw" : "ldr");
                this->setImm(this->imm19(_mem));
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printRegister(this->getRd(_mem), ((((_mem & 0x40000000) != 0) && !this->_isLdrsw) ? 1 : 0), None, true);

                this->printImmediate(_addr + this->getImm(), false);
            }
    };

    class LoadStoreUnsignedImm : virtual public Eydis {
        private:
            unsigned int opnb = 0x0;
            unsigned int scale = 0x0;

        public:
            LoadStoreUnsignedImm(void) : Eydis() {
                this->setImm(this->imm12(_mem));
                this->setValidity(_mem, 0x3b000000, 0x39000000);
                this->opnb = (this->size2(_mem) << 0x3) | (this->bBit(_mem) << 0x2) | this->sh0(_mem);
                this->setMnemonic(this->uLoadStore[this->opnb]);
            }

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (cBit(_mem)) {
                    this->printRegisterPrefix(this->getRd(_mem), this->size2(_mem) + 0x2, true);
                    this->scale = ((this->sh0(_mem) & 0x2) << 0x1) | this->size2(_mem);
                } else {
                    this->printRegister(this->getRd(_mem), this->is64Bit2(_mem), (!this->sh0(_mem)) ? Zero : None, true);
                    this->scale = this->size2(_mem);
                }

                std::cout << "[";

                this->printRegister(this->getRn(_mem), 0x1, SPtr, this->getImm() ? true : false);

                if (this->getImm()) {
                    this->printImmediate(this->getImm() << scale, false);
                }

                std::cout << "]";
            }
    };

    class LoadStoreRegister : virtual public Eydis {
        private:
            unsigned int opnb = 0x0;
            unsigned int shift = 0x0;
            unsigned int scale = 0x0;

        public:
            LoadStoreRegister(void) : Eydis() {
                this->setValidity(_mem, 0x3b200c00, 0x38200800);
                this->opnb = (this->size2(_mem) << 0x3) | (this->bBit(_mem) << 0x2) | this->sh0(_mem);
                this->setMnemonic(this->uLoadStore[this->opnb]);
            }

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (cBit(_mem)) {
                    this->printRegisterPrefix(this->getRd(_mem), this->size2(_mem) + 0x2, true);
                    this->scale = ((this->sh0(_mem) & 0x2) << 0x1) | this->size2(_mem);
                } else {
                    this->printRegister(this->getRd(_mem), this->is64Bit2(_mem), (!this->sh0(_mem)) ? Zero : None, true);
                    this->scale = this->size2(_mem);
                }

                std::cout << "[";

                this->printRegister(this->getRn(_mem), 0x1, SPtr, (this->getRm(_mem) != 0x1f) ? true : false);

                if (this->getRm(_mem) != 0x1f) {
                    this->shift = this->fBit(_mem) ? this->scale : 0;

                    this->printRegister(this->getRm(_mem), (this->opt(_mem) == 0x3), None, true);

                    if ((this->opt(_mem) == 0x3) && this->shift) {
                        this->printOption("lsl", false);
                        this->printShift(this->shift, true);
                    } else {
                        this->printOption(this->extendBit[this->opt(_mem)], false);
                        this->printImmediate(shift, false);
                    }
                }

                std::cout << "]";
            }
    };

#endif
