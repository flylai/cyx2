#ifndef CVM_CFG_BUILDER_H
#define CVM_CFG_BUILDER_H

#include "../../utility/utility.hpp"
#include "../ast/ast_visitor.h"
#include "../ast/expr.hpp"
#include "../ast/stmt.hpp"
#include "basicblock.hpp"
#include "cfg.hpp"
#include "ir_instruction.hpp"

#include <unordered_map>

namespace COMPILER
{
    class CFGBuilder
    {
      public:
        void setInsts(const std::vector<IRInstruction *> &inst);
        void buildCFG();
        void cfg2Graph();

      public:
        std::string graph;

      private:
        BasicBlock *getBasicBlock(const std::string &name = "");

      private:
        BasicBlock *cur_bb{ nullptr };
        BasicBlock *entry{ nullptr };
        std::unordered_map<BasicBlock *, bool> vis;
        std::unordered_map<std::string, BasicBlock *> basic_blocks;
        std::vector<IRInstruction *> insts;
    };
} // namespace COMPILER

#endif // CVM_CFG_BUILDER_H
