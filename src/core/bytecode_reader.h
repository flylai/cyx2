#ifndef CVM_BYTECODE_READER_H
#define CVM_BYTECODE_READER_H

#include "../utility/log.h"
#include "opcode.hpp"
#include "vm_instruction.hpp"

#include <cmath>
#include <fstream>
#include <utility>

namespace CVM
{
    class BytecodeReader
    {
      public:
        BytecodeReader() = delete;
        explicit BytecodeReader(std::string filename) : filename(std::move(filename))
        {
        }
        void readFile();
        void readInsts();
        std::string vmInstStr();

      private:
        void readHeader();
        unsigned char readByte();
        long long readInt();
        double readDouble();
        std::string readString();
        //
        CVM::Opcode readOpcode();
        //
        void readBinary();
        //
        void readLoadX();
        void readLoadA();
        void readLoadXA();
        template<typename T>
        void readLoad();
        //
        void readStoreX();
        void readStoreA();
        template<typename T>
        void readStore();
        //
        void readArg();
        void readCall();
        void readFunc();
        void readParam();
        void readRet();
        void readJmp();
        void readJif();
        //
        void readArrIdx(std::vector<ArrIdx> &arr_idx);

      public:
        int entry{ -1 };
        int entry_end{ -1 };
        int global_var_len{ -1 };
        std::vector<VMInstruction *> vm_insts;

      private:
        Opcode cur_opcode;
        std::string filename;
        int pos{ 0 };
        int size{ 0 };
        unsigned char *buffer{ nullptr };
        void readUnary();
    };
} // namespace CVM

#endif // CVM_BYTECODE_READER_H
