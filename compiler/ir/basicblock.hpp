#ifndef CVM_BASICBLOCK_HPP
#define CVM_BASICBLOCK_HPP

#include "../ast/ast.hpp"
#include "ir_instruction.hpp"

#include <utility>
#include <vector>

namespace COMPILER
{
    static int BASICBLOCK_COUNT = 0;
    class BasicBlock
    {
      public:
        explicit BasicBlock() : name("block"), block_index(BASICBLOCK_COUNT++){};
        explicit BasicBlock(std::string name) : name(std::move(name)), block_index(BASICBLOCK_COUNT++){};

        template<typename... T>
        void addInst(Expr *first, T *...others)
        {
            addInst(first);
            addInst(others...);
        }
        void addInst(Expr *instruction)
        {
            _insts.push_back(instruction);
        }
        //
        template<typename... T>
        void addPre(BasicBlock *first, T *...others)
        {
            addPre(first);
            addPre(others...);
        }
        void addPre(BasicBlock *block)
        {
            _pres.push_back(block);
        }
        //
        template<typename... T>
        void addSucc(BasicBlock *first, T *...others)
        {
            addSucc(first);
            addSucc(others...);
        }
        void addSucc(BasicBlock *block)
        {
            _succs.push_back(block);
        }
        //
        const std::vector<BasicBlock *> &succs() const
        {
            return _succs;
        }
        const std::vector<BasicBlock *> &pres() const
        {
            return _pres;
        }
        const std::vector<Expr *> &insts() const
        {
            return _insts;
        }

      public:
        std::string name;
        int block_index;

      private:
        std::vector<Expr *> _insts;
        std::vector<BasicBlock *> _pres;
        std::vector<BasicBlock *> _succs;
    };
} // namespace COMPILER

#endif // CVM_BASICBLOCK_HPP
