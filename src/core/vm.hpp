#ifndef CORE_VM_HPP
#define CORE_VM_HPP

#include "../common/buildin.hpp"
#include "../common/value.hpp"
#include "frame.hpp"
#include "opcode.hpp"
#include "vm_instruction.hpp"

#include <dbg.h>
#include <memory>
#include <string>
#include <vector>

namespace CVM
{
    class VM
    {
      public:
        void run();

        //
        int pc = 0;

      private:
        bool fetch();
        void binary();
        //
        void loadX();
        void load();
        //
        void storeX();
        void store();
        //
        void arg();
        void call();
        void callBuildin();
        void func();
        void param();
        void ret();
        void jmp();
        void jif();

      private:
        std::array<CYX::Value, 12> reg;
        std::vector<CVM::Frame> frame{ Frame() };
        bool state = false; // if stmt state
        std::vector<VMInstruction *> vm_insts;
        VMInstruction *cur_inst{ nullptr };

      public:
        void setInsts(const std::vector<VMInstruction *> &insts);
        void setEntry(int i);
    };
} // namespace CVM

#endif