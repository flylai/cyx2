#include "cfg_builder.h"

#define LINK(PRE, SUCC)                                                                                                \
    if (PRE != nullptr && SUCC != nullptr && PRE != SUCC)                                                              \
    {                                                                                                                  \
        PRE->addSucc(SUCC);                                                                                            \
        SUCC->addPre(PRE);                                                                                             \
    }

COMPILER::BasicBlock *COMPILER::CFGBuilder::getBasicBlock(const std::string &name)
{
    if (basic_blocks.find(name) == basic_blocks.end())
    {
        basic_blocks[name] = name.empty() ? new BasicBlock() : new BasicBlock(name);
    }
    return basic_blocks[name];
}

void COMPILER::CFGBuilder::setInsts(const std::vector<IRInstruction *> &inst)
{
    insts = inst;
}

void COMPILER::CFGBuilder::buildCFG()
{
    for (auto x : insts)
    {
        if (x->opcode == IROpcode::IR_LABEL)
        {
            auto *next_bb = getBasicBlock(x->operand1.as<std::string>());
            if (cur_bb != nullptr)
            {
                if (cur_bb->insts().empty()) cur_bb->addSucc(next_bb);
                next_bb->addPre(cur_bb);
            }
            cur_bb = next_bb;
        }
        else if (x->opcode == IROpcode::IR_GOTO)
        {
            auto *basic_block = getBasicBlock(x->operand1.as<std::string>());
            cur_bb->addInst(x);
            cur_bb->addSucc(basic_block);
            basic_block->addPre(cur_bb);
        }
        else
        {
            cur_bb->addInst(x);
        }
    }
}
void COMPILER::CFGBuilder::cfg2Graph()
{
    for (const auto &block : basic_blocks)
    {
        graph += block.first + " [shape=record, label=\"{" + block.second->name + "|";
        for (int i = 0; i < block.second->insts().size(); i++)
        {
            graph += block.second->insts()[i]->toString(false);
            if (i != block.second->insts().size() - 1) graph += "|";
        }
        graph += "}\"]\n";
        for (auto *block_succ : block.second->succs())
        {
            graph += block.first + "->" + block_succ->name + "\n";
        }
    }
}

#define NODE(PTR) PTR->name + std::to_string(PTR->block_index)
// digit2HexStr(static_cast<void *>(PTR))
#define EDGE(FROM, TO) ((FROM) != nullptr && (TO) != nullptr) ? NODE(FROM) + "->" + NODE(TO) + "\n" : ""
