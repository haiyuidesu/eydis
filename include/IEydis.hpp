//
//  ieydis.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef IEYDIS_HPP
    #define IEYDIS_HPP
    #include <cstring>
    #include <string_view>

    extern uint32_t _mem;
    extern uint64_t _addr;

    typedef std::string_view str_t;
    // i had to do this in order to make things easier even if it's ugly i admit...

    class IEydis {
        public:
            /* ============= builders ============= */
            virtual ~IEydis(void) = default;

            /* ============= setters ============= */
            virtual void setRet(int) = 0;
            virtual void setImm(int64_t) = 0;
            virtual void setMnemonic(str_t) = 0;
            virtual void setOperands(str_t) = 0;
            virtual void setValidity(uint32_t, uint32_t, uint32_t) = 0;

            /* ============= getters ============= */
            virtual int getRet(void) const = 0;
            virtual int64_t getImm(void) const = 0;
            virtual int getValidity(void) const = 0;
            virtual str_t getOperands(void) const = 0;
            virtual str_t getMnemonic(void) const = 0;

            /* ============= functions ============= */
            virtual void analyzeInsn(str_t) = 0;
    };

#endif
