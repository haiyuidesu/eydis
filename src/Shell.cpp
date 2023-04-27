//
//  shell.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#include "../include/Shell.hpp"

void Shell::eydish(void)
{
    std::map<str_t, void (Shell::*)(std::string)> map = {
        { "help",    &Shell::help },
        { "file",    &Shell::saveFile },
        { "patch",   &Shell::patchImage },
        { "routine", &Shell::routine },
        { "reload",  &Shell::reload },
    };

    std::cout << "\n-------------------------------------------------------------------\n" << std::endl;

    for (std::string cmd, tmp; cmd != "exit";) {
        std::cout << "\033[0m> eydis $ ";

        std::getline(std::cin, cmd);

        if (std::cin.fail() || std::cin.eof()) break;

        tmp = cmd.substr(0x0, cmd.find(" ")); // split at the 1st string in case we're having a cmd with args

        if (map[tmp] != nullptr) {
            (this->*map[tmp])(cmd);
        } else if ((cmd != "") && (cmd != "exit")) {
            std::cerr << "\t[ERROR]: unknown command: \"" << cmd << "\", please use 'help'." << std::endl;
        }

        tmp.clear();
    }

    this->saveDb("");

    map.clear();
}

/* ============= functions ============= */

void Shell::help(std::string cmd)
{
    (void)cmd;
    std::cout << "  list of all available commands:" << std::endl
              << "  \thelp\t\t\tprint this help" << std::endl
              << "  \tfile\t\t\tsave the current instance in its current state" << std::endl
              << "  \treload\t\t\treload the current instance" << std::endl
              << "  \troutine\t\t\tre-analyze the whole instance" << std::endl
              << "  \tpatch [addr] [patch]\tpatch the current file" << std::endl;
}

/*
 *  This function will just run the 'analyzeImage(false)',
 *  In order to reload the current disassemlage with every modifications applied to it.
*/
void Shell::reload(std::string cmd)
{
    (void)cmd;
    this->_eydis->analyzeImage(false);
    std::cout << "\n-------------------------------------------------------------------\n" << std::endl;
}

/*
 *  This function will just run the 'analyzeImage(true)',
 *  In order to detect if any new routines has been added since the first run.
*/
void Shell::routine(std::string cmd)
{
    (void)cmd;
    this->_eydis->analyzeImage(true);
}

/*
 *  This function is simply used for saving the current file
 *  With every modifications applied to it.
*/
void Shell::saveFile(std::string cmd)
{
    (void)cmd;
    std::string name = std::string(this->_eydis->getName()) + ".patched";
    std::ofstream out(name, std::ofstream::binary | std::ofstream::out);

    std::cout << "  [INFO]: writing " << name << "... ";

    out.write(this->_eydis->getImg(), this->_eydis->getLength());

    std::cout << "[OK!]" << std::endl;

    out.close();
}

/*
 *  This function has the same purpose as the previous one,
 *  Except that it was made for the Database.
*/
void Shell::saveDb(std::string cmd)
{
    (void)cmd;
    std::string what = "";
    auto *_db = this->_eydis->getDb()->getDb();

    std::cout << "  [INFO]: saving eydis database [y/n] > ";

    std::getline(std::cin, what);

    ((what == "n") ? remove(this->_eydis->getDbName().data()) : 0);

    if (what == "y") {
        std::ofstream out(this->_eydis->getDbName().data(), std::ofstream::out);

        for (auto it = _db->begin(); it != _db->end(); it++) {
            out << std::hex << it->first << "|" << std::dec << it->second << std::endl;
        }

        out.close();
    }

    std::cout << "  [INFO]: the database has been " << ((what == "y") ? "saved." : "removed.") << std::endl;
}

/*
 *  This function purpose's in the name: patch image.
 *  It should be used like: patch [addr] [patch hex].
 *  I will maybe implement an instruction builder later.
*/
void Shell::patchImage(std::string cmd)
{
    bool success;
    uint64_t addr = 0x0;
    uint64_t patch = 0x0;
    std::vector<std::string> split;

    if (cmd == "patch") {
        std::cerr << "  [ERROR]: missing arguments." << std::endl;
        return;
    }

    this->_eydis->getDb()->split(&split, cmd, " ");

    if ((split[1] == "") || (split[2] == "") || (split.size() > 3)) {
        std::cerr << "  [ERROR]: wrong patch or address resquested." << std::endl;
        goto end;
    }

    addr = strtol(split[1].c_str(), NULL, 0x10);
    patch = strtol(split[2].c_str(), NULL, 0x10);

    addr -= ((addr > this->_eydis->getLength()) ? this->_eydis->getBase() : 0x0);

    if ((addr <= 0x0) || (addr > this->_eydis->getLength())) {
        std::cerr << "  [ERROR]: wrong patch address resquested." << std::endl;
        goto end;
    }

    *(uint32_t *)&(this->_eydis->getImg()[addr]) = bswap32(patch);

    success = ((*(uint32_t *)&(this->_eydis->getImg()[addr]) == bswap32(patch)) ? true : false);

    std::cout << "  [" << ((success == true) ? "INFO" : "ERROR")
            << "]: patched (0x" << patch << ")" << " at 0x" << std::hex << this->_eydis->getBase() + addr
            << ((success == true) ? ": [OK!]" : ": [FAIL!]") << std::endl;

end:
    split.clear();
}
