#ifndef CVM_CFG_BUILDER_H
#define CVM_CFG_BUILDER_H

#include "../../utility/utility.hpp"
#include "../ast/ast_visitor.h"
#include "../ast/expr.hpp"
#include "../ast/stmt.hpp"
#include "basicblock.hpp"
#include "cfg.h"
#include "ir_instruction.hpp"

#include <unordered_map>
#include <vector>

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
        std::vector<BasicBlock *> basicBlock();
        BasicBlock *entry{ new BasicBlock("CFG ENTRY") };

      private:
        BasicBlock *getBasicBlock(const std::string &name = "");

      private:
        BasicBlock *cur_basic_block{ nullptr };
        std::unordered_map<std::string, BasicBlock *> basic_blocks;
        std::vector<IRInstruction *> insts;
    };
} // namespace COMPILER

#endif // CVM_CFG_BUILDER_H
