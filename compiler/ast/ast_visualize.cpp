#include "ast_visualize.h"

#define NODE(PTR) ("node" + std::to_string(PTR->row) + "_" + std::to_string(PTR->column))

// Expr

void COMPILER::ASTVisualize::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
}

void COMPILER::ASTVisualize::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
    if (ptr == nullptr) return;

    graph += NODE(ptr) + "[label=\"BinaryExpr#" + ptr->op.keywordName() + "\"]\n";
    if (ptr->lhs != nullptr)
    {
        graph += NODE(ptr) + "->" + NODE(ptr->lhs) + "\n";
        ptr->lhs->visit(this);
    }
    if (ptr->rhs != nullptr)
    {
        graph += NODE(ptr) + "->" + NODE(ptr->rhs) + "\n";
        ptr->rhs->visit(this);
    }
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

    graph += NODE(ptr) + "[label=\"BinaryExpr#" + ptr->op.keywordName() + "\"]\n";
    graph += NODE(ptr) + "->" + NODE(ptr->lhs) + "\n";
    ptr->lhs->visit(this);
    graph += NODE(ptr) + "->" + NODE(ptr->rhs) + "\n";
    ptr->rhs->visit(this);
}

void COMPILER::ASTVisualize::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
    if (ptr == nullptr) return;
    graph += NODE(ptr) + "[label=\"IdentifierExpr#" + ptr->value + "\"]\n";
}

// Stmt

void COMPILER::ASTVisualize::visitIfStmt(COMPILER::IfStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitForStmt(COMPILER::ForStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitSwitchStmt(COMPILER::SwitchStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitMatchStmt(COMPILER::MatchStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitFuncDeclStmt(COMPILER::FuncDeclStmt *ptr)
{
    graph += NODE(ptr) + "[label=\"FuncDeclStmt#" + "\"]\n";
    graph += NODE(ptr) + "->" + NODE(ptr->block) + "\n";
    visitBlockStmt(ptr->block);
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
}

void COMPILER::ASTVisualize::visitBlockStmt(COMPILER::BlockStmt *ptr)
{
    graph += NODE(ptr) + "[label=\"BlockStmt#" + "\"]\n";

    for (auto &x : ptr->stmts)
    {
        graph += NODE(ptr) + "->" + NODE(x) + "\n";
        x->visit(this);
    }
}

void COMPILER::ASTVisualize::visitExprStmt(COMPILER::ExprStmt *ptr)
{
}

void COMPILER::ASTVisualize::visitTree(COMPILER::Tree *ptr)
{
}
