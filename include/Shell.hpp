//
//  shell.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef SHELL_HPP
    #define SHELL_HPP
    #include <map>
    #include <memory>
    #include <cstddef>
    #include <cstring>
    #include <iostream>
    #include <algorithm>
    #include "Eydis.hpp"
    #include <string_view>

    class Shell {
        private:
            std::shared_ptr<Eydis> _eydis = nullptr;

        public:
            /* ============= builders ============= */
            ~Shell(void) {};
            Shell(std::shared_ptr<Eydis> &what) : _eydis(what) {};

            /* ============= functions ============= */
            void eydish(void);
            void help(std::string);
            void reload(std::string);
            void saveDb(std::string);
            void routine(std::string);
            void saveFile(std::string);
            void patchImage(std::string);
    };

    inline
    std::shared_ptr<Shell> createShell(std::shared_ptr<Eydis> eydis)
    {
        return std::make_shared<Shell>(eydis);
    }

#endif
