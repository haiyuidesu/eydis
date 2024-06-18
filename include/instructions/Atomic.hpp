//
//  Atomic.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef ATOMIC_HPP
    #define ATOMIC_HPP
    #include <include/Eydis.hpp>

    //  31   30            24 23   22 21   20       16 15  14  13     10  9     4 3   0
    // +-----+---------------+---+---+---+------------+---+---+---------+--------+----+
    // |  1  | x 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 0 0 0 0 |   Rn   | Rt | LDADD, LDADDA, LDADDAL, LDADDL
    // |  1  | x 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 1 0 0 0 |   Rn   | Rt | LDEOR, LDEORA, LDEORAL, LDEORL
    // |  1  | x 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 1 1 0 0 |   Rn   | Rt | LDSET, LDSETA, LDSETAL, LDSETL
    // |  1  | x 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 0 1 0 0 |   Rn   | Rt | LDCLR, LDCLRA, LDCLRAL, LDCLRL
    // |  0  | 0 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 1 0 0 0 |   Rn   | Rt | LDEORB, LDEORAB, LDEORALB, LDEORLB
    // |  0  | 0 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 0 1 0 0 |   Rn   | Rt | LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB
    // |  0  | 1 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 0 1 0 0 |   Rn   | Rt | LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH
    // |  0  | 0 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 0 0 0 0 |   Rn   | Rt | LDADDB, LDADDAB, LDADDALB, LDADDLB
    // |  0  | 1 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 0 0 0 0 |   Rn   | Rt | LDADDH, LDADDAH, LDADDALH, LDADDLH
    // |  0  | 1 1 1 1 0 0 0 | A | R | 1 |     Rs     | 0 | 0 | 1 1 0 0 |   Rn   | Rt | LDSETH, LDSETAH, LDSETALH, LDSETLH
    // +-----+---------------+---+---+---+------------+---+---+---------+--------+----+
    //  size                                               opc
    class LoadAtomic : virtual public Eydis {
        private:
            unsigned _op = 0x0;

        public:
            LoadAtomic(void) : Eydis() {
                this->setValidity(_mem, 0x3f208c00, 0x38200000);
                this->_op = (((_mem >> 12) & 0x7) << 4) | (this->size2(_mem) << 2) | this->sh0(_mem);
                this->setMnemonic((this->_op > 0x40) ? "" : this->atomicLoad[this->_op]);
                this->_isValid = ((this->getMnemonic() == "") ? 0 : this->getValidity());
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printRegister(this->getRm(_mem), this->is64Bit(_mem), None, true);

                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, true);

                std::cout << "[";

                this->printRegister(this->getRn(_mem), this->is64Bit(_mem), None, false);

                std::cout << "]";
            }
    };

    //  31   30            24 23   22 21   20       16 15  14  13     10  9     4 3  0
    // +-----+---------------+---+---+---+------------+---+---+---------+--------+----+
    // |  1  | x 1 1 1 0 0 0 | A | R | 1 |     Rs     | 1 | 0 | 0 0 0 0 |   Rn   | Rt | SWP, SWPA, SWPAL, SWPL
    // |  0  | 0 1 1 1 0 0 0 | A | R | 1 |     Rs     | 1 | 0 | 0 0 0 0 |   Rn   | Rt | SWPB, SWPAB, SWPALB, SWPLB
    // |  0  | 1 1 1 1 0 0 0 | A | R | 1 |     Rs     | 1 | 0 | 0 0 0 0 |   Rn   | Rt | SWPH, SWPAH, SWPALH, SWPLH
    // +-----+---------------+---+---+---+------------+---+---+---------+--------+----+
    //  size
    class SwapAtomic : virtual public Eydis {
        public:
            SwapAtomic(void) : Eydis() {
                this->setValidity(_mem, 0x3f20fc00, 0x38208000);
                this->setMnemonic(this->atomicSwap[(this->size2(_mem) << 2) | this->sh0(_mem)]);
                this->_isValid = ((this->getMnemonic() == "") ? 0 : this->getValidity());
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);

                this->printRegister(this->getRm(_mem), this->is64Bit(_mem), None, true);

                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, true);

                std::cout << "[";

                this->printRegister(getRn(_mem), this->is64Bit(_mem), None, false);

                std::cout << "]";
            }
    };

#endif
