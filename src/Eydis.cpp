//
//  eydis.cpp
//  eydis
//
//  Created by haiyuidesu on 02.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#include "../include/Eydis.hpp"
#include "../include/instructions/General.hpp"

/* ============= database ============= */

/*
 *  This function will 'setup' the database.
 *  I believe this could be speeded up but i don't know how
 *  (multi-threading was the first idea back then but I removed it because
 *  it seemed to be slower than I expected...)
*/
void Eydis::analyzeImage(bool isAnalyzing)
{
    _addr = this->getBase();

    if (isAnalyzing == false) this->getDb()->printRoutine(1, this->getType(), "_start");

    for (uint64_t i = this->getStart(); i <= this->getEnd(); i += 0x4) {
        _addr = this->getBase() + i;
        _mem = *(uint32_t *)(this->getImg() + i);

        ((isAnalyzing == true) ? this->findRoutines(i) : this->disasm());
    }
}

/*
 *  This function is used to find and insert every routines addresses possible.
 *  At first, I used multi-threading in order to "try" to make Eydis
 *  The more optimised possible but I removed it since I was quite not sure about the efficiency.
 *  I consider to re-implement it though since it may be useful somehow-
*/
void Eydis::findRoutines(int where)
{
    std::list<std::unique_ptr<Eydis>> list = makeInstruction<BL, BCond, BComp, BTest>();
    // 'makeInstruction' will setup a list of insn that contains routines addresses as operands.

    std::array<str_t, 0x4> prologues = {
        "\x7f\x23\x03\xd5", "\xbe\xa9", "\xbf\xa9", "\xbd\xa9"
    };

    for (auto &idx : prologues) {
        if (std::memcmp(&(this->getImg())[where], &idx, idx.size()) == 0) {
            this->getDb()->writeDatabase(1, _addr);
            break;
        }
    }
    
    for (auto &insn : list) {
        if (insn->getValidity() == 1) {
            this->getDb()->writeDatabase(insn->getRet(), (_addr + insn->getImm()));
            break;
        }
    }

    list.clear();

    this->progressBar(((double)where / (double)this->getEnd()) * 100.0);
}

/*
 *  This function will check among all instructions set which one seems correct.
 *  I believe this could be improved.
*/
void Eydis::disasm(void)
{
    std::list<std::unique_ptr<Eydis>> list = makeInstruction<BL, BCond, BComp, BTest,
        MSRImm, Return, CalcImm, MoveWide, LoadStorePairRegister, LDRLiteral,
        PCRelativeCalc, CalcShiftRegister, ConditionalSelect, LoadStoreRegister,
        CalcExtendRegister, UnsignedLoadStoreOffset, LoadStoreUnsignedImm, DataProcessFloat,
        BitwiseRegister, DataProcessNext, DataProcess, Multiply, Hint, memBarrier, LoadAtomic, SwapAtomic, Unknow>();
    // all instructions to find will be added there, just enter the insn class name.

    for (auto &insn : list) {
        if (insn->getValidity() == 1) {
            this->getDb()->readDatabase(_addr, true); // check to see if the current address is a routine's beginning
            insn->analyzeInsn(this->getType());  // 'getType()' is used to print the type of the current instance
            std::cout << std::endl;
            break; // we're done, no need to stay in the loop any longer
        }
    }

    list.clear();
}

/* ============= configuration ============= */

/*
 *  This function's purpose is to set correct values into Eydis class's members
 *  And check their validity.
*/
int Eydis::setupEydis(const char **argv)
{
    FILE *fd = NULL;
    std::vector<std::vector<str_t>> infos = {
        { "iBoot",     "\x69\x42\x6F\x6F" },
        { "SEPROM",    "\x64\x2E\x2E\x2E\x28\x72\x6F\x6F" },
        { "SecureROM", "\x72\x65\x52\x4F" }
    };

    this->setName(argv[optind]);

    if (!(fd = std::fopen(this->getName().data(), "rb+"))) {
        std::cerr << "[ERROR]: unable to open " << this->getName() << std::endl;
        return -1;
    }

    std::fseek(fd, 0x0, SEEK_END);

    this->setLength(std::ftell(fd));

    std::fseek(fd, 0x0, SEEK_SET);

    this->setImg((char *)std::malloc(sizeof(char) * this->getLength() + 1));

    std::fread(this->getImg(), 1, this->getLength(), fd);

    std::fclose(fd);

    // setting up the limits of the disassembling
    this->setEnd((((this->getEnd() == 0x0) || (this->getEnd() > this->getLength())) ? this->getLength() : this->getEnd()));

    if (this->getStart() > this->getEnd()) {
        std::cerr << "[ERROR]: the starting point cannot be above the ending point" << std::endl;
        return -1;
    }
    
    if (*(uint32_t *)this->getImg() == bswap32(0x0e0000ea)) {
        this->setArch(EYDIS_ARM);
    } // simple check to see if we're dealing with a 32-bit image (which is not supported for the moment)

    for (auto &str : infos) {
        if (this->setImgInfos(str[0], str[1]) == 0) break;
    }

    for (auto &str : infos) str.clear();

    infos.clear();

    return 0;
}

