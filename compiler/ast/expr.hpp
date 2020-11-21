#ifndef CVM_EXPR_HPP
#define CVM_EXPR_HPP

#include "ast_visitor.h"

namespace COMPILER
{
    class Expr : public AST
    {
      public:
        using AST::AST;
    };

    class IntExpr : public Expr
    {
      public:
        using Expr::Expr;
        int value;

        void visit(ASTVisitor *visitor) override
        {
            visitor->visitIntExpr(this);
        };
        std::string toString() const override{};
    };

    class DoubleExpr : public Expr
    {
      public:
        using Expr::Expr;
        double value;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitDoubleExpr(this);
        };
        std::string toString() const override{};
    };

    class IdentifierExpr : public Expr
    {
      public:
        using Expr ::Expr;
        std::string value;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitIdentifierExpr(this);
        };
        std::string toString() const override{};
    };

    class StringExpr : public Expr
    {
      public:
        using Expr::Expr;
        std::string value;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitStringExpr(this);
        };
        std::string toString() const override{};
    };

    class BinaryExpr : public Expr
    {
      public:
        using Expr::Expr;
        Expr *lhs;
        Token op;
        Expr *rhs;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitBinaryExpr(this);
        };
        std::string toString() const override{};
    };
    class UnaryExpr : public Expr
    {
      public:
        using Expr::Expr;
        Token op;
        Expr *rhs;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitUnaryExpr(this);
        };
        std::string toString() const override{};
    };

    class AssignExpr : public BinaryExpr
    {
      public:
        using BinaryExpr::BinaryExpr;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitAssignExpr(this);
        };
        std::string toString() const override{};
    };
} // namespace COMPILER

#endif // CVM_EXPR_HPP
