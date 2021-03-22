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

      private:
        bool fetch();
        void binary();
        //
        void loadX();
        void loadXA();
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
        //
        CYX::Value *findSymbol(const std::string &name);

      private:
        enum class Mode
        {
            INIT,
            MAIN
        };
        std::array<CYX::Value, 12> reg;
        std::vector<CVM::Frame> frame{ Frame() };
        //
        CYX::Value &state = reg[0]; // if stmt state
        std::vector<VMInstruction *> vm_insts;
        VMInstruction *cur_inst{ nullptr };
        //
        Mode mode = Mode::INIT;
        int entry{ 0 };               // main function position
        int pc{ 0 };                  // program counter
        int global_var_init_len{ 0 }; // global data initialize instruction length

      public:
        void setInsts(const std::vector<VMInstruction *> &insts);
        void setEntry(int i);
        void setGlobalInitLen(int i);
    };
} // namespace CVM

#endif