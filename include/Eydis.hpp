//
//  eydis.hpp
//  eydis
//
//  Created by haiyuidesu on 03.01.23.
//  Copyright © 2023 haiyuidesu. All rights reserved.
//

#ifndef EYDIS_HPP
    #define EYDIS_HPP
    #include <include/Config.hpp>
    #include <include/IEydis.hpp>
    #include <include/Database.hpp>
    #include <include/Utilities.hpp>

    class Eydis : virtual public IEydis, virtual public Config, public Utilities {
        protected:
            str_t _name;
            int _isValid;
            int64_t _imm;
            uint64_t _ret;
            str_t _operands;
            str_t _mnemonic;
            std::shared_ptr<Database> _db = nullptr;

        public:
            enum r31 {
                SPtr = 0,
                Zero = 1,
                None = 2,
            }; // this enum. is used for the 31th (Zero) register (which can also be a StackPointer)
            
            /* ============= builders ============= */
            Eydis(void) : Config() {};
            virtual ~Eydis(void) = default;

            /* ============= setters ============= */
            void setName(str_t name) { this->_name = name; }
            void setRet(int ret) override { this->_ret = ret; }
            void setImm(int64_t imm) override { this->_imm = imm; }
            void setDb(std::shared_ptr<Database> db) { this->_db = db; }
            void setOperands(str_t op) override { this->_operands = op; }
            void setMnemonic(str_t mne) override { this->_mnemonic = mne; }

            void setValidity(uint32_t mem, uint32_t mask, uint32_t value) override
            {
                this->_isValid = (((mem & mask) != value) ? -1 : 1);
            }

            /* ============= getters ============= */
            str_t getName(void) const { return this->_name; }
            int getRet(void) const override { return this->_ret; }
            int64_t getImm(void) const override { return this->_imm; }
            int getValidity(void) const override { return this->_isValid; }
            std::shared_ptr<Database> getDb(void) const { return this->_db; }
            str_t getOperands(void) const override { return this->_operands; }
            str_t getMnemonic(void) const override { return this->_mnemonic; }

            /* ============= database ============= */
            void disasm(void);
            void findRoutines(int);
            void analyzeImage(bool);
            int setupEydis(const char **);

            /* ============= configuration ============= */
            void progressBar(double);
            int setImgInfos(str_t, str_t);
            void analyzeInsn(str_t) override {};

            /* ============= print ============= */
            void printRegister(unsigned, unsigned, Eydis::r31, bool);
            template <typename Type> void printImmediate(Type, bool);
            template <typename Type> void printShift(Type, bool);
            void printRegisterPrefix(unsigned, unsigned, bool);
            void printSysRegDesc(str_t, str_t);
            bool isPrintable(unsigned char);
            void printAddrString(uint64_t);
            void printOption(str_t, bool);
            void printInsnInfos(str_t);
            void printAddr(str_t);
            void hexDump(void);
    };

    std::ostream &operator<<(std::ostream &, const Eydis &);

    inline
    std::shared_ptr<Eydis> createEydis(void)
    {
        return std::make_shared<Eydis>();
    }

#endif
