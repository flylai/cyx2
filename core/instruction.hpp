#ifndef CVM_INSTRUCTION_HPP
#define CVM_INSTRUCTION_HPP

#include "opcode.hpp"
#include "type.hpp"

#include <dbg.h>

namespace CVM::ASM
{
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

        int idx1{ -1 }; // if operand target are REGISTER/STACK/GLOBAL
        std::shared_ptr<Type> operand1{ nullptr };
        OperandTarget operand_target1{ OperandTarget::UNKNOWN };

        int idx2{ -1 };
        std::shared_ptr<Type> operand2{ nullptr };
        OperandTarget operand_target2{ OperandTarget::UNKNOWN };
    };
} // namespace CVM::ASM

#endif // CVM_INSTRUCTION_HPP
