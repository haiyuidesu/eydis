//
//  database.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef DATABASE_HPP
    #define DATABASE_HPP
    #include <map>
    #include <string>
    #include <vector>
    #include <iomanip>
    #include <sstream>
    #include <iostream>
    #include <unistd.h>
    #include <stdexcept>
    #include "IEydis.hpp"
    #include <string_view>

    class Database {
        private:
            str_t _type;
            bool _found = true;
            std::vector<std::pair<uint64_t, int>> _db;

        public:
            /* ============= builders ============= */
            ~Database(void);
            Database(str_t, str_t);

            /* ============= database stuff ============= */
            void writeDatabase(int, uint64_t);
            bool readDatabase(uint64_t, bool);
            void printRoutine(int, str_t, str_t);
            void split(std::vector<std::string> *, std::string, str_t);

            /* ============= getters ============= */
            bool getFound(void) const { return this->_found; };
            str_t getType(void) const { return this->_type; };
            std::vector<std::pair<uint64_t, int>> *getDb(void) { return &this->_db; };

            /* ============= setters ============= */
            void setFound(bool found) { this->_found = found; };
            void setType(str_t type) { this->_type = type; };
    };

    inline
    std::shared_ptr<Database> createDb(str_t name, str_t type)
    {
        return std::make_shared<Database>(name, type);
    }

#endif
