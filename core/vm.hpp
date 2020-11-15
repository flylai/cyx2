#ifndef CORE_VM_HPP
#define CORE_VM_HPP

#include "instruction.hpp"
#include "opcode.hpp"
#include "stack.h"
#include "type.hpp"
#include "vm_config.h"

#include <array>
#include <dbg.h>
#include <memory>
#include <string>

namespace CVM::ASM
{
    class VM
    {
      public:
        void run();

      private:
        void mov(const ASM::Instruction &instruction);
        void jmp(const ASM::Instruction &instruction);
        void jif(const ASM::Instruction &instruction);
        void call(const ASM::Instruction &instruction);
        void ret(const ASM::Instruction &instruction);
        void push(const ASM::Instruction &instruction);
        void pop(const ASM::Instruction &instruction);

        template<Opcode Op>
        void arithmetic(const ASM::Instruction &instruction);
        template<Opcode Op>
        void comparison(const ASM::Instruction &instruction);

      private:
        std::array<Type *, CONFIG::REGISTER_SIZE> reg;
        CORE::Stack stack;
        //
        int pc    = 0;
        int state = 0; // if stmt state
        std::vector<ASM::Instruction> code;

      public:
        void setCode(const std::vector<ASM::Instruction> &insts);
        void setEntry(int i);
    };
} // namespace CVM::ASM

#endif