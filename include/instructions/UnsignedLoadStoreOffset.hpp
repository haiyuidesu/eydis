//
//  UnsignedLoadStoreOffset.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef UNSIGNEDLOADSTOREOFFSET_HPP
    #define UNSIGNEDLOADSTOREOFFSET_HPP
    #include "../Eydis.hpp"

    //  31 30 29   27 26  25 24 23 22 21                     10 9         5 4         0
    // +-----+-------+---+-----+-----+-------------------------+-----------+----------+
    // | 1 x | 1 1 1 | 0 | 0 1 | 0 1 |          imm12          |    Rn     |    Rt    | LDR   unsigned offset
    // | 1 x | 1 1 1 | 0 | 0 1 | 0 0 |          imm12          |    Rn     |    Rt    | STR   unsigned offset
    // | 0 0 | 1 1 1 | 0 | 0 1 | 0 0 |          imm12          |    Rn     |    Rt    | STRB  unsigned offset
    // | 0 1 | 1 1 1 | 0 | 0 1 | 0 0 |          imm12          |    Rn     |    Rt    | STRH  unsigned offset
    // | 0 0 | 1 1 1 | 0 | 0 1 | 0 1 |          imm12          |    Rn     |    Rt    | LDRB  unsigned offset
    // | 0 1 | 1 1 1 | 0 | 0 1 | 0 1 |          imm12          |    Rn     |    Rt    | LDRH  unsigned offset
    // | 0 0 | 1 1 1 | 0 | 0 1 | 1 x |          imm12          |    Rn     |    Rt    | LDRSB unsigned offset
    // | 0 1 | 1 1 1 | 0 | 0 1 | 1 x |          imm12          |    Rn     |    Rt    | LDRSH unsigned offset
    // | 1 0 | 1 1 1 | 0 | 0 1 | 1 0 |          imm12          |    Rn     |    Rt    | LDRSW unsigned offset
    // +-----+-------+---+-----+-----+-------------------------+-----------+----------+
    //  size                     opc
    class UnsignedLoadStoreOffset : virtual public Eydis {
        private:
            unsigned _opnb = 0x0;
            unsigned _scale = 0x0;

        public:
            UnsignedLoadStoreOffset(void) : Eydis() {
                _opnb = (this->size2(_mem) << 0x3) | (this->cBit(_mem) << 0x2) | this->sh0(_mem);
                _scale = ((this->bBit(_mem) ? ((this->sh0(_mem) & 0x2) << 0x1) | this->size2(_mem)
                                            : this->size2(_mem)));

                this->setImm(this->imm12(_mem));
                this->setValidity(_mem, 0x3b000000, 0x39000000);
                this->setMnemonic(this->uLoadStore[this->_opnb]);
                this->_isValid = ((this->getMnemonic() == "") ? 0 : this->getValidity());
            }

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (this->bBit(_mem)) {
                    this->printRegister(this->getRd(_mem), this->size2(_mem), None, true);
                } else {
                    this->printRegister(this->getRd(_mem), this->is64Bit2(_mem), (!this->sh0(_mem) ? Zero : None), true);
                }

                std::cout << "[";

                this->printRegister(getRn(_mem), true, SPtr, (this->getImm() ? true : false));

                if (this->getImm()) {
                    this->printImmediate(this->getImm() << this->_scale, false);
                }

                std::cout << "]";
            }
    };

#endif
