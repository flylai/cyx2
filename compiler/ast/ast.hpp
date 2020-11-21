#ifndef CVM_AST_HPP
#define CVM_AST_HPP

#include "../token.hpp"
#include "../utility/log.h"

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
        virtual std::string toString() const    = 0;

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

    // Stmt

} // namespace COMPILER

#endif // CVM_AST_HPP
