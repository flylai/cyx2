#include "lifetime_analysis.h"

// https://ssw.jku.at/Research/Papers/Wimmer04Master/Wimmer04Master.pdf

void COMPILER::LifetimeAnalysis::buildIntervals()
{
    for (auto block_it = function->blocks.rbegin(); block_it != function->blocks.rend(); block_it++)
    {
        const auto *block = *block_it;
        for (const auto &opr : block->live_out)
        {
            intervals[opr].addRange(block->from, block->to);
        }
        for (auto inst_it = block->insts.rbegin(); inst_it != block->insts.rend(); inst_it++)
        {
            auto *inst   = *inst_it;
            auto *assign = as<IRAssign, IR::Tag::ASSIGN>(inst);
            if (assign != nullptr)
            {
                auto *binary = as<IRBinary, IR::Tag::BINARY>(assign->src());
                if (binary != nullptr)
                {
                    auto *lhs = as<IRVar, IR::Tag::VAR>(binary->lhs);
                    auto *rhs = as<IRVar, IR::Tag::VAR>(binary->rhs);
                    if (lhs != nullptr)
                    {
                        intervals[lhs->toString()].addRange(block->from, assign->id);
                    }
                    if (rhs != nullptr)
                    {
                        intervals[rhs->toString()].addRange(block->from, assign->id);
                    }
                }
                auto *dest = assign->dest();
                intervals[dest->toString()].setFrom(assign->id);
            }
        }
    }
}

void COMPILER::LifetimeAnalysis::numberingOperations()
{
    int idx = 0;
    for (auto *block : function->blocks)
    {
        if (block->from == -1) block->from = idx;
        for (auto *inst : block->insts)
        {
            inst->id = idx;
            idx += 2;
            //
            auto *assign = as<IRAssign, IR::Tag::ASSIGN>(inst);
            auto *branch = as<IRBranch, IR::Tag::BRANCH>(inst);
            if (assign != nullptr)
            {
                block->kill.insert(assign->dest()->toString());
                auto *binary = as<IRBinary, IR::Tag::BINARY>(assign->src());
                auto *var    = as<IRVar, IR::Tag::VAR>(assign->src());
                if (binary != nullptr)
                {
                    auto *lhs = as<IRVar, IR::Tag::VAR>(binary->lhs);
                    auto *rhs = as<IRVar, IR::Tag::VAR>(binary->rhs);
                    if (lhs != nullptr)
                    {
                        block->gen.insert(lhs->toString());
                    }
                    if (rhs != nullptr)
                    {
                        block->gen.insert(rhs->toString());
                    }
                }
                else if (var != nullptr)
                {
                    block->gen.insert(var->toString());
                }
            }
            else if (branch != nullptr)
            {
                auto *var = as<IRVar, IR::Tag::VAR>(branch->cond);
                block->gen.insert(var->toString());
            }
        }
        block->to = idx;
    }
}

std::string COMPILER::LifetimeAnalysis::getName(COMPILER::IRVar *var)
{
    return var->name + std::to_string(var->ssa_index);
}
std::string COMPILER::LifetimeAnalysis::intervalsStr()
{
    std::string retval;
    for (const auto &x : intervals)
    {
        retval += x.first + "\n";
        for (const auto &interval : x.second.interval)
        {
            addSpace(retval, 2);
            retval += std::to_string(interval.first) + " " + std::to_string(interval.second) + "\n";
        }
    }
    return retval;
}

void COMPILER::LifetimeAnalysis::computeGlobalLiveSet() const
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto block_it = function->blocks.rbegin(); block_it != function->blocks.rend(); block_it++)
        {
            auto block = *block_it;
            auto pre_live_out(block->live_out);
            block->live_out.clear();
            for (auto *succ : block->succs)
            {
                block->live_out.insert(succ->live_in.begin(), succ->live_in.end());
            }
            // unordered_set `!=` on c++20 is removed
            if (!(pre_live_out == block->live_out)) changed = true;

            auto pre_live_in(block->live_in);
            block->live_in = block->gen;
            for (const auto &x : block->live_out)
            {
                if (block->kill.find(x) == block->kill.end()) block->live_in.insert(x);
            }
            if (!(pre_live_in == block->live_in)) changed = true;
        }
    }
}

void COMPILER::LifetimeAnalysis::debug()
{
    for (auto *block : function->blocks)
    {
        logX(block->name);
        logX(block->live_in);
        logX(block->live_out);
        logX(block->gen);
        logX(block->kill);
    }
}
