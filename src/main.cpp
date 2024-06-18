//
//  main.cpp
//  eydis
//
//  Created by haiyuidesu on 02.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#include <include/Shell.hpp>
#include <include/Eydis.hpp>

uint32_t _mem = 0x0;
uint64_t _addr = 0x0;

void sighandler(const int sig)
{
    signal(sig, SIG_IGN);

    exit(0);
}

static struct option largs[] = {
    { "database",  required_argument,   NULL, 'd' },
    { "rebase",    required_argument,   NULL, 'r' },
    { "help",      no_argument,         NULL, 'h' },
    { "end",       required_argument,   NULL, 'e' },
    { "start",     required_argument,   NULL, 's' },
    {  NULL,       no_argument,         NULL,  0  }
};

int usage(int argc, const char **argv)
{
    str_t name = argv[0] + ((argv[0][0] == '.') ? 2 : 0);

    std::cout << "usage: " << name << " <image> [args]" << std::endl
        << "\t-d, --database [file]\tspecify a database," << std::endl
        << "\t-r, --rebase [addr]\tspecify the rebase address,\n" << std::endl
        << "\t-s, --start [addr]\tstart from a specified address," << std::endl
        << "\t-e, --end [addr]\tend to a specified address." << std::endl;

    return ((argc != 2) ? -1 : 0);
}

int parseArgs(int argc, const char **argv, std::shared_ptr<Eydis> &eydis)
{
    int optidx = 0, opt = 0, ret = 1;

    std::map<char, void (Config::*)(uint64_t)> map = {
        { 'e',  &Config::setEnd },
        { 'r',  &Config::setBase },
        { 's',  &Config::setStart },
    };

    for (; (opt = getopt_long(argc, (char *const *)argv, "drhes", largs, &optidx)) != -1;) {
        switch (opt) {
            case 'h':
                return usage(argc, argv);
            case 'd':
            case 'r':
            case 's':
            case 'e':
                if (argv[optind] == NULL) {
                    std::cerr << "[ERROR]: missing argument." << std::endl;
                    return -1;
                }

                if (opt == 'd') {
                    eydis->setDbName(str_t(argv[optind]));
                } else {
                    (eydis.get()->*map[opt])(strtol(argv[optind], NULL, 0x10));
                }

                break;
            default:
                ret = -1;
        }
    }

    map.clear();

    return ret;
}

int main(int argc, const char **argv)
{
    int ret = 0;
    std::shared_ptr<Shell> shell = nullptr;
    std::shared_ptr<Eydis> eydis = nullptr;

    if (argc < 2) return usage(argc, argv);

    signal(SIGINT, (void (*)(int))sighandler);

    eydis = createEydis();

    std::cout << "\t\t[Eydis Disassembler, haiyuidesu © 2023]" << std::endl;
    std::cout.width(38);
    std::cout << "[version: " << EYDIS_VERSION << "]\n" << std::endl;

    if ((ret = parseArgs(argc, argv, eydis)) != 1) return ret;

    if ((ret = eydis->setupEydis(argv)) == -1) return ret;

    std::cout << *eydis << std::endl;

    if (eydis->getArch() == EYDIS_ARM) {
        std::cerr << "[ERROR]: 32-bit images are not supported yet" << std::endl;
        return -1;
    }

   eydis->setDb(createDb(eydis->getDbName(), eydis->getType()));

    /*
     *  When the 'getFound()' function returns false, it means that a database has been found
     *  And so, Eydis can starts now the disassemblage (else, it will search for every routines possible).
    */
    eydis->analyzeImage(eydis->getDb()->getFound());

    if (eydis->getDb()->getFound() == true) {
        std::cout << std::endl;
        eydis->analyzeImage(false);
    }

    shell = createShell(std::move(eydis));

    shell->eydish();

    return ret;
}
