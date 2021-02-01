#ifndef CVM_CFG_H
#define CVM_CFG_H

#include "../../utility/utility.hpp"
#include "basicblock.hpp"
#include "ir_instruction.hpp"

#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace COMPILER
{
    class CFG
    {

      public:
        void simplifyCFG();
        void buildDominateTree();
        std::string iDomDetailStr() const;
        std::string dominanceFrontierStr() const;
        std::string dumpCFG() const;

      public:
        std::vector<IRFunction *> funcs;
        BasicBlock *entry{};

      private:
        void init();
        void dfs(BasicBlock *cur_basic_block);
        void tarjan();
        void calcDominanceFrontier();
        // disjoint set
        COMPILER::BasicBlock *find(COMPILER::BasicBlock *block);
        // SSA construction
        void collectVarAssign();
        void insertPhiNode();
        void tryRename();
        void rename(BasicBlock *block);
        int newId(const std::string &name);
        int getId(const std::string &name);

      private:
        // dfs related
        std::vector<BasicBlock *> dfn;
        std::unordered_map<BasicBlock *, int> dfn_map;
        std::unordered_map<BasicBlock *, BasicBlock *> father;
        std::unordered_map<BasicBlock *, bool> visited;
        // dominate tree related
        std::unordered_map<BasicBlock *, BasicBlock *> sdom; // a.k.a semi, not strict.
        std::unordered_map<BasicBlock *, BasicBlock *> idom; // the closest point of dominated point
        std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> tree;
        std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> dominance_frontier;
        // disjoint set (union find) related
        std::unordered_map<BasicBlock *, BasicBlock *> disjoint_set;
        std::unordered_map<BasicBlock *, BasicBlock *> disjoint_set_val;
        // SSA construction.....
        std::unordered_map<std::string, std::unordered_set<BasicBlock *>> var_block_map;
        std::unordered_map<std::string, int> counter;
        std::unordered_map<std::string, std::stack<int>> stack;
    };
} // namespace COMPILER

#endif // CVM_CFG_H
