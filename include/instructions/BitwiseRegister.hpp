//
//  BitwiseRegister.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef BITWISEREGISTER_HPP
    #define BITWISEREGISTER_HPP
    #include "../Eydis.hpp"

    //  31   30             24 23   22 21  20        16 15          10 9         5 4      0
    // +----+---+-------------+-------+---+------------+--------------+-----------+-------+
    // | sf | 0 | 0 0 1 0 1 0 | shift | 1 |     Rm     |     imm6     |    Rn     |   Rd  | BIC shift
    // | sf | 0 | 0 0 1 0 1 0 | shift | 0 |     Rm     |     imm6     |    Rn     |   Rd  | AND shift
    // | sf | 0 | 1 0 1 0 1 0 | shift | 0 |     Rm     |     imm6     |    Rn     |   Rd  | ORR shift
    // | sf | 0 | 1 0 1 0 1 0 | shift | 1 |     Rm     |     imm6     |    Rn     |   Rd  | ORN shift
    // | sf | 1 | 0 0 1 0 1 0 | shift | 1 |     Rm     |     imm6     |    Rn     |   Rd  | EON shift
    // | sf | 1 | 0 0 1 0 1 0 | shift | 0 |     Rm     |     imm6     |    Rn     |   Rd  | EOR shift
    // +----+---+-------------+-------+---+------------+--------------+-----------+-------+
    //       opc                        N
    class BitwiseRegister : virtual public Eydis {
        private:
            bool _isMov = 0x0;
            bool _isTst = 0x0;
            unsigned _op = 0x0;

        public:
            BitwiseRegister(void) : Eydis() {
                this->setImm(this->imm6(_mem));
                this->setValidity(_mem, 0x1f000000, 0x0a000000);

                this->_op = (this->opc(_mem) << 0x1) | this->eBit(_mem);
                this->_isTst = ((this->_op == 0x6) && (this->getRd(_mem) == 0x1f));
                this->_isMov = ((this->opc(_mem) == 0x1) && (this->getRn(_mem) == 0x1f));

                if (this->_isTst) {
                    this->setMnemonic("tst");
                } else {
                    if (this->_isMov) {
                        this->setMnemonic(this->eBit(_mem) ? "mvn" : "mov");
                    } else {
                        this->setMnemonic(this->bitwise[this->_op]);
                    }
                }

                if (this->getMnemonic() == "" || ((!this->is64Bit(_mem)) && (this->getImm() & 0x20))) {
                    this->_isValid = 0;
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                if (!this->_isTst) this->printRegister(this->getRd(_mem), this->is64Bit(_mem), Zero, true);

                if (!this->_isMov) this->printRegister(this->getRn(_mem), this->is64Bit(_mem), Zero, true);

                this->printRegister(this->getRm(_mem), this->is64Bit(_mem), Zero, (this->getImm() ? true : false));

                if (this->getImm()) {
                    this->printShift(this->sh0(_mem), false);
                    this->printShift(this->getImm(), true);
                }
            }
    };

#endif
