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
        void buildDominateTree(COMPILER::IRFunction *func);
        void transformToSSA();
        void removeUnusedPhis(IRFunction *func);
        std::string iDomDetailStr() const;
        std::string dominanceFrontierStr() const;
        std::string cfgStr() const;

      public:
        std::vector<IRFunction *> funcs;

      private:
        void init(IRFunction *func);
        void clear();
        void dfs(BasicBlock *cur_basic_block);
        void tarjan();
        void calcDominanceFrontier(COMPILER::IRFunction *func);
        // disjoint set
        COMPILER::BasicBlock *find(COMPILER::BasicBlock *block);
        // SSA construction
        void collectVarAssign(COMPILER::IRFunction *func);
        void insertPhiNode();
        void removeTrivialPhi(COMPILER::IRFunction *func);
        //
        void constantPropagation(COMPILER::IRFunction *func);
        void constantFolding(COMPILER::IRFunction *func);
        std::optional<CYX::Value> tryFindConstant(COMPILER::IRVar *var);
        //
        void destroyPhiNode(COMPILER::IRAssign *assign);
        void phiElimination(COMPILER::IRFunction *func);
        // rename
        void tryRename(COMPILER::IRFunction *func);
        void rename(BasicBlock *block);
        void renameIrArgs(IR *inst);
        void renameFuncCall(IRCall *inst);
        void renameVar(IRVar *var);
        int newId(const std::string &name, IRVar *def);
        std::pair<int, IRVar *> getId(const std::string &name);

      private:
        BasicBlock *entry{ nullptr };
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
        std::unordered_map<std::string, std::stack<std::pair<int, IRVar *>>> stack;
        // for build new def-use chain.
        std::unordered_map<std::string, IRVar *> ssa_def_map;
    };
} // namespace COMPILER

#endif // CVM_CFG_H
