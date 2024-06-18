//
//  database.cpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#include <include/Database.hpp>

/* ============= builders ============= */

/*
 *  This constructor will be used to create (or not) a database.
 *  Here, I will set the value '0xdeadbeaf' just to tell that this database
 *  Was created by Eydis, so if this value was found then we don't have to create a new one.
*/
Database::Database(str_t name, str_t type) : _type(type)
{
    std::vector<std::string> split;

    std::cout << "\r[INFO]: loading file into the database... ";

    if (access(name.data(), F_OK) != -1) {
        std::ifstream file(name.data(), std::ifstream::in);

        if (file.is_open() == true) {
            for (std::string line = ""; std::getline(file, line);) {
                this->split(&split, line, "|");

                if (split.size() != 2) {
                    split.clear();
                    continue;
                }

                try {
                    this->_db.push_back({ std::stoll(split[0], 0x0, 0x10), std::stoll(split[1], 0x0, 0x10) });
                } catch (std::out_of_range &e) {
                    this->_db.push_back({ std::atoi(split[0].c_str()), std::atoi(split[1].c_str()) });
                } // It appears that std::stoi is a bit capricious so i'm using std::atoi.

                split.clear();
            }

            file.close();
        }

        this->setFound(!this->readDatabase(0xdeadbeef, false));

        std::cout << " [OK!]" << std::endl; // meaning that a database was already created by Eydis
    } else {
        this->_db.push_back({ 0xdeadbeef, 1 });
    }

    std::cout << std::endl;
}

Database::~Database(void)
{
    this->getDb()->clear();
}

/* ============= functions ============= */

/*
 *  This function purpose is in the name.
*/
void Database::split(std::vector<std::string> *ret, std::string str, str_t delim)
{
    (*ret).clear();

    for (size_t pos = 0; (pos = str.find(delim)) != std::string::npos;) {
        (*ret).push_back(str.substr(0, pos));
        str.erase(0, pos + std::strlen(delim.data()));
    }

    (*ret).push_back(str);
}

/*
 *  This function is used to print new subroutines infos.
 *  It do be ugly yes, but it do the trick.
*/
void Database::printRoutine(int isSub, str_t type, str_t name)
{
    if (isSub == 1) {
        std::cout << "\033[38;5;242m" << type << ":" << std::hex << std::setfill('0') << std::setw(0x10) << _addr << std::endl;
        std::cout << "\033[38;5;242m" << type << ":" << std::hex << std::setfill('0') << std::setw(0x10) << _addr
        << "  ; =================== S U B R O U T I N E =======================" << std::endl;
    }
    std::cout << "\033[38;5;242m" << type << ":" << std::hex << std::setfill('0') << std::setw(0x10) << _addr << std::endl;
    std::cout << "\033[38;5;242m" << type << ":" << std::hex << std::setfill('0') << std::setw(0x10) << _addr << "\t  \033[38;5;227m" << name << "\033[30;0m" << std::endl;
    std::cout << "\033[38;5;242m" << type << ":" << std::hex << std::setfill('0') << std::setw(0x10) << _addr << std::endl;
}

/*
 *  This function is used to insert new data into the database.
 *  I believe this could be improved.
*/
void Database::writeDatabase(int isSub, uint64_t addr)
{
    if (this->readDatabase(addr, false) == true) return;

    this->_db.push_back({ addr, isSub });
}

/*
 *  This function is supposed to be called to print each routines.
 *  I believe this could be improved.
*/
bool Database::readDatabase(uint64_t addr, bool print)
{
    auto where = std::find_if(
          this->getDb()->begin(),
          this->getDb()->end(),
          [addr](const auto &mo) { return mo.first == addr; }
        );
    
    if (where == this->getDb()->end()) return false;

    if (print == true) {
        int isSub = where->second;
        uint64_t what = where->first;

        std::size_t bytes = std::snprintf(NULL, 0x0, "%s%llx", ((isSub == 1) ? "sub_" : "loc_"), what);

        char *buf = (char *)malloc(sizeof(char) * (bytes + 1));

        std::snprintf(buf, bytes + 1, "%s%llx", ((isSub == 1) ? "sub_" : "loc_"), what);

        this->printRoutine(isSub, this->_type, buf);

        free(buf);
    }

    return true;
}
