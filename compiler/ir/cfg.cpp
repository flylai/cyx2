#include "cfg.h"

void COMPILER::CFG::init()
{
    for (auto block : basic_blocks)
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
    for (auto *block : cur_basic_block->succs())
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
        for (auto *pre_block : cur_block->pres())
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

std::string COMPILER::CFG::iDomDetailStr()
{
    std::string str;
    for (const auto &x : idom)
    {
        str += (x.first != nullptr ? x.first->name : "null") + " dominated by " +
               (x.second != nullptr ? x.second->name : "null") + "\n";
    }
    return str;
}

void COMPILER::CFG::calcDominanceFrontier()
{
    for (auto *block : basic_blocks)
    {
        if (block->pres().size() >= 2)
        {
            for (auto *pre : block->pres())
            {
                auto *runner = pre;
                while (runner != idom[block])
                {
                    dominance_frontier[runner].insert(block);
                    runner = idom[runner];
                }
            }
        }
    }
}
