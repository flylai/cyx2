#include "cfg_builder.h"

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
            if (cur_basic_block != nullptr)
            {
                if (cur_basic_block->insts().empty() ||
                    (!cur_basic_block->insts().empty() && cur_basic_block->insts().back()->opcode != IROpcode::IR_GOTO))
                {
                    cur_basic_block->addSucc(next_bb);
                    next_bb->addPre(cur_basic_block);
                }
            }
            else
            {
                // entry block.
                entry->addSucc(next_bb);
            }
            cur_basic_block = next_bb;
        }
        else if (x->opcode == IROpcode::IR_GOTO)
        {
            auto *basic_block = getBasicBlock(x->operand1.as<std::string>());
            cur_basic_block->addInst(x);
            cur_basic_block->addSucc(basic_block);
            basic_block->addPre(cur_basic_block);
        }
        else
        {
            cur_basic_block->addInst(x);
        }
    }
}
void COMPILER::CFGBuilder::cfg2Graph()
{
    for (const auto &block : basic_blocks)
    {
        graph += block.first + " [shape=record, label=\"{@" + block.second->name + "|";
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

std::vector<COMPILER::BasicBlock *> COMPILER::CFGBuilder::basicBlock()
{
    std::vector<BasicBlock *> vct;
    for (auto &basic_block : basic_blocks)
    {
        vct.push_back(basic_block.second);
    }
    return vct;
}
