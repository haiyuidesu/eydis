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
    #include <memory>
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

    template<typename ... Instructions>
    inline std::list<std::unique_ptr<Eydis>> makeInstruction(void)
    {
        std::list<std::unique_ptr<Eydis>> list;

        (list.emplace_back(std::move(std::make_unique<Instructions>())), ...);

        return list;
    }

#endif
