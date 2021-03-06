#ifndef CVM_PARSER_H
#define CVM_PARSER_H

#include "../utility/utility.hpp"
#include "ast/ast_visualize.h"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "ir/ir_generator.h"
#include "lexer.h"

#include <exception>
#include <unordered_map>
#include <utility>

namespace COMPILER
{
    class Parser
    {
      public:
        explicit Parser(std::string code) : lexer(std::move(code))
        {
            cur_token = lexer.nextToken();
            pre_token = Token(Keyword::INVALID, "", -1, -1);
        };
        Tree *parse();
        bool eat(Keyword tk);
        bool optional(Keyword tk);
        void eat();
        Tree *program();

        Expr *parsePrimaryExpr();
        Expr *parseUnaryExpr();
        ArrayExpr *parseArrayExpr();
        ArrayIdExpr *parseArrayIdExpr();

        template<typename... U>
        COMPILER::Expr *parseBinaryExpr(Expr *lhs, int priority, U... args);
        COMPILER::Expr *parseGroupingExpr();
        COMPILER::Expr *parseExpr(int priority = 0);

        //

        COMPILER::Stmt *parseStmt();
        COMPILER::ExprStmt *parseExprStmt();
        COMPILER::IfStmt *parseIfStmt();
        BlockStmt *parseElseStmt();
        BlockStmt *parseBlockStmt();
        COMPILER::Stmt *parseFuncDeclStmt();
        std::vector<std::string> parseParamListStmt();
        std::vector<COMPILER::Stmt *> parseArgListStmt();
        COMPILER::Stmt *parseForStmt();
        COMPILER::Stmt *parseWhileStmt();
        COMPILER::Stmt *parseReturnStmt();
        COMPILER::Stmt *parseBreakStmt();
        COMPILER::Stmt *parseContinueStmt();
        COMPILER::Stmt *parseImportStmt();
        SwitchStmt *parseSwitchStmt();

      private:
        inline constexpr int opcodePriority(Keyword keyword);

      private:
        Lexer lexer;
        Token cur_token;
        Token pre_token;
        std::unordered_map<std::string, Token> symbol;
    };
}; // namespace COMPILER

#endif // CVM_PARSER_H
