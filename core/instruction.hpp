#ifndef CVM_INSTRUCTION_HPP
#define CVM_INSTRUCTION_HPP

#include "opcode.hpp"
#include "type.hpp"

#include <dbg.h>

namespace CVM::ASM
{
    enum class OperandType
    {
        NUMBER
    };

    enum class OperandTarget
    {
        REGISTER = 0x00,
        STACK    = 0x01,
        GLOBAL   = 0x02,
        RAW      = 0x03,
        UNKNOWN  = 0xff
    };

    class Instruction
    {
      public:
        CORE::Opcode opcode{ 0xff };

        RUNTIME::Object operand1;
        OperandTarget operand_target1{ OperandTarget::UNKNOWN };

        RUNTIME::Object operand2;
        OperandTarget operand_target2{ OperandTarget::UNKNOWN };
    };
} // namespace CVM::ASM

#endif // CVM_INSTRUCTION_HPP
