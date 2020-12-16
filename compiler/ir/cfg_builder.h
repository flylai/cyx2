#ifndef CVM_CFG_BUILDER_H
#define CVM_CFG_BUILDER_H

#include "../../utility/utility.hpp"
#include "../ast/ast_visitor.h"
#include "../ast/expr.hpp"
#include "../ast/stmt.hpp"
#include "basicblock.hpp"

#include <unordered_map>

namespace COMPILER
{
    class CFGBuilder : public ASTVisitor
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
        void visitTree(Tree *ptr) override;

        //
        void dfs(BasicBlock *block);

      public:
        std::string graph;

      private:
        BasicBlock *newBasicBlock(const std::string &name = "", bool force = false);

      private:
        BasicBlock *cur_bb{ nullptr };
        BasicBlock *entry{ nullptr };
        std::unordered_map<BasicBlock *, bool> vis;
    };
} // namespace COMPILER

#endif // CVM_CFG_BUILDER_H
