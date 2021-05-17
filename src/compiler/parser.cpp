#include "parser.h"

#include "ir/cfg.h"

COMPILER::Tree *COMPILER::Parser::parse()
{
    return program();
}

bool COMPILER::Parser::eat(COMPILER::Keyword tk)
{
    if (cur_token.keyword == tk)
    {
        eat();
        return true;
    }
    CERR("expected " + keyword_table[tk].identifier);
}

void COMPILER::Parser::eat()
{
    pre_token = cur_token;
    cur_token = lexer.nextToken();
}

bool COMPILER::Parser::optional(COMPILER::Keyword tk)
{
    if (cur_token.keyword == tk)
    {
        eat();
        return true;
    }
    return false;
}

COMPILER::Tree *COMPILER::Parser::program()
{
    auto *ast = new Tree();
    while (cur_token.keyword != Keyword::INVALID)
    {
        ast->stmts.push_back(parseStmt());
    }
    return ast;
}

COMPILER::Expr *COMPILER::Parser::parsePrimaryExpr()
{
    if (cur_token.keyword == Keyword::IDENTIFIER)
    {
        auto is_func_decl = false;
        if (pre_token.keyword == Keyword::DEF) is_func_decl = true;
        eat(Keyword::IDENTIFIER);
        if (!is_func_decl && cur_token.keyword == Keyword::LPAREN)
        {
            auto func_call_expr       = new FuncCallExpr(pre_token.row, pre_token.column);
            func_call_expr->func_name = pre_token.value;
            func_call_expr->args      = std::move(parseArgListStmt());
            return func_call_expr;
        }
        else if (cur_token.keyword == Keyword::LBRACKET)
        {
            return parseArrayIdExpr();
        }
        else if (inOr(cur_token.keyword, Keyword::SELFADD, Keyword::SELFSUB))
        {
            // suffix unary expr
            // a++, b--
            auto unary_expr = new UnaryExpr(pre_token.row, pre_token.column);
            Keyword keyword = cur_token.keyword == Keyword::SELFSUB ? Keyword::SELFSUB_SUFFIX : Keyword::SELFADD_SUFFIX;
            auto identifier_expr   = new IdentifierExpr(pre_token.row, pre_token.column);
            identifier_expr->value = pre_token.value;
            unary_expr->op         = Token(keyword, pre_token.row, pre_token.column);
            unary_expr->rhs        = identifier_expr;
            eat();
            return unary_expr;
        }
        auto *identifier_expr  = new IdentifierExpr(pre_token.row, pre_token.column);
        identifier_expr->value = pre_token.value;
        return identifier_expr;
    }
    else if (cur_token.keyword == Keyword::INTEGER)
    {
        auto *int_expr  = new IntExpr(cur_token.row, cur_token.column);
        int_expr->value = std::stoll(cur_token.value);
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
    else if (cur_token.keyword == Keyword::LPAREN)
    {
        return parseGroupingExpr();
    }
    return nullptr;
}

COMPILER::Expr *COMPILER::Parser::parseUnaryExpr()
{
    if (inOr(cur_token.keyword, Keyword::SUB, Keyword::LNOT, Keyword::BNOT, //
             Keyword::SELFADD, Keyword::SELFSUB))
    {
        auto *unary_expr = new UnaryExpr(cur_token.row, cur_token.column);
        if (inOr(cur_token.keyword, Keyword::SELFADD, Keyword::SELFSUB))
        {
            Keyword keyword = cur_token.keyword == Keyword::SELFSUB ? Keyword::SELFSUB_PREFIX : Keyword::SELFADD_PREFIX;
            unary_expr->op  = Token(keyword, cur_token.row, cur_token.column);
        }
        else
        {
            unary_expr->op = cur_token;
        }
        eat();
        unary_expr->rhs = parseUnaryExpr();
        return unary_expr;
    }
    else if (inOr(cur_token.keyword, Keyword::IDENTIFIER, Keyword::INTEGER, Keyword::DOUBLE, Keyword::STRING))
        return parsePrimaryExpr();
    else if (cur_token.keyword == Keyword::LPAREN)
        return parsePrimaryExpr();

    return nullptr;
}

template<typename... U>
COMPILER::Expr *COMPILER::Parser::parseBinaryExpr(COMPILER::Expr *lhs, int priority, U... args)
{
    while (inOr(cur_token.keyword, args...))
    {
        if (cur_token.keyword == Keyword::LPAREN)
        {
            return parseGroupingExpr();
        }
        if (priority > opcodePriority(cur_token.keyword)) return lhs;
        auto *binary_expr = cur_token.keyword == Keyword::ASSIGN ? new AssignExpr(pre_token.row, pre_token.column) :
                            new BinaryExpr(pre_token.row, pre_token.column);
        eat();
        binary_expr->lhs = lhs;
        binary_expr->op  = pre_token;
        binary_expr->rhs = parseExpr(opcodePriority(pre_token.keyword) + 1);
        lhs              = binary_expr;
    }
    return lhs;
}

COMPILER::Expr *COMPILER::Parser::parseExpr(int priority)
{
    auto *lhs = parseUnaryExpr();

    if (cur_token.keyword == Keyword::LBRACKET)
    {
        return parseArrayExpr();
    }

    return parseBinaryExpr(lhs, priority,

                           Keyword::ADD, Keyword::SUB, Keyword::MUL, Keyword::DIV, Keyword::MOD, Keyword::BAND,
                           Keyword::BXOR, Keyword::BOR, Keyword::EXP, Keyword::SHL, Keyword::SHR, Keyword::LAND,
                           Keyword::LOR,

                           Keyword::EQ, Keyword::NE, Keyword::LE, Keyword::LT, Keyword::GE, Keyword::GT,

                           Keyword::ASSIGN, Keyword::ADD_ASSIGN, Keyword::SUB_ASSIGN, Keyword::MUL_ASSIGN,
                           Keyword::SUB_ASSIGN, Keyword::MOD_ASSIGN, Keyword::DIV_ASSIGN

    );
}

COMPILER::ArrayExpr *COMPILER::Parser::parseArrayExpr()
{
    eat(Keyword::LBRACKET);
    auto *array_expr = new ArrayExpr;
    while (true)
    {
        auto *idx = parseStmt();
        if (idx != nullptr) array_expr->content.push_back(idx);
        if (cur_token.keyword == Keyword::COMMA)
            eat(Keyword::COMMA);
        else
            break;
    }
    eat(Keyword::RBRACKET);
    return array_expr;
}

COMPILER::ArrayIdExpr *COMPILER::Parser::parseArrayIdExpr()
{
    auto arr_id_expr  = new ArrayIdExpr;
    arr_id_expr->name = pre_token.value;
    while (cur_token.keyword == Keyword::LBRACKET)
    {
        eat(Keyword::LBRACKET);
        arr_id_expr->index.push_back(parseStmt());
        eat(Keyword::RBRACKET);
    }
    return arr_id_expr;
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
    eat(Keyword::LPAREN);
    auto *retval = parseExpr();
    if (!eat(Keyword::RPAREN)) CERR("expected `)`");
    return retval;
}

COMPILER::Stmt *COMPILER::Parser::parseStmt()
{
    Stmt *retval{ nullptr };
    switch (cur_token.keyword)
    {
        case Keyword::DEF: retval = parseFuncDeclStmt(); break;
        case Keyword::IF: retval = parseIfStmt(); break;
        case Keyword::FOR: retval = parseForStmt(); break;
        case Keyword::WHILE: retval = parseWhileStmt(); break;
        case Keyword::IMPORT: retval = parseImportStmt(); break;
        case Keyword::RETURN: retval = parseReturnStmt(); break;
        case Keyword::BREAK: retval = parseBreakStmt(); break;
        case Keyword::CONTINUE: retval = parseContinueStmt(); break;
        case Keyword::SWITCH: retval = parseSwitchStmt(); break;
        default: retval = parseExprStmt();
    }
    optional(Keyword::SEMICOLON);
    return retval;
}

COMPILER::ExprStmt *COMPILER::Parser::parseExprStmt()
{
    auto *expr_stmt = new ExprStmt(cur_token.row, cur_token.column);
    expr_stmt->expr = parseExpr();
    return expr_stmt->expr == nullptr ? nullptr : expr_stmt;
}

COMPILER::IfStmt *COMPILER::Parser::parseIfStmt()
{
    auto *if_stmt = new IfStmt(cur_token.row, cur_token.column);
    eat(Keyword::IF);
    eat(Keyword::LPAREN);
    if_stmt->cond = parseExpr();
    eat(Keyword::RPAREN);
    if_stmt->true_block = parseBlockStmt();
    if (cur_token.keyword == Keyword::ELSE)
    {
        if_stmt->false_block = parseElseStmt();
    }

    return if_stmt;
}

COMPILER::BlockStmt *COMPILER::Parser::parseElseStmt()
{
    eat(Keyword::ELSE);
    auto *else_stmt = parseBlockStmt();
    return else_stmt;
}

COMPILER::BlockStmt *COMPILER::Parser::parseBlockStmt()
{
    auto *block = new BlockStmt(cur_token.row, cur_token.column);
    eat(Keyword::LBRACE);
    std::vector<Stmt *> stmts;

    while (cur_token.keyword != Keyword::RBRACE)
    {
        stmts.push_back(parseStmt());
    }

    block->stmts = std::move(stmts);

    eat(Keyword::RBRACE);
    return block;
}

COMPILER::Stmt *COMPILER::Parser::parseFuncDeclStmt()
{
    auto *func = new FuncDeclStmt(cur_token.row, cur_token.column);
    eat(Keyword::DEF);

    func->func_name = dynamic_cast<IdentifierExpr *>(parsePrimaryExpr());
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

std::vector<COMPILER::Stmt *> COMPILER::Parser::parseArgListStmt()
{
    std::vector<Stmt *> args;
    eat(Keyword::LPAREN);
    while (cur_token.keyword != Keyword::RPAREN)
    {
        if (cur_token.keyword == Keyword::COMMA)
            eat();
        else
            args.push_back(parseExprStmt());
    }
    eat(Keyword::RPAREN);
    return args;
}

COMPILER::Stmt *COMPILER::Parser::parseForStmt()
{
    auto *for_stmt = new ForStmt(cur_token.row, cur_token.column);
    eat(Keyword::FOR);
    eat(Keyword::LPAREN);

    for_stmt->init = parseExpr();
    eat(Keyword::SEMICOLON);
    for_stmt->cond = parseExpr();
    eat(Keyword::SEMICOLON);
    for_stmt->final = parseExpr();

    eat(Keyword::RPAREN);
    // TODO: optional `{` `}`
    for_stmt->block = parseBlockStmt();
    return for_stmt;
}

COMPILER::SwitchStmt *COMPILER::Parser::parseSwitchStmt()
{
    /*  switch()
     *  {
     *      () => {},
     *      () => {},
     *      () => {},
     *  }
     *
     */
    auto *switch_stmt = new SwitchStmt(cur_token.row, cur_token.column);
    eat(Keyword::SWITCH);
    eat(Keyword::LPAREN);
    switch_stmt->cond = parsePrimaryExpr();
    eat(Keyword::RPAREN);

    eat(Keyword::LBRACE);

    std::vector<MatchStmt *> matches;

    while (cur_token.keyword != Keyword::RBRACE)
    {
        auto *match_stmt = new MatchStmt(cur_token.row, cur_token.column);
        // (expr)
        eat(Keyword::LPAREN);
        match_stmt->cond = parsePrimaryExpr();
        eat(Keyword::RPAREN);
        // =>
        eat(Keyword::POINT_TO);
        // { stmts }
        match_stmt->block = parseBlockStmt();
        matches.push_back(match_stmt);

        if (cur_token.keyword != Keyword::COMMA)
            break;
        else
            eat(Keyword::COMMA);
    }

    eat(Keyword::RBRACE);

    switch_stmt->matches = std::move(matches);
    return switch_stmt;
}

COMPILER::Stmt *COMPILER::Parser::parseWhileStmt()
{
    auto *while_stmt = new WhileStmt(cur_token.row, cur_token.column);
    eat(Keyword::WHILE);
    eat(Keyword::LPAREN);
    while_stmt->cond = parseExpr();
    eat(Keyword::RPAREN);
    while_stmt->block = parseBlockStmt();
    return while_stmt;
}

COMPILER::Stmt *COMPILER::Parser::parseReturnStmt()
{
    auto *return_stmt   = new ReturnStmt(cur_token.row, cur_token.column);
    eat(Keyword::RETURN);
    return_stmt->retval = parseExprStmt();
    return return_stmt;
}

COMPILER::Stmt *COMPILER::Parser::parseBreakStmt()
{
    auto *break_stmt = new BreakStmt(cur_token.row, cur_token.column);
    eat(Keyword::BREAK);
    return break_stmt;
}

COMPILER::Stmt *COMPILER::Parser::parseContinueStmt()
{
    auto *break_stmt = new ContinueStmt(cur_token.row, cur_token.column);
    eat(Keyword::CONTINUE);
    return break_stmt;
}

COMPILER::Stmt *COMPILER::Parser::parseImportStmt()
{
    eat(Keyword::IMPORT);
    auto *import_stmt = new ImportStmt(cur_token.row, cur_token.column);
    import_stmt->path = dynamic_cast<StringExpr *>(parsePrimaryExpr())->value;
    return import_stmt;
}
