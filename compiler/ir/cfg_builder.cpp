#include "cfg_builder.h"

#define LINK(PRE, SUCC)                                                                                                \
    if (PRE != nullptr && SUCC != nullptr && PRE != SUCC)                                                              \
    {                                                                                                                  \
        PRE->addSucc(SUCC);                                                                                            \
        SUCC->addPre(PRE);                                                                                             \
    }

void COMPILER::CFGBuilder::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
    cur_bb->addInst(ptr);
}

void COMPILER::CFGBuilder::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
    cur_bb->addInst(ptr);
}

void COMPILER::CFGBuilder::visitIntExpr(COMPILER::IntExpr *ptr)
{
    cur_bb->addInst(ptr);
}

void COMPILER::CFGBuilder::visitDoubleExpr(COMPILER::DoubleExpr *ptr)
{
    cur_bb->addInst(ptr);
}

void COMPILER::CFGBuilder::visitStringExpr(COMPILER::StringExpr *ptr)
{
    cur_bb->addInst(ptr);
}

void COMPILER::CFGBuilder::visitAssignExpr(COMPILER::AssignExpr *ptr)
{
    cur_bb->addInst(ptr);
}

void COMPILER::CFGBuilder::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
    cur_bb->addInst(ptr);
}

/////////////////////////// Stmt start /////////////////////////////

void COMPILER::CFGBuilder::visitExprStmt(COMPILER::ExprStmt *ptr)
{
    cur_bb->addInst(ptr->expr);
}

void COMPILER::CFGBuilder::visitIfStmt(COMPILER::IfStmt *ptr)
{
    // +-----+     +------------+     +-----------+     +-----+
    // | bb1 | --> |    cond    | --> | if_block | --> | bb2 |
    // +-----+     +------------+     +----------+     +-----+
    //                  |                                 ^
    //                  |                                 |
    //                  v                                 |
    //             +------------+                         |
    //             | else_block | ------------------------+
    //             +------------+

    auto *cond_block = newBasicBlock("if_cond", true);
    LINK(cur_bb, cond_block)
    cur_bb = cond_block;
    ptr->cond->visit(this);

    auto *true_block = newBasicBlock("if_block", true);
    LINK(cond_block, true_block)
    cur_bb = true_block;
    ptr->true_block->visit(this);

    BasicBlock *false_block = nullptr;
    if (ptr->false_block != nullptr)
    {
        false_block = newBasicBlock("else_block", true);
        LINK(cond_block, false_block)
        cur_bb = false_block;
        ptr->false_block->visit(this);
    }

    cur_bb = newBasicBlock("", true);
    LINK(false_block, cur_bb)
    LINK(true_block, cur_bb)
}

void COMPILER::CFGBuilder::visitForStmt(COMPILER::ForStmt *ptr)
{
    //                  +-----------------------+
    //                  v                       |
    //    +-----+     +----------+     +-----+  |
    //    | bb1 | --> | for_decl | --> | bb2 |  |
    //    +-----+     +----------+     +-----+  |
    //                  |                       |
    //                  |                       |
    //                  v                       |
    //                +----------+              |
    //                |  block   | -------------+
    //                +----------+

    auto *for_decl = newBasicBlock("for_decl");
    LINK(cur_bb, for_decl)
    cur_bb = for_decl;
    cur_bb->addInst(ptr->init, ptr->cond, ptr->final);

    auto *block = newBasicBlock("block");

    LINK(for_decl, block)
    LINK(block, for_decl)

    cur_bb = block;
    ptr->block->visit(this);

    cur_bb = newBasicBlock();
    LINK(for_decl, cur_bb)
}

void COMPILER::CFGBuilder::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
    // similar to `for`

    auto *cond = newBasicBlock("while_cond");
    LINK(cur_bb, cond)
    cur_bb = cond;
    ptr->cond->visit(this);
    cur_bb->addInst(ptr->cond);

    auto *block = newBasicBlock("while_block");
    LINK(cond, block)
    LINK(block, cond)
    cur_bb = block;
    ptr->block->visit(this);

    cur_bb = newBasicBlock();
    LINK(cond, cur_bb)
}

