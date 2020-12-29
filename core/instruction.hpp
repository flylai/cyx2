#ifndef CVM_INSTRUCTION_HPP
#define CVM_INSTRUCTION_HPP

#include "../common/value.hpp"
#include "opcode.hpp"

#include <memory>

namespace CYX::ASM
{
    class Instruction
    {
      public:
        ASM::Opcode opcode{ 0xff };

        int idx1{ -1 }; // if operand target are REGISTER/STACK/GLOBAL
        std::shared_ptr<Value> operand1{ nullptr };
        OperandTarget operand_target1{ OperandTarget::UNKNOWN };

        int idx2{ -1 };
        std::shared_ptr<Value> operand2{ nullptr };
        OperandTarget operand_target2{ OperandTarget::UNKNOWN };
    };
} // namespace CYX::ASM

#endif // CVM_INSTRUCTION_HPP
