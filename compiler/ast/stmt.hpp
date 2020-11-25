#ifndef CVM_STMT_HPP
#define CVM_STMT_HPP

#include "ast_visitor.h"

#include <vector>

namespace COMPILER
{
    class Stmt : public AST
    {
      public:
        using AST::AST;
        void visit(ASTVisitor *visitor) override{};
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
        BlockStmt *block;
    };

    class ElseStmt : public Stmt
    {
      public:
        using Stmt::Stmt;
        void visit(ASTVisitor *visitor) override
        {
            visitor->visitElseStmt(this);
        };

      public:
        BlockStmt *block;
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
        // TODO
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
        Expr *func_name;
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
        // TODO
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
        // TODO
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
        std::vector<Expr *> stmts;
    };

} // namespace COMPILER

#endif // CVM_STMT_HPP
