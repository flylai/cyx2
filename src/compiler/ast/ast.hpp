#ifndef CVM_AST_HPP
#define CVM_AST_HPP

#include "../../utility/log.h"
#include "../token.hpp"

#include <string>
#include <vector>

namespace COMPILER
{
    class ASTVisitor;
    class AST
    {
      public:
        explicit AST(int row, int column) : row(row), column(column){};
        AST()  = default;
        ~AST() = default;

        virtual void visit(ASTVisitor *visitor) = 0;

      public:
        int row{ 0 };
        int column{ 0 };
    };

    //
    //
    // AST DECLARE-----------------------------------
    //
    //

    // Expr
    class Expr;
    class UnaryExpr;
    class BinaryExpr;
    class IntExpr;
    class DoubleExpr;
    class StringExpr;
    class AssignExpr;
    class IdentifierExpr;
    class FuncCallExpr;
    class ArrayExpr;
    class ArrayIdExpr;

    // Stmt
    class Stmt;
    class ExprStmt;
    class IfStmt;
    class ForStmt;
    class WhileStmt;
    class SwitchStmt;
    class MatchStmt;
    class FuncDeclStmt;
    class BreakStmt;
    class ContinueStmt;
    class ReturnStmt;
    class ImportStmt;
    class BlockStmt;

    class Tree;

} // namespace COMPILER

#endif // CVM_AST_HPP
