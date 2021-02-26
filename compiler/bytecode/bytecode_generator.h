#ifndef CVM_BYTECODE_GENERATOR_H
#define CVM_BYTECODE_GENERATOR_H

#include "../../common/config.h"
#include "../../core/opcode.hpp"
#include "../../core/vm_instruction.hpp"
#include "../../utility/utility.hpp"
#include "../ir/ir_instruction.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace COMPILER
{
    class BytecodeGenerator
    {
      public:
        void ir2VmInst();
        std::vector<IRFunction *> funcs;
        std::vector<CVM::VMInstruction *> vm_insts;
        std::string vmInstStr();

      private:
        void fixJmp(int start, int end);
        void genBinary(IRBinary *ptr);
        void genLoadConst(CYX::Value &val, int reg_idx);
        void genStoreConst(CYX::Value &val, const std::string &name);
        void genReturn(IRReturn *ptr);
        void genJump(IRJump *ptr);
        void genJif(BasicBlock *target1, BasicBlock *target2);
        void genCall(IRCall *ptr);
        void genFunc(IRFunction *ptr);
        void genBranch(IRBranch *ptr);
        void genAssign(IRAssign *ptr);
        //
        template<typename T>
        void genLoad(int reg_idx, T val);
        void genLoadX(int reg_idx, const std::string &name);
        //
        template<typename T>
        void genStore(const std::string &name, T val);
        void genStoreX(const std::string &name, int reg_idx);

      public:
        int entry{ -1 };

      private:
        std::unordered_map<std::string, int> block_table;
        std::unordered_map<std::string, int> funcs_table;
    };
} // namespace COMPILER

#endif // CVM_BYTECODE_GENERATOR_H