/*
 *  This function's purpose is to check if the image is an Apple bootloader
 *  And set the other correct values corresponding to.
*/
int Eydis::setImgInfos(str_t type, str_t hex)
{
    // the version is only useful for the iBoots
    int version = std::atoi(this->getImg() + 0x286);

    if (memmem(this->getImg(), this->_length, hex.data(), hex.size())) {
        this->setType(type);

        if (this->getArch() == EYDIS_ARM) {
            this->setBase(*(uint32_t *)(this->getImg() + 0x20) & ~0xfffff);
        } else {
            this->setBase(((type == "SEPROM") ? 0x240000000 : *(uint64_t *)(this->getImg() + ((version >= 6723) ? 0x300 : 0x318))));
        }

        return 0;
    }

    return -1;
}

/*
 *  Simple loading bar to let the user know about the file analysis progress.
 *  At the beginning, I used it as callback: typedef void (Eydis::*eydis_callback_t)(double);
 *  But now I use it as a simple function because... because.
*/
void Eydis::progressBar(double what)
{
    if (what < 0) return;

    if (what > 100) what = 100;

    std::cout << std::right << "\r[";

    for (int i = 0; i < 50; i++) {
        std::cout << ((i < (what / 2)) ? "=" : " ");
    }

    std::cout << "] loading... (" << std::dec << (int)what << "%)";

    if (what >= 100) std::cout << std::endl;

    std::cout.flush();
}

/*
 *  Technically, this operator is useless.
 *  I just could've add a printf but I judged better to do things that way for my sake.
*/
std::ostream &operator<<(std::ostream &fd, const Eydis &eydis)
{
    return fd << std::hex << "[INFO]: "
        << eydis.getType() << ((eydis.getArch() == EYDIS_ARM) ? " (32-bit)" : " (64-bit)") << std::endl
        << "[INFO]: base_addr = 0x" << eydis.getBase() << std::endl
        << "[INFO]: limits = [0x" << eydis.getStart() << " - 0x" << eydis.getEnd() << "]" << std::endl;
}

/* ============= print ============= */

template <typename Type>
void Eydis::printImmediate(Type imm, bool sep)
{
    std::cout << "#\033[38;5;196m0x" << std::hex << imm << "\033[30;0m";

    if (sep == true) std::cout << ", ";
}

template <typename Type>
void Eydis::printShift(Type shift, bool isImm)
{
    std::array<str_t, 4> mne = { "lsl", "lsr", "asl", "ror" };

    std::cout << "\033[38;5;242m";

    if (isImm == true) {
        std::cout << "#" << std::dec << shift << std::hex;
    } else {
        std::cout << mne[shift & 0x3];
    }

    std::cout << "\033[30;0m";
}

void Eydis::printAddr(str_t type)
{
    std::cout << "\033[38;5;242m" << type << ":" << std::hex << std::setfill('0') << std::setw(0x10) << _addr;
}

void Eydis::printInsnInfos(str_t type)
{
    this->printAddr(type);
    std::cout << "\033[30;0m\t\t";
    this->hexDump();
    std::cout << "\t\t" << this->getMnemonic() << "\t\t";
}

void Eydis::printRegister(unsigned reg, unsigned is64Bit, Eydis::r31 state, bool sep)
{
    std::vector<str_t> what = {
        { (is64Bit ? "sp" : "wsp") },  // SPtr -> Stack Pointer
        { (is64Bit ? "xzr" : "wzr") }, // Zero -> Zero Register
        { (is64Bit ? "x" : "w") + std::to_string(reg) }, // None -> Not the 31th register
    };

    state = ((reg != 0x1f) ? None : state);

    std::cout << "\033[0;36m" << what[state] << "\033[30;0m";

    if (sep == true) std::cout << ", ";

    what.clear();
}

void Eydis::printRegisterPrefix(unsigned reg, unsigned which, bool sep)
{
    std::cout << "\033[0;36m" << this->regPrefix[which] << reg << "\033[30;0m";

    if (sep == true) std::cout << ", ";
}

void Eydis::printOption(str_t opt, bool sep)
{
    std::cout << "\033[38;5;242m" << opt << "\033[30;0m";

    if (sep == true) std::cout << ", ";
}

void Eydis::hexDump(void)
{
    std::stringstream str;

    str << std::hex << bswap32(_mem);

    std::string tmp = str.str();

    if (tmp.size() != 0x8) tmp.insert(0x0, 0x8 - tmp.size(), '0');

    for (uint64_t i = 0; i != tmp.size(); i++) {
        std::cout << tmp[i];

        if ((i % 2) == 1) std::cout << " ";
    }

    str.clear();
}

void Eydis::printSysRegDesc(str_t reg, str_t desc)
{
    std::cout << "\033[30;0m ; \033[38;5;177m[>] " << reg << " | " << desc << "\033[30;0m";
}

// bool Eydis::isPrintable(unsigned char c)
// {
//     return ((c >= 0x20) && (c <= 0x7e || c == 0x09) ? 1 : 0);
// }

// void Eydis::printAddrString(uint64_t addr)
// {
//     std::ifstream tmp(this->getName(), std::ifstream::in | std::ifstream::binary);

//     tmp.seekg(addr);

//     auto my_str = std::string();

//     for (int i = 0; isPrintable(addr + i) != 0; i++) {
//         copy_n(std::istream_iterator<char>(tmp), 1, std::back_inserter(my_str));
//     }

//     tmp.close();
// }
