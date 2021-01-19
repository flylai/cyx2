#ifndef CVM_CFG_H
#define CVM_CFG_H

#include "basicblock.hpp"

#include <unordered_map>
#include <unordered_set>

namespace COMPILER
{
    class CFG
    {

      public:
        void buildDominateTree();
        std::string iDomDetailStr();

      public:
        std::vector<BasicBlock *> basic_blocks;
        BasicBlock *entry{};

      private:
        void init();
        void dfs(BasicBlock *cur_basic_block);
        void tarjan();
        void calcDominanceFrontier();
        // disjoint set
        COMPILER::BasicBlock *find(COMPILER::BasicBlock *block);

      private:
        std::vector<BasicBlock *> dfn;
        std::unordered_map<BasicBlock *, int> dfn_map;
        std::unordered_map<BasicBlock *, BasicBlock *> father;
        //
        std::unordered_map<BasicBlock *, BasicBlock *> sdom; // a.k.a semi, not strict.
        std::unordered_map<BasicBlock *, BasicBlock *> idom; // the closest point of dominated point
        std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> tree;
        std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> dominance_frontier;
        //
        std::unordered_map<BasicBlock *, BasicBlock *> disjoint_set;
        std::unordered_map<BasicBlock *, BasicBlock *> disjoint_set_val;
        //
        std::unordered_map<BasicBlock *, bool> visited;
    };
} // namespace COMPILER

#endif // CVM_CFG_H
