#include "cfg.h"

void COMPILER::CFG::init()
{
    for (auto block : funcs[0]->blocks)
    {
        sdom[block]             = block;
        disjoint_set[block]     = block;
        disjoint_set_val[block] = block;
    }
}

void COMPILER::CFG::dfs(COMPILER::BasicBlock *cur_basic_block)
{
    if (cur_basic_block == nullptr) return;
    if (visited.find(cur_basic_block) != visited.end()) return;
    visited[cur_basic_block] = true;
    dfn.push_back(cur_basic_block);
    dfn_map[cur_basic_block] = dfn.size() - 1;
    for (auto *block : cur_basic_block->succs)
    {
        dfs(block);
        father[block] = cur_basic_block;
    }
}

COMPILER::BasicBlock *COMPILER::CFG::find(COMPILER::BasicBlock *block)
{
    if (block == disjoint_set[block]) return block;
    auto *tmp = find(disjoint_set[block]);
    if (dfn_map[sdom[disjoint_set_val[disjoint_set[block]]]] < dfn_map[sdom[disjoint_set_val[block]]])
        disjoint_set_val[block] = disjoint_set_val[disjoint_set[block]];
    disjoint_set[block] = tmp;
    return tmp;
}

void COMPILER::CFG::tarjan()
{
    for (unsigned long i = dfn.size() - 1; i >= 1; i--)
    {
        auto *cur_block = dfn[i];
        if (cur_block == nullptr) continue;
        for (auto *pre_block : cur_block->pres)
        {
            if (dfn_map.find(pre_block) != dfn_map.end())
            {
                find(cur_block);
                int a           = dfn_map[sdom[cur_block]];
                int b           = dfn_map[sdom[disjoint_set_val[pre_block]]];
                sdom[cur_block] = dfn[std::min(a, b)];
            }
        }
        tree[sdom[cur_block]].insert(cur_block);
        disjoint_set[cur_block] = father[cur_block];
        auto *tmp               = disjoint_set[cur_block];
        for (auto *block : tree[tmp])
        {
            find(block);
            if (dfn_map[sdom[disjoint_set_val[block]]] < dfn_map[father[cur_block]])
                idom[block] = disjoint_set_val[block];
            else
                idom[block] = father[cur_block];
        }
        tree[tmp].clear();
    }
    for (int i = 1; i < dfn.size(); i++)
    {
        auto *cur_block = dfn[i];
        if (idom[cur_block] != sdom[cur_block]) idom[cur_block] = idom[idom[cur_block]];
        tree[idom[cur_block]].insert(cur_block);
    }
}

void COMPILER::CFG::buildDominateTree()
{
    init();
    dfs(entry);
    tarjan();
    calcDominanceFrontier();
}

void COMPILER::CFG::calcDominanceFrontier()
{
    for (auto *func : funcs)
    {
        for (auto *block : func->blocks)
        {
            if (block->pres.size() >= 2)
            {
                for (auto *pre : block->pres)
                {
                    auto *runner = pre;
                    while (runner != idom[block] && runner != nullptr)
                    {
                        dominance_frontier[runner].insert(block);
                        runner = idom[runner];
                    }
                }
            }
        }
    }
}

void COMPILER::CFG::simplifyCFG()
{
    for (auto *func : funcs)
    {
        for (auto it = func->blocks.begin(); it != func->blocks.end();)
        {
            auto *block = *it;
            if (block->insts.empty() && !block->pres.empty() && block->succs.size() == 1)
            {
                // remove it!
                auto *succ = *block->succs.begin();
                succ->pres.erase(block);
                for (auto *pre : block->pres)
                {
                    // remove block from pred's succs
                    pre->succs.erase(block);
                    pre->succs.insert(succ);
                    succ->pres.insert(pre);
                    // remove block from pred's insts
                    for (auto *inst : pre->insts)
                    {
                        if (inOr(inst->tag, IR::Tag::BRANCH, IR::Tag::JMP))
                        {
                            if (inst->tag == IR::Tag::BRANCH)
                            {
                                auto *tmp = static_cast<IRBranch *>(inst);
                                if (tmp->true_block == block) tmp->true_block = succ;
                                if (tmp->false_block == block) tmp->false_block = succ;
                            }
                            else
                            {
                                auto *tmp = static_cast<IRJump *>(inst);
                                if (tmp->target == block) tmp->target = succ;
                            }
                        }
                    }
                }
                it = func->blocks.erase(it);
                delete block;
            }
            else
            {
                it++;
            }
        }
    }
}

