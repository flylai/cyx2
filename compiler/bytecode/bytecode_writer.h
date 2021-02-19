#ifndef CVM_BYTECODE_WRITER_H
#define CVM_BYTECODE_WRITER_H

#include "../../core/opcode.hpp"
#include "../../core/vm_instruction.hpp"

#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace COMPILER
{
    class BytecodeWriter
    {
      public:
        explicit BytecodeWriter(std::string filename) : filename(std::move(filename))
        {
        }
        void writeInsts();
        void writeToFile();

      private:
        void writeHeader(long long entry);
        void writeByte(unsigned char val);
        void writeInt(long long val);
        void writeDouble(double val);
        void writeString(const std::string &val);
        void writeOpcode(CVM::Opcode opcode);
        //
        void writeBinary();
        //
        void writeLoadX();
        template<typename T>
        void writeLoad();
        //
        void writeStoreX();
        template<typename T>
        void writeStore();
        //
        void writeArg();
        void writeCall();
        void writeFunc();
        void writeParam();
        void writeRet();
        void writeJmp();
        void writeJif();

      private:
        CVM::VMInstruction *cur_inst{ nullptr };

        std::string filename;
        int pos{ 0 };
        int size{ 0 };
        std::vector<unsigned char> buffer;

      public:
        std::vector<CVM::VMInstruction *> vm_insts;
    };
} // namespace COMPILER

#endif // CVM_BYTECODE_WRITER_H
