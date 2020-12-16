#ifndef CVM_AST_VISUALIZE_H
#define CVM_AST_VISUALIZE_H

#include "../utility/utility.hpp"
#include "ast_visitor.h"
#include "expr.hpp"
#include "stmt.hpp"

namespace COMPILER
{
    class ASTVisualize : public ASTVisitor
    {
      public:
        void visitUnaryExpr(UnaryExpr *ptr) override;
        void visitBinaryExpr(BinaryExpr *ptr) override;
        void visitIntExpr(IntExpr *ptr) override;
        void visitDoubleExpr(DoubleExpr *ptr) override;
        void visitStringExpr(StringExpr *ptr) override;
        void visitAssignExpr(AssignExpr *ptr) override;
        void visitIdentifierExpr(IdentifierExpr *ptr) override;
        void visitFuncCallExpr(FuncCallExpr *ptr) override;

        void visitExprStmt(ExprStmt *ptr) override;
        void visitTree(Tree *ptr) override;
        void visitIfStmt(IfStmt *ptr) override;
        void visitForStmt(ForStmt *ptr) override;
        void visitWhileStmt(WhileStmt *ptr) override;
        void visitSwitchStmt(SwitchStmt *ptr) override;
        void visitMatchStmt(MatchStmt *ptr) override;
        void visitFuncDeclStmt(FuncDeclStmt *ptr) override;
        void visitBreakStmt(BreakStmt *ptr) override;
        void visitContinueStmt(ContinueStmt *ptr) override;
        void visitReturnStmt(ReturnStmt *ptr) override;
        void visitImportStmt(ImportStmt *ptr) override;
        void visitBlockStmt(BlockStmt *ptr) override;

      public:
        std::string graph;
    };
} // namespace COMPILER

#endif // CVM_AST_VISUALIZE_H
