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
        void writeArrIdx(const std::vector<CVM::ArrIdx> &arr_idx);

      private:
        CVM::VMInstruction *cur_inst{ nullptr };
        std::string filename;
        std::ofstream out;

      public:
        int entry{ -1 };
        int entry_end{ -1 };
        int global_var_len{ -1 };
        std::vector<CVM::VMInstruction *> vm_insts;
        void writeUnary();
    };
} // namespace COMPILER

#endif // CVM_BYTECODE_WRITER_H
