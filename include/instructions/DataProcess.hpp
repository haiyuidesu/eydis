//
//  DataProcess.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef DATAPROCESS_HPP
    #define DATAPROCESS_HPP
    #include "../Eydis.hpp"

    //  Data Processing: convert data from one size to another.
    class DataProcess : virtual public Eydis {
        private:
            unsigned _op = 0x0;
            unsigned _isPac = 0x0;
            unsigned _nextPac = 0x0;
            unsigned _isDataProcess = 0x0;

        public:
            DataProcess(void) : Eydis() {
                this->setImm(this->imms(_mem));
                this->setValidity(_mem, 0x5fe00000, 0x5ac00000);

                this->_op = (this->getRm(_mem) == 1) && (this->is64Bit(_mem)) && (this->imms(_mem) <= 0x1001);

                this->_isPac = this->_op && (this->imms(_mem) <= 0x00111 || this->getRd(_mem) == 0x11111);

                this->_nextPac = (!this->_isPac) || (this->_isPac && ((this->imms(_mem) <= 0x00111)));

                this->_isDataProcess = (this->getRm(_mem))
                                        || (this->imms(_mem) & 0x38)
                                        || ((this->imms(_mem) & 0x3e) == 0x6)
                                        || (this->is64Bit(_mem)
                                        && (this->imms(_mem) == 0x3));

                this->setMnemonic((this->_isPac ? this->pacAuth[this->getImm()]
                                                : ((!this->is64Bit(_mem) && this->imms(_mem) == 0x2)
                                                ? "rev" : this->dataProcess[this->getImm() & 0x7])));
                
                if ((this->getMnemonic() == "") || (this->aBit(_mem)) || ((!this->_isPac) && (!this->_isDataProcess))) {
                    this->_isValid = 0;
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), None, (this->_nextPac ? true : false));
                if (this->_nextPac) this->printRegister(this->getRn(_mem), this->is64Bit(_mem), None, false);
            }
    };

    //  DataProcessing: convert data from one size to another.
    class DataProcessNext : virtual public Eydis {
        public:
            DataProcessNext(void) : Eydis() {
                this->setImm(this->imms(_mem));
                this->setValidity(_mem, 0x5fe00000, 0x1ac00000);
                this->setMnemonic(this->shiftNext[this->getImm()]);

                if ((this->getMnemonic() == "") || (this->aBit(_mem)) || ((!this->is64Bit(_mem) && this->hw0(_mem) >= 0x2))) {
                    this->_isValid = 0;
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
                this->printRegister(this->getRd(_mem), this->is64Bit(_mem), Zero, true);
                this->printRegister(this->getRn(_mem), this->is64Bit(_mem), Zero, true);
                this->printRegister(this->getRm(_mem), this->is64Bit(_mem), Zero, false);
            }
    };

    //  DataProcessingFloat: convert data from one size to another (for float).
    class DataProcessFloat : virtual public Eydis {
        private:
            unsigned _cond = 0x0;

        public:
            DataProcessFloat(void) : Eydis() {
                this->setValidity(_mem, 0x5f207c00, 0x1e204000);
                this->setMnemonic(this->floatPointProcess[this->op6(_mem)]);
                this->_cond = (this->op6(_mem) >= 0x4) && (this->op6(_mem) <= 0x7);

                if ((this->getMnemonic() == "") || (this->opd(_mem)) || (this->aBit(_mem)) || (this->op6(_mem) > 0x10)) {
                    this->_isValid = 0;
                }
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
                this->printRegisterPrefix(this->getRd(_mem), ((this->_cond) ? ((this->op6(_mem) & 0x3) ^ 0x2) : this->sh0(_mem)), false);
                this->printRegisterPrefix(this->getRn(_mem), this->sh0(_mem) ^ ((this->_cond) ? 0x2 : 0x0), false);
            }
    };

#endif