void COMPILER::CFG::collectVarAssign()
{
    for (auto *func : funcs)
    {
        var_block_map.clear();
        for (auto *block : func->blocks)
        {
            for (auto *inst : block->insts)
            {
                if (inst->tag == IR::Tag::ASSIGN)
                {
                    auto *assign = static_cast<IRAssign *>(inst);
                    if (assign->dest->def == nullptr && assign->dest->is_ir_gen) continue;
                    var_block_map[assign->dest->name].insert(block);
                }
            }
        }
    }
}

void COMPILER::CFG::insertPhiNode()
{
    std::queue<BasicBlock *> work_list;
    std::unordered_map<BasicBlock *, std::string> inserted;

    for (const auto &p : var_block_map)
    {
        const std::string var_name = p.first;
        for (auto *block : p.second)
        {
            work_list.push(block);
        }
        while (!work_list.empty())
        {
            auto *block = work_list.front();
            work_list.pop();

            for (auto *df_block : dominance_frontier[block])
            {
                if (inserted[df_block] != var_name)
                {
                    // add phi node
                    inserted[df_block] = var_name;
                    auto *phi          = new IRPhi;
                    phi->name          = var_name;
                    df_block->phis.push_back(phi);
                    // add block's dominance frontier to worklist
                    work_list.push(df_block);
                }
            }
        }
    }
}

void COMPILER::CFG::tryRename()
{
    for (const auto &p : var_block_map)
    {
        counter[p.first] = 0;
    }
    rename(entry);
}

void COMPILER::CFG::rename(COMPILER::BasicBlock *block)
{
    for (auto *phi : block->phis)
    {
        phi->ssa_index = newId(phi->name);
    }
    for (auto *inst : block->insts)
    {
        if (inst->tag == IR::Tag::ASSIGN)
        {
            auto *assign = static_cast<IRAssign *>(inst);
            if (assign->src->tag == IR::Tag::BINARY)
            {
                auto *src = static_cast<IRBinary *>(assign->src);
                if (src->lhs != nullptr && src->lhs->tag == IR::Tag::VAR)
                {
                    auto *src_lhs      = static_cast<IRVar *>(src->lhs);
                    src_lhs->ssa_index = getId(src_lhs->name);
                }
                if (src->rhs != nullptr && src->rhs->tag == IR::Tag::VAR)
                {
                    auto *src_rhs      = static_cast<IRVar *>(src->rhs);
                    src_rhs->ssa_index = getId(src_rhs->name);
                }
            }
            else if (assign->src->tag == IR::Tag::VAR)
            {
                auto *src      = static_cast<IRVar *>(assign->src);
                src->ssa_index = getId(src->name);
            }
            if (assign->dest->def == nullptr && assign->dest->is_ir_gen) continue;
            assign->dest->ssa_index = newId(assign->dest->name);
        }
    }
    for (auto *succ : block->succs)
    {
        for (auto *phi : succ->phis)
        {
            phi->args.push_back(phi->name + std::to_string(getId(phi->name)));
        }
    }
    for (auto *succ : tree[block])
    {
        rename(succ);
    }
    for (auto *inst : block->insts)
    {
        if (inst->tag == IR::Tag::ASSIGN)
        {
            auto *assign = static_cast<IRAssign *>(inst);
            if (!stack[assign->dest->name].empty()) stack[assign->dest->name].pop();
        }
    }
    for (auto *phi : block->phis)
    {
        if (!stack[phi->name].empty()) stack[phi->name].pop();
    }
}

int COMPILER::CFG::newId(const std::string &name)
{
    int i = counter[name]++;
    stack[name].push(i);
    return i;
}

int COMPILER::CFG::getId(const std::string &name)
{
    if (stack[name].empty()) return 0;
    return stack[name].top();
}

/////////////////////////dump as str//////////////////////

std::string COMPILER::CFG::iDomDetailStr() const
{
    std::string str;
    for (const auto &x : idom)
    {
        str += (x.first != nullptr ? x.first->name : "null") + " dominated by " +
               (x.second != nullptr ? x.second->name : "null") + "\n";
    }
    return str;
}

std::string COMPILER::CFG::dominanceFrontierStr() const
{
    std::string str;
    for (const auto &p : dominance_frontier)
    {
        str += p.first->name + " : {";
        for (auto *block : p.second)
        {
            str += block->name + " ";
        }
        str += "}\n";
    }
    return str;
}

std::string COMPILER::CFG::dumpCFG() const
{
    std::string str;
    for (auto *func : funcs)
    {
        for (auto *block : func->blocks)
        {
            str += block->name + " [shape=record, label=\"{" + block->name;
            for (auto *inst : block->insts)
            {
                str += "|" + inst->toString();
            }
            str += "}\"]\n";

            for (auto *succ : block->succs)
            {
                str += block->name + " -> " + succ->name + "\n";
            }
        }
    }
    return str;
}
