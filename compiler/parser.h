#ifndef CVM_PARSER_H
#define CVM_PARSER_H

#include "ast/ast_visualize.h"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "lexer.h"

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
        void parse();
        bool eat(Keyword tk);
        void eat();
        void program();

        Expr *parsePrimaryExpr();
        Expr *parseUnaryExpr();

        template<typename... U>
        COMPILER::Expr *parseBinaryExpr(Expr *lhs, int priority, U... args);
        COMPILER::Expr *parseGroupingExpr();
        COMPILER::Expr *parseExpr(int priority = 0);

        //

        COMPILER::Stmt *parseStmt();
        IfStmt *parseIfStmt();
        COMPILER::Stmt *parseElseStmt();
        BlockStmt *parseBlockStmt();
        COMPILER::Stmt *parseFuncDeclStmt();
        std::vector<std::string> parseParamListStmt();
        COMPILER::Stmt *parseForStmt();
        COMPILER::Stmt *parseWhileStmt();
        COMPILER::Stmt *parseReturnStmt();

      private:
        template<typename T, typename... U>
        bool inOr(T lhs, U... args)
        {
            return ((lhs == args) || ...);
        }

        template<typename T, typename... U>
        bool inAnd(T lhs, U... args)
        {
            return ((lhs == args) && ...);
        }
        inline constexpr int opcodePriority(Keyword keyword);

      private:
        Lexer lexer;
        Token cur_token;
        Token pre_token;
        std::unordered_map<std::string, Token> symbol;
    };
}; // namespace COMPILER

#endif // CVM_PARSER_H
