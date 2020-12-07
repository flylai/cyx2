#ifndef CVM_AST_VISITOR_H
#define CVM_AST_VISITOR_H

#include "ast.hpp"

namespace COMPILER
{
    class ASTVisitor
    {
      public:
        // Expr
        virtual void visitUnaryExpr(UnaryExpr *ptr)           = 0;
        virtual void visitBinaryExpr(BinaryExpr *ptr)         = 0;
        virtual void visitIntExpr(IntExpr *ptr)               = 0;
        virtual void visitDoubleExpr(DoubleExpr *ptr)         = 0;
        virtual void visitStringExpr(StringExpr *ptr)         = 0;
        virtual void visitAssignExpr(AssignExpr *ptr)         = 0;
        virtual void visitIdentifierExpr(IdentifierExpr *ptr) = 0;

        // Stmt
        virtual void visitExprStmt(ExprStmt *ptr)         = 0;
        virtual void visitIfStmt(IfStmt *ptr)             = 0;
        virtual void visitForStmt(ForStmt *ptr)           = 0;
        virtual void visitWhileStmt(WhileStmt *ptr)       = 0;
        virtual void visitSwitchStmt(SwitchStmt *ptr)     = 0;
        virtual void visitMatchStmt(MatchStmt *ptr)       = 0;
        virtual void visitFuncDeclStmt(FuncDeclStmt *ptr) = 0;
        virtual void visitBreakStmt(BreakStmt *ptr)       = 0;
        virtual void visitContinueStmt(ContinueStmt *ptr) = 0;
        virtual void visitReturnStmt(ReturnStmt *ptr)     = 0;
        virtual void visitImportStmt(ImportStmt *ptr)     = 0;
        virtual void visitBlockStmt(BlockStmt *ptr)       = 0;

        virtual void visitTree(Tree *ptr) = 0;
    };

} // namespace COMPILER

#endif // CVM_AST_VISITOR_H
