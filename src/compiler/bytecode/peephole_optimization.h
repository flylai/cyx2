#ifndef CYX2_PEEPHOLE_OPTIMIZATION_H
#define CYX2_PEEPHOLE_OPTIMIZATION_H

#include "../../core/vm_instruction.hpp"
#include "../../utility/utility.hpp"
#include "bytecode_basicblock.hpp"

#include <deque>
#include <unordered_map>
#include <vector>

namespace COMPILER
{
    class PeepholeOptimization
    {
      public:
        void doPeepholeOptimization();

      public:
        std::vector<BytecodeBasicBlock *> *block_list;

      private:
        void collectJumpTarget();
        //
        void traversal();
        void pass(
            std::deque<std::pair<std::list<CVM::VMInstruction *> *, std::list<CVM::VMInstruction *>::iterator>> &window,
            std::list<CVM::VMInstruction *>::iterator &cur_it);

      private:
        bool changed = true;
        std::unordered_map<std::string, int> jump_map;
    };
}; // namespace COMPILER

#endif // CYX2_PEEPHOLE_OPTIMIZATION_H
