//
//  config.hpp
//  eydis
//
//  Created by haiyuidesu on 02.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef CONFIG_HPP
    #define CONFIG_HPP
    #include <cstddef>
    #include <cstring>
    #include <fstream>
    #include <unistd.h>
    #include <getopt.h>
    #include <iostream>
    #include <algorithm>
    #include <string_view>
    #include <include/IEydis.hpp>

    #define bswap32(x)     __builtin_bswap32(x)

    #define EYDIS_ARM      (1 << 0)
    #define EYDIS_ARM64    (1 << 1)

    #define EYDIS_VERSION  "3.0"

    class Config {
        protected:
            char *_img = NULL;
            str_t _type = "ROM";
            uint64_t _end = 0x0;
            uint64_t _base = 0x0;
            uint64_t _start = 0x0;
            int _arch = EYDIS_ARM64;
            std::size_t _length = 0x0;
            str_t _database = "db.eydis";

        public:
            /* ============= builders ============= */
            Config(void) {};

            ~Config(void) {
                if (this->getImg() != NULL) free(this->getImg());
            };

            /* ============= setters ============= */
            void setImg(char *img) { this->_img = img; };
            void setArch(int arch) { this->_arch = arch; };
            void setEnd(uint64_t end) { this->_end = end; };
            void setType(str_t type) { this->_type = type; };
            void setBase(uint64_t base) { this->_base = base; };
            void setStart(uint64_t start) { this->_start = start; };
            void setLength(std::size_t length) { this->_length = length; };
            void setDbName(str_t database) { this->_database = database; };

            /* ============= getters ============= */
            std::size_t getLength(void) const { return this->_length; };
            str_t getDbName(void) const { return this->_database; };
            uint64_t getStart(void) const { return this->_start; };
            uint64_t getBase(void) const { return this->_base; };
            uint64_t getArch(void) const { return this->_arch; };
            uint64_t getEnd(void) const { return this->_end; };
            str_t getType(void) const { return this->_type; };
            char *getImg(void) { return this->_img; };
    };

#endif
