#ifndef CVM_BASICBLOCK_HPP
#define CVM_BASICBLOCK_HPP

#include "../ast/ast.hpp"

#include <list>
#include <unordered_set>
#include <utility>
#include <vector>

namespace COMPILER
{
    class IRInst;
    class IRAssign;
    class IR;
    static int BASIC_BLOCK_COUNT = 0;
    class BasicBlock
    {
      public:
        explicit BasicBlock() : name("cfg_auto_gen_"), block_index(BASIC_BLOCK_COUNT++){};
        explicit BasicBlock(const std::string &name)
        {
            if (name.empty())
            {
                BasicBlock();
            }
            else
            {
                this->name  = name;
                block_index = BASIC_BLOCK_COUNT++;
            }
        };
        //
        void addInst(IRInst *instruction)
        {
            insts.push_back(instruction);
        }
        //
        void addPre(BasicBlock *block)
        {
            pres.insert(block);
        }
        //
        void addSucc(BasicBlock *block)
        {
            succs.insert(block);
        }

      public:
        std::string name;
        int block_index;
        //
        std::list<IRInst *> insts;
        std::list<IRAssign *> phis;
        std::unordered_set<BasicBlock *> pres;
        std::unordered_set<BasicBlock *> succs;
        // lifetime interval (a.k.a live ness, live interval analysis) related
        std::unordered_set<IR *> live_in;
        std::unordered_set<IR *> live_out;
        std::unordered_set<IR *> kill;
        std::unordered_set<IR *> gen;
    };
} // namespace COMPILER

#endif // CVM_BASICBLOCK_HPP
