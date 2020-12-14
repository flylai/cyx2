#ifndef CVM_BASICBLOCK_HPP
#define CVM_BASICBLOCK_HPP

#include "../ast/ast.hpp"
#include "ir_instruction.hpp"

#include <utility>
#include <vector>

namespace COMPILER
{
    class BasicBlock
    {
      public:
        explicit BasicBlock() = default;
        explicit BasicBlock(std::string name) : name(std::move(name)){};

        template<typename... T>
        void addInst(Stmt *first, T *...others)
        {
            addInst(first);
            addInst(others...);
        }
        void addInst(Stmt *instruction)
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
        const std::vector<BasicBlock *> &succs() const
        {
            return _succs;
        }

      public:
        std::string name;
        std::vector<Stmt *> _insts;
        std::vector<BasicBlock *> _pres;
        std::vector<BasicBlock *> _succs;
    };
} // namespace COMPILER

#endif // CVM_BASICBLOCK_HPP
