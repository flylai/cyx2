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
    };

} // namespace COMPILER

#endif // CVM_AST_VISITOR_H