void COMPILER::CFGBuilder::visitSwitchStmt(COMPILER::SwitchStmt *ptr)
{

    //                  +---------+
    //                  | target3 | ------------------+
    //                  +---------+                   |
    //                    ^                           |
    //                    |                           |
    //                    |                           v
    //  +---------+     +---------+     +-----+     +--------+
    //  |   bb1   | --> |         | --> | ... | --> |        |
    //  +---------+     |         |     +-----+     |        |
    //                  |         |                 |        |
    //                  |  cond   | --------------> |  bb2   |
    //                  |         |                 |        |
    //  +---------+     |         |                 |        |
    //  | target2 | <-- |         |                 |        |
    //  +---------+     +---------+                 +--------+
    //    |               |                           ^    ^
    //    |               |                           |    |
    //    |               v                           |    |
    //    |             +---------+                   |    |
    //    |             | target1 | ------------------+    |
    //    |             +---------+                        |
    //    |                                                |
    //    +------------------------------------------------+

    auto *cond = newBasicBlock("switch_cond");
    LINK(cur_bb, cond)
    cur_bb = cond;
    ptr->cond->visit(this);

    // the basicblock after switch stmt
    auto *exit_block = newBasicBlock("switch_exit"); // aka `bb2`
    LINK(cond, exit_block)

    int target_count = 0;
    for (auto x : ptr->matches)
    {
        auto *target = newBasicBlock("target" + std::to_string(target_count));
        cur_bb       = target;
        LINK(cond, target)
        x->cond->visit(this);

        auto *block = newBasicBlock("block" + std::to_string(target_count++), true);
        LINK(target, block)
        cur_bb = block;
        x->block->visit(this);
        LINK(cur_bb, exit_block)

        cur_bb = cond;
    }
}

void COMPILER::CFGBuilder::visitMatchStmt(COMPILER::MatchStmt *ptr)
{
    // pass
}

void COMPILER::CFGBuilder::visitFuncDeclStmt(COMPILER::FuncDeclStmt *ptr)
{
    auto *func_decl = newBasicBlock("func_decl" + ptr->func_name->value, true);
    LINK(cur_bb, func_decl)
    cur_bb = func_decl;
    ptr->block->visit(this);
}

void COMPILER::CFGBuilder::visitBreakStmt(COMPILER::BreakStmt *ptr)
{
}

void COMPILER::CFGBuilder::visitContinueStmt(COMPILER::ContinueStmt *ptr)
{
}

void COMPILER::CFGBuilder::visitReturnStmt(COMPILER::ReturnStmt *ptr)
{
}

void COMPILER::CFGBuilder::visitImportStmt(COMPILER::ImportStmt *ptr)
{
}

void COMPILER::CFGBuilder::visitBlockStmt(COMPILER::BlockStmt *ptr)
{
    for (auto x : ptr->stmts)
    {
        x->visit(this);
    }
}

void COMPILER::CFGBuilder::visitTree(COMPILER::Tree *ptr)
{
    // ENTRY!
    entry  = newBasicBlock("Entry");
    cur_bb = newBasicBlock("", true);
    LINK(entry, cur_bb)

    for (auto x : ptr->stmts)
    {
        if (typeid(*x) == typeid(ExprStmt) || typeid(*x) == typeid(ImportStmt) || dynamic_cast<Expr *>(x))
        {
            // TODO ImportStmt
            if (typeid(*x) == typeid(ExprStmt))
                cur_bb->addInst(dynamic_cast<ExprStmt *>(x)->expr);
            else
                cur_bb->addInst(dynamic_cast<Expr *>(x));
            continue;
        }
        auto next_bb = newBasicBlock();
        LINK(cur_bb, next_bb)
        cur_bb = next_bb;
        x->visit(this);
    }

    dfs(entry);
}

COMPILER::BasicBlock *COMPILER::CFGBuilder::newBasicBlock(const std::string &name, bool force)
{
    if (!force && cur_bb != nullptr && cur_bb->insts().empty())
    {
        if (!name.empty()) cur_bb->name = name;
        return cur_bb;
    }
    return name.empty() ? new BasicBlock() : new BasicBlock(name);
}

#define NODE(PTR) PTR->name + std::to_string(PTR->block_index)
// digit2HexStr(static_cast<void *>(PTR))
#define EDGE(FROM, TO) ((FROM) != nullptr && (TO) != nullptr) ? NODE(FROM) + "->" + NODE(TO) + "\n" : ""

void COMPILER::CFGBuilder::dfs(BasicBlock *block)
{
    if (vis.find(block) != vis.end()) return;
    vis[block] = true;
    for (auto x : block->succs())
    {
        if (vis.find(x) == vis.end())
        {
            dfs(x);
        }
        graph += EDGE(block, x);
    }
}

void COMPILER::CFGBuilder::visitFuncCallExpr(COMPILER::FuncCallExpr *ptr)
{
}