#ifndef CVM_IR_GENERATOR_H
#define CVM_IR_GENERATOR_H

#include "../../utility/utility.hpp"
#include "../ast/ast_visitor.h"
#include "../ast/expr.hpp"
#include "../ast/stmt.hpp"
#include "../symbol.hpp"
#include "ir_instruction.hpp"

#include <stack>

namespace COMPILER
{
    class IRGenerator : public ASTVisitor
    {
      public:
        IRGenerator();
        ~IRGenerator();
        void visitTree(Tree *ptr) override;
        std::string irCodeString();
        //
        void removeUnusedVarDef();
        void simplifyIR();

      private:
        //
        void visitUnaryExpr(UnaryExpr *ptr) override;
        void visitBinaryExpr(BinaryExpr *ptr) override;
        void visitIntExpr(IntExpr *ptr) override;
        void visitDoubleExpr(DoubleExpr *ptr) override;
        void visitStringExpr(StringExpr *ptr) override;
        void visitAssignExpr(AssignExpr *ptr) override;
        void visitIdentifierExpr(IdentifierExpr *ptr) override;
        void visitFuncCallExpr(FuncCallExpr *ptr) override;
        //
        void visitExprStmt(ExprStmt *ptr) override;
        void visitIfStmt(IfStmt *ptr) override;
        void visitForStmt(ForStmt *ptr) override;
        void visitWhileStmt(WhileStmt *ptr) override;
        void visitMatchStmt(MatchStmt *ptr) override;
        void visitSwitchStmt(SwitchStmt *ptr) override;
        void visitFuncDeclStmt(FuncDeclStmt *ptr) override;
        void visitBreakStmt(BreakStmt *ptr) override;
        void visitContinueStmt(ContinueStmt *ptr) override;
        void visitReturnStmt(ReturnStmt *ptr) override;
        void visitImportStmt(ImportStmt *ptr) override;
        void visitBlockStmt(BlockStmt *ptr) override;
        //
        void enterNewScope();
        void exitScope();
        //
        IRVar *newVariable();
        std::string newLabel();
        COMPILER::IRVar *consumeVariable(bool force_IRVar = true);
        BasicBlock *newBasicBlock(const std::string &name = "");
        //
        void fixContinueTarget();

      private:
        std::vector<BasicBlock *> loop_stack;
        std::vector<IRJump *> fix_continue_wait_list;

      public:
        int var_cnt{ 0 };
        int label_cnt{ 0 };
        SymbolTable global_table;
        //
        bool check_var_exist{ false };
        std::stack<IRVar *> tmp_vars;
        //
        CYX::Value cur_value;
        IRFunction *cur_func{ nullptr };
        BasicBlock *cur_basic_block{ nullptr };
        SymbolTable *cur_symbol{ nullptr };
        //
        std::vector<IRFunction *> funcs;
        BasicBlock *global_var_decl{ nullptr };

      private:
        // AST first scan.
        class HIRFunction
        {
          public:
            std::string name;
            std::vector<IRVar *> params;
            BlockStmt *block{ nullptr }; // ast body
        };

        class HIRVar
        {
          public:
            std::string name;
            Expr *rhs{ nullptr };
        };
        int step = 1;
        std::unordered_map<std::string, HIRFunction *> first_scan_funcs;
        std::unordered_map<std::string, HIRVar *> first_scan_vars;
    };

} // namespace COMPILER

#endif // CVM_IR_GENERATOR_H
