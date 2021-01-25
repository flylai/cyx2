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
            _insts.push_back(instruction);
        }
        //
        void addPre(BasicBlock *block)
        {
            _pres.insert(block);
        }
        //
        void addSucc(BasicBlock *block)
        {
            _succs.insert(block);
        }
        //
        const std::unordered_set<BasicBlock *> &succs() const
        {
            return _succs;
        }
        const std::unordered_set<BasicBlock *> &pres() const
        {
            return _pres;
        }
        std::list<IRInst *> &insts()
        {
            return _insts;
        }

      public:
        std::string name;
        int block_index;

      private:
        std::list<IRInst *> _insts;
        std::unordered_set<BasicBlock *> _pres;
        std::unordered_set<BasicBlock *> _succs;
    };
} // namespace COMPILER

#endif // CVM_BASICBLOCK_HPP
