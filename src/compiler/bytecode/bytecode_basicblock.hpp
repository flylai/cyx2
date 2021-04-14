#ifndef CYX2_BYTECODE_BASICBLOCK_HPP
#define CYX2_BYTECODE_BASICBLOCK_HPP

#include "../../core/vm_instruction.hpp"

#include <list>
#include <string>
#include <utility>

namespace COMPILER
{
    class BytecodeBasicBlock
    {
      public:
        BytecodeBasicBlock() = delete;
        explicit BytecodeBasicBlock(std::string block_name) : name(std::move(block_name)){};
        std::list<CVM::VMInstruction *> vm_insts;
        std::string name;
    };
} // namespace COMPILER

#endif // CYX2_BYTECODE_BASICBLOCK_HPP
