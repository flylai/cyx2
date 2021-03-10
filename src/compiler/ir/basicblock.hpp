#ifndef CVM_BASICBLOCK_HPP
#define CVM_BASICBLOCK_HPP

#include <list>
#include <unordered_set>
#include <utility>
#include <vector>

namespace COMPILER
{
    class IRInst;
    class IRAssign;
    class IRVar;
    static int BASIC_BLOCK_COUNT = 0;
    class BasicBlock
    {
      public:
        explicit BasicBlock() : name("cfg_auto_gen_"), block_index(BASIC_BLOCK_COUNT++){};
        explicit BasicBlock(const std::string &name)
        {
            if (name.empty())
            {
                BasicBlock();
            }
            else
            {
                this->name  = name;
                block_index = BASIC_BLOCK_COUNT++;
            }
        };
        //
        void addInst(IRInst *instruction)
        {
            insts.push_back(instruction);
        }
        void addInstAfter(IRInst *instruction, const IRInst *after)
        {
            for (auto it = insts.begin(); it != insts.end(); it++)
            {
                auto *inst = *it;
                if (inst == after)
                {
                    insts.insert(it, instruction);
                    break;
                }
            }
        }
        void addInstBefore(IRInst *instruction, const IRInst *before)
        {
            for (auto it = insts.begin(), pre_it = it; it != insts.end(); it++)
            {
                auto *inst = *it;
                if (inst == before)
                {
                    insts.insert(pre_it, instruction);
                    break;
                }
                pre_it = it;
            }
        }
        //
        void addPre(BasicBlock *block)
        {
            pres.insert(block);
        }
        //
        void addSucc(BasicBlock *block)
        {
            succs.insert(block);
        }

      public:
        std::string name;
        int block_index;
        //
        std::list<IRInst *> insts;
        std::list<IRAssign *> phis;
        std::unordered_set<BasicBlock *> pres;
        std::unordered_set<BasicBlock *> succs;
        // lifetime interval (a.k.a live ness, live interval analysis) related
        std::unordered_set<std::string> live_in;
        std::unordered_set<std::string> live_out;
        std::unordered_set<std::string> kill;
        std::unordered_set<std::string> gen;
        //
        int from{ -1 };
        int to{ -1 };
        // loop related
        BasicBlock *loop_start{ nullptr };
        BasicBlock *loop_end{ nullptr };
    };
} // namespace COMPILER

#endif // CVM_BASICBLOCK_HPP
