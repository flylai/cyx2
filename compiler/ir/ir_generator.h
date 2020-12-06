#ifndef CVM_IR_GENERATOR_H
#define CVM_IR_GENERATOR_H

#include "../ast/ast_visitor.h"
#include "../ast/expr.hpp"
#include "../ast/stmt.hpp"
#include "../symbol.hpp"
#include "ir_instruction.hpp"

namespace COMPILER
{
    class IRGenerator : public ASTVisitor
    {
      public:
        IRGenerator();
        //
        void visitUnaryExpr(UnaryExpr *ptr) override;
        void visitBinaryExpr(BinaryExpr *ptr) override;
        void visitIntExpr(IntExpr *ptr) override;
        void visitDoubleExpr(DoubleExpr *ptr) override;
        void visitStringExpr(StringExpr *ptr) override;
        void visitAssignExpr(AssignExpr *ptr) override;
        void visitIdentifierExpr(IdentifierExpr *ptr) override;
        //
        void visitExprStmt(ExprStmt *ptr) override;
        void visitIfStmt(IfStmt *ptr) override;
        void visitElseStmt(ElseStmt *ptr) override;
        void visitForStmt(ForStmt *ptr) override;
        void visitWhileStmt(WhileStmt *ptr) override;
        void visitSwitchStmt(SwitchStmt *ptr) override;
        void visitFuncDeclStmt(FuncDeclStmt *ptr) override;
        void visitBreakStmt(BreakStmt *ptr) override;
        void visitContinueStmt(ContinueStmt *ptr) override;
        void visitReturnStmt(ReturnStmt *ptr) override;
        void visitImportStmt(ImportStmt *ptr) override;
        void visitBlockStmt(BlockStmt *ptr) override;
        //

        void genExpr(COMPILER::Expr *ptr, const std::string &var_name);
        void genStmt(COMPILER::Stmt *ptr);
        void genCond(COMPILER::Expr *ptr, const std::string &true_cond, const std::string &false_cond);

        //
        void enterNewScope();
        void exitScope();
        //
        std::string newVariable();
        std::string newLabel();
        //
        std::string irCodeString();

      public:
        int var_cnt{ 0 };
        int label_cnt{ 0 };
        SymbolTable *current_symbol{ nullptr };
        std::vector<IRInstruction> instructions;
    };

} // namespace COMPILER

#endif // CVM_IR_GENERATOR_H
