#ifndef CYX2_BYTECODE_BASICBLOCK_H
#define CYX2_BYTECODE_BASICBLOCK_H

#include "../../core/vm_instruction.hpp"

#include <list>
#include <string>
#include <utility>

class BytecodeBasicBlock
{
  public:
    BytecodeBasicBlock() = delete;
    explicit BytecodeBasicBlock(std::string block_name) : name(std::move(block_name)){};
    std::list<CVM::VMInstruction *> vm_insts;
    std::string name;
};

#endif // CYX2_BYTECODE_BASICBLOCK_H
