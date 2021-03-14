#include "ast_visualize.h"

#define NODE(PTR) "node" + digit2HexStr(static_cast<void *>(PTR))
#define EDGE(FROM, TO) ((FROM) != nullptr && (TO) != nullptr) ? NODE(FROM) + "->" + NODE(TO) + "\n" : ""
#define VISIT(PTR)                                                                                                     \
    if ((PTR) != nullptr) PTR->visit(this)

// Expr

void COMPILER::ASTVisualize::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
}

void COMPILER::ASTVisualize::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
    if (ptr == nullptr) return;

    graph += NODE(ptr) + "[label=\"BinaryExpr#" + ptr->op.keywordName() + "\"]\n";
    graph += EDGE(ptr, ptr->lhs);
    graph += EDGE(ptr, ptr->rhs);
    VISIT(ptr->lhs);
    VISIT(ptr->rhs);
}

void COMPILER::ASTVisualize::visitIntExpr(COMPILER::IntExpr *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"IntExpr#" + std::to_string(ptr->value) + "\"]\n";
}

void COMPILER::ASTVisualize::visitDoubleExpr(COMPILER::DoubleExpr *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"DoubleExpr#" + std::to_string(ptr->value) + "\"]\n";
}

void COMPILER::ASTVisualize::visitStringExpr(COMPILER::StringExpr *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"StringExpr#" + ptr->value + "\"]\n";
}

void COMPILER::ASTVisualize::visitAssignExpr(COMPILER::AssignExpr *ptr)
{
    if (ptr == nullptr) return;

    graph += NODE(ptr) + "[label=\"AssignExpr#" + ptr->op.keywordName() + "\"]\n";
    graph += EDGE(ptr, ptr->lhs);
    graph += EDGE(ptr, ptr->rhs);
    VISIT(ptr->lhs);
    VISIT(ptr->rhs);
}

void COMPILER::ASTVisualize::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"IdentifierExpr#" + ptr->value + "\"]\n";
}

// Stmt

void COMPILER::ASTVisualize::visitIfStmt(COMPILER::IfStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"IfStmt#\"]\n";
    graph += EDGE(ptr, ptr->cond);
    graph += EDGE(ptr, ptr->true_block);
    graph += EDGE(ptr, ptr->false_block);
    VISIT(ptr->cond);
    VISIT(ptr->true_block);
    VISIT(ptr->false_block);
}

void COMPILER::ASTVisualize::visitForStmt(COMPILER::ForStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"ForStmt#\"]\n";
    graph += EDGE(ptr, ptr->init);
    graph += EDGE(ptr, ptr->cond);
    graph += EDGE(ptr, ptr->final);
    graph += EDGE(ptr, ptr->block);

    VISIT(ptr->init);
    VISIT(ptr->cond);
    VISIT(ptr->final);
    VISIT(ptr->block);
}

void COMPILER::ASTVisualize::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"WhileStmt#\"]\n";
    graph += EDGE(ptr, ptr->cond);
    graph += EDGE(ptr, ptr->block);
    VISIT(ptr->cond);
    VISIT(ptr->block);
}

void COMPILER::ASTVisualize::visitSwitchStmt(COMPILER::SwitchStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"SwitchStmt#\"]\n";
    graph += EDGE(ptr, ptr->cond);
    VISIT(ptr->cond);
    for (auto x : ptr->matches)
    {
        graph += EDGE(ptr, x);
        VISIT(x);
    }
}

void COMPILER::ASTVisualize::visitMatchStmt(COMPILER::MatchStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"MatchStmt#\"]\n";
    graph += EDGE(ptr, ptr->cond);
    graph += EDGE(ptr, ptr->block);
    VISIT(ptr->cond);
    VISIT(ptr->block);
}

void COMPILER::ASTVisualize::visitFuncDeclStmt(COMPILER::FuncDeclStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"FuncDeclStmt#" + ptr->func_name->value + "\"]\n";
    graph += EDGE(ptr, ptr->block);
    VISIT(ptr->block);
}

void COMPILER::ASTVisualize::visitBreakStmt(COMPILER::BreakStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitContinueStmt(COMPILER::ContinueStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitReturnStmt(COMPILER::ReturnStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitImportStmt(COMPILER::ImportStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"ImportStmt#\\n" + ptr->path + "\"]\n";
}

void COMPILER::ASTVisualize::visitBlockStmt(COMPILER::BlockStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"BlockStmt#\"]\n";

    for (auto x : ptr->stmts)
    {
        graph += EDGE(ptr, x);
        VISIT(x);
    }
}

void COMPILER::ASTVisualize::visitExprStmt(COMPILER::ExprStmt *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"ExprStmt#\"]\n";
    graph += EDGE(ptr, ptr->expr);
    VISIT(ptr->expr);
}

void COMPILER::ASTVisualize::visitTree(COMPILER::Tree *ptr)
{
    if (ptr == nullptr) return;

    graph += NODE(ptr) + "[label=\"Tree#\"]\n";

    for (auto x : ptr->stmts)
    {
        graph += EDGE(ptr, x);
        VISIT(x);
    }
}

void COMPILER::ASTVisualize::visitFuncCallExpr(COMPILER::FuncCallExpr *ptr)
{
}

std::string COMPILER::ASTVisualize::astStr()
{
    return graph;
}

#undef NODE
#undef VISIT
#undef EDGE