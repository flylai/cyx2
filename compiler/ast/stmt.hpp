#ifndef CVM_STMT_HPP
#define CVM_STMT_HPP

#include "ast_visitor.h"

#include <vector>

namespace COMPILER
{
    class Tree : public AST
    {
      public:
        using AST::AST;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitTree(this);
        };
        std::vector<Stmt *> stmts;
    };

    class Stmt : public AST
    {
      public:
        using AST::AST;
        void visit(ASTVisitor *visitor) override{};
    };

    class ExprStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitExprStmt(this);
        };

      public:
        Expr *expr;
    };

    class IfStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitIfStmt(this);
        };

      public:
        Expr *cond;
        BlockStmt *true_block;  // true condition
        BlockStmt *false_block; // false condition, a.k.a else block
    };

    class ForStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitForStmt(this);
        };

      public:
        Expr *init;
        Expr *cond;
        Expr *final;
        BlockStmt *block;
    };

    class WhileStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitWhileStmt(this);
        };

      public:
        Expr *cond;
        BlockStmt *block;
    };

    class SwitchStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitSwitchStmt(this);
        };

      public:
        Expr *cond;
        std::vector<MatchStmt *> matches;
    };

    class MatchStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitMatchStmt(this);
        };

      public:
        Expr *cond;
        BlockStmt *block;
    };

    class FuncDeclStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitFuncDeclStmt(this);
        };

      public:
        IdentifierExpr *func_name;
        std::vector<std::string> params;
        BlockStmt *block;
    };

    class BreakStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitBreakStmt(this);
        };

      public:
        // TODO
    };

    class ContinueStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitContinueStmt(this);
        };

      public:
        // TODO
    };

    class ReturnStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitReturnStmt(this);
        };

      public:
        ExprStmt *retval;
    };

    class ImportStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitImportStmt(this);
        };

      public:
        std::string path;
    };

    class BlockStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitBlockStmt(this);
        };

      public:
        std::vector<Stmt *> stmts;
    };

} // namespace COMPILER

#endif // CVM_STMT_HPP
