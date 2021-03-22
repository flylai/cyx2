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
        void writeHeader();
        void writeByte(unsigned char val);
        void writeInt(long long val);
        void writeDouble(double val);
        void writeString(const std::string &val);
        void writeOpcode(CVM::Opcode opcode);
        //
        void writeBinary();
        //
        void writeLoadX();
        void writeLoadA();
        void writeLoadXA();
        template<typename T>
        void writeLoad();
        //
        void writeStoreX();
        template<typename T>
        void writeStore();
        void writeStoreA();
        //
        void writeArg();
        void writeCall();
        void writeFunc();
        void writeParam();
        void writeRet();
        void writeJmp();
        void writeJif();
        //
        void writeIntTag();
        void writeDoubleTag();
        void writeStringTag();
        void writeEmptyTag();

      private:
        CVM::VMInstruction *cur_inst{ nullptr };

        std::string filename;
        int pos{ 0 };
        int size{ 0 };
        std::vector<unsigned char> buffer;

      public:
        int entry{ -1 };
        int global_var_len{ -1 };
        std::vector<CVM::VMInstruction *> vm_insts;
    };
} // namespace COMPILER

#endif // CVM_BYTECODE_WRITER_H
