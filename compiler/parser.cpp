#include "parser.h"

void COMPILER::Parser::parse()
{
    program();
}

bool COMPILER::Parser::eat(COMPILER::Keyword tk)
{
    if (cur_token.keyword == tk)
    {
        eat();
        return true;
    }
    return false;
}

void COMPILER::Parser::eat()
{
    pre_token = cur_token;
    cur_token = lexer.nextToken();
    log(pre_token);
}

void COMPILER::Parser::program()
{
    // import
    // fundecl
    // vardecl
    //
    auto *x = parseFuncDeclStmt();

    //    Expr *x  = parseExpr();
    auto *xx = new ASTVisualize;
    x->visit(xx);
    log(xx->graph);
}

COMPILER::Expr *COMPILER::Parser::parsePrimaryExpr()
{
    if (cur_token.keyword == Keyword::IDENTIFIER)
    {
        auto *identity_expr  = new IdentifierExpr(cur_token.row, cur_token.column);
        identity_expr->value = cur_token.value;
        eat(Keyword::IDENTIFIER);
        return identity_expr;
    }
    else if (cur_token.keyword == Keyword::INTEGER)
    {
        auto *int_expr  = new IntExpr(cur_token.row, cur_token.column);
        int_expr->value = std::stoi(cur_token.value);
        eat(Keyword::INTEGER);
        return int_expr;
    }
    else if (cur_token.keyword == Keyword::DOUBLE)
    {
        auto *double_expr  = new DoubleExpr(cur_token.row, cur_token.column);
        double_expr->value = std::stod(cur_token.value);
        eat(Keyword::DOUBLE);
        return double_expr;
    }
    else if (cur_token.keyword == Keyword::STRING)
    {
        auto *string_expr  = new StringExpr(cur_token.row, cur_token.column);
        string_expr->value = cur_token.value;
        eat(Keyword::STRING);
        return string_expr;
    }
    return nullptr;
}

COMPILER::Expr *COMPILER::Parser::parseUnaryExpr()
{
    if (inOr(cur_token.keyword, Keyword::SUB, Keyword::LNOT, Keyword::BNOT))
    {
        auto *unary_expr = new UnaryExpr(cur_token.row, cur_token.column);
        unary_expr->op   = cur_token;
        unary_expr->rhs  = parseUnaryExpr();
        eat();
        return unary_expr;
    }
    else if (inOr(cur_token.keyword, Keyword::IDENTIFIER, Keyword::INTEGER, Keyword::DOUBLE, Keyword::STRING))
        return parsePrimaryExpr();

    return nullptr;
}

template<typename... U>
COMPILER::Expr *COMPILER::Parser::parseBinaryExpr(COMPILER::Expr *lhs, int priority, U... args)
{
    while (inOr(cur_token.keyword, args...))
    {
        if (priority > opcodePriority(cur_token.keyword)) return lhs;
        eat();
        auto *binary_expr = new BinaryExpr(pre_token.row, pre_token.column);
        binary_expr->lhs  = lhs;
        binary_expr->op   = pre_token;
        binary_expr->rhs  = parseExpr(opcodePriority(pre_token.keyword));
        lhs               = binary_expr;
    }
    return lhs;
}

COMPILER::Expr *COMPILER::Parser::parseExpr(int priority)
{
    auto *lhs = parseUnaryExpr();

    if (cur_token.keyword == Keyword::LPAREN)
    {
        eat(Keyword::LPAREN);
        return parseGroupingExpr();
    }

    return parseBinaryExpr(lhs, priority,

                           Keyword::ADD, Keyword::SUB, Keyword::MUL, Keyword::DIV, Keyword::MOD, Keyword::BAND,
                           Keyword::BXOR, Keyword::BOR, Keyword::EXP, Keyword::SHL, Keyword::SHR, Keyword::LAND,
                           Keyword::LOR,

                           Keyword::EQ, Keyword::NE, Keyword::LE, Keyword::LT, Keyword::GE, Keyword::GT,

                           Keyword::ASSIGN

    );
}

inline constexpr int COMPILER::Parser::opcodePriority(COMPILER::Keyword keyword)
{
    // copy from
    // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
    switch (keyword)
    {
        case Keyword::LPAREN:
        case Keyword::RPAREN: return 21;
        case Keyword::LNOT:
        case Keyword::BNOT: return 17;
        case Keyword::EXP: return 16;
        case Keyword::MUL:
        case Keyword::DIV:
        case Keyword::MOD: return 15;
        case Keyword::ADD:
        case Keyword::SUB: return 14;
        case Keyword::SHL:
        case Keyword::SHR: return 13;
        case Keyword::LT:
        case Keyword::LE:
        case Keyword::GT:
        case Keyword::GE: return 12;
        case Keyword::EQ:
        case Keyword::NE: return 11;
        case Keyword::BAND: return 10;
        case Keyword::BXOR: return 9;
        case Keyword::BOR: return 8;
        case Keyword::LAND: return 7;
        case Keyword::LOR: return 6;
        case Keyword::ASSIGN: return 3;
        default: return 0;
    }
}

COMPILER::Expr *COMPILER::Parser::parseGroupingExpr()
{
    auto *retval = parseExpr();
    if (!eat(Keyword::RPAREN)) log("should not reach here");
    return retval;
}

COMPILER::Stmt *COMPILER::Parser::parseStmt()
{
    switch (cur_token.keyword)
    {
        case Keyword::DEF: return parseFuncDeclStmt();
        default: return nullptr;
    }
}

COMPILER::IfStmt *COMPILER::Parser::parseIfStmt()
{
    return nullptr;
}

COMPILER::Stmt *COMPILER::Parser::parseElseStmt()
{
    return nullptr;
}

COMPILER::BlockStmt *COMPILER::Parser::parseBlockStmt()
{
    auto *block = new BlockStmt(cur_token.row, cur_token.column);
    eat(Keyword::LBRACKET);
    std::vector<Expr *> stmts;

    while (cur_token.keyword != Keyword::RBRACKET)
    {
        stmts.push_back(parseExpr());
    }

    block->stmts = std::move(stmts);

    eat(Keyword::RBRACKET);
    return block;
}

COMPILER::Stmt *COMPILER::Parser::parseFuncDeclStmt()
{
    auto *func = new FuncDeclStmt(cur_token.row, cur_token.column);
    eat(Keyword::DEF);

    func->func_name = parsePrimaryExpr();
    func->params    = parseParamListStmt();
    func->block     = parseBlockStmt();

    return func;
}

std::vector<std::string> COMPILER::Parser::parseParamListStmt()
{
    eat(Keyword::LPAREN);

    std::vector<std::string> params;

    while (inOr(cur_token.keyword, Keyword::COMMA, Keyword::IDENTIFIER))
    {
        if (cur_token.keyword == Keyword::IDENTIFIER) params.push_back(cur_token.value);
        eat();
    }

    eat(Keyword::RPAREN);
    return params;
}

COMPILER::Stmt *COMPILER::Parser::parseForStmt()
{
    return nullptr;
}

COMPILER::Stmt *COMPILER::Parser::parseWhileStmt()
{
    return nullptr;
}

COMPILER::Stmt *COMPILER::Parser::parseReturnStmt()
{
    return nullptr;
}