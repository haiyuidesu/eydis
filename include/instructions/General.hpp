//
//  general.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef GENERAL_HPP
    #define GENERAL_HPP
    #include <list>
    #include "BL.hpp"
    #include "Hint.hpp"
    #include "BTest.hpp"
    #include "BComp.hpp"
    #include "BCond.hpp"
    #include "Atomic.hpp"
    #include "Return.hpp"
    #include "LoadInsn.hpp"
    #include "MoveWide.hpp"
    #include "Multiply.hpp"
    #include "MemBarrier.hpp"
    #include "DataProcess.hpp"
    #include "Calculation.hpp"
    #include "PCRelativeCalc.hpp"
    #include "BitwiseRegister.hpp"
    #include "ConditionalSelect.hpp"
    #include "UnsignedLoadStoreOffset.hpp"

    class Unknow : virtual public Eydis {
        public:
            Unknow(void) : Eydis() {
                this->setMnemonic("db");
                this->_isValid = 1;
            };

            void analyzeInsn(str_t type)
            {
                this->printInsnInfos(type);
            }
    };

    inline std::list<std::unique_ptr<Eydis>> makeInstruction(bool isAnalyzing)
    {
        std::list<std::unique_ptr<Eydis>> list;

        list.emplace_back(new BL {});
        list.emplace_back(new BCond {});
        list.emplace_back(new BComp {});
        list.emplace_back(new BTest {});

        if (isAnalyzing == false) {
            list.emplace_back(new MSRImm {});
            list.emplace_back(new Return {});
            list.emplace_back(new CalcImm {});
            list.emplace_back(new MoveWide {});
            list.emplace_back(new LoadStorePairRegister {});
            list.emplace_back(new LDRLiteral {});
            list.emplace_back(new PCRelativeCalc {});
            list.emplace_back(new CalcShiftRegister {});
            list.emplace_back(new ConditionalSelect {});
            list.emplace_back(new LoadStoreRegister {});
            list.emplace_back(new CalcExtendRegister {});
            list.emplace_back(new UnsignedLoadStoreOffset {});
            list.emplace_back(new LoadStoreUnsignedImm {});
            list.emplace_back(new DataProcessFloat {});
            list.emplace_back(new BitwiseRegister {});
            list.emplace_back(new DataProcessNext {});
            list.emplace_back(new DataProcess {});
            list.emplace_back(new Multiply {});
            list.emplace_back(new Hint {});
            list.emplace_back(new memBarrier {});
            list.emplace_back(new LoadAtomic {});
            list.emplace_back(new SwapAtomic {});
            list.emplace_back(new Unknow {}); // no insn were detected then.
        }

        return list;
    }
#endif
