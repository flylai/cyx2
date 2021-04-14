#ifndef CVM_BYTECODE_GENERATOR_H
#define CVM_BYTECODE_GENERATOR_H

#include "../../common/buildin.hpp"
#include "../../common/config.h"
#include "../../core/opcode.hpp"
#include "../../core/vm_instruction.hpp"
#include "../../utility/utility.hpp"
#include "../ir/ir_instruction.hpp"
#include "bytecode_basicblock.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace COMPILER
{
    class BytecodeGenerator
    {
      public:
        BytecodeGenerator();
        void ir2VmInst();
        void relocation();
        std::string vmInstStr();

      private:
        void addInst(CVM::VMInstruction *inst);
        //
        void collectBlockMap();
        void fixJmp();
        void fixCall();
        //
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
        void genLoadX(int reg_idx, const std::string &name, const std::vector<CVM::ArrIdx> &idx);
        void genLoadXA(int reg_idx, const std::vector<std::pair<std::string, int>> &idx);
        void genLoadA(int reg_idx, const std::vector<CYX::Value> &index);
        //
        template<typename T>
        void genStore(const std::string &name, T val);
        void genStoreA(const std::string &name, CYX::Value &value, const std::vector<CVM::ArrIdx> &idx);
        void genStoreX(const std::string &name, int reg_idx);
        void genStoreX(const std::string &name, int reg_idx, const std::vector<CVM::ArrIdx> &idx);
        //
        void parseVarArr(IRVar *var, std::vector<CVM::ArrIdx> &arr_idx);
        //
        void genGlobalVarDecl();

      public:
        int entry{ -1 };
        int entry_end{ -1 };
        int global_var_len{ -1 };
        std::vector<IRFunction *> funcs;
        std::vector<CVM::VMInstruction *> vm_insts;
        std::vector<BytecodeBasicBlock *> bytecode_basicblocks;
        BasicBlock *global_vars{ nullptr };

      private:
        std::string entry_end_block_name;
        std::unordered_map<std::string, int> block_table;
        std::unordered_map<std::string, int> funcs_table;
    };
} // namespace COMPILER

#endif // CVM_BYTECODE_GENERATOR_H
