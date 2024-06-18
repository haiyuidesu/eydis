//
//  general.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef GENERAL_HPP
    #define GENERAL_HPP
    #include <vector>
    #include <memory>
    #include <include/instructions/BL.hpp>
    #include <include/instructions/Hint.hpp>
    #include <include/instructions/BTest.hpp>
    #include <include/instructions/BComp.hpp>
    #include <include/instructions/BCond.hpp>
    #include <include/instructions/Atomic.hpp>
    #include <include/instructions/Return.hpp>
    #include <include/instructions/LoadInsn.hpp>
    #include <include/instructions/MoveWide.hpp>
    #include <include/instructions/Multiply.hpp>
    #include <include/instructions/MemBarrier.hpp>
    #include <include/instructions/DataProcess.hpp>
    #include <include/instructions/Calculation.hpp>
    #include <include/instructions/PCRelativeCalc.hpp>
    #include <include/instructions/BitwiseRegister.hpp>
    #include <include/instructions/ConditionalSelect.hpp>
    #include <include/instructions/UnsignedLoadStoreOffset.hpp>

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
    inline std::vector<std::unique_ptr<Eydis>> makeInstruction(void)
    {
        std::vector<std::unique_ptr<Eydis>> vector;

        (vector.emplace_back(std::move(std::make_unique<Instructions>())), ...);

        return vector;
    }

#endif
