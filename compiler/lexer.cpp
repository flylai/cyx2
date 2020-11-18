#include "lexer.h"

void COMPILER::Lexer::skipBlank()
{
    while (isspace(current_char))
        advance();
}

std::string COMPILER::Lexer::number()
{
    std::string retval;
    while (isdigit(current_char))
    {
        retval.push_back(current_char);
        advance();
    }
    return retval;
}

void COMPILER::Lexer::advance()
{
    if (pos + 1 >= raw_code.size())
        current_char = EOF;
    else
        current_char = raw_code[++pos];
}

COMPILER::Token COMPILER::Lexer::nextToken()
{
    if (current_char == '+')
    {
        advance();
        return Token(TK::ADD, "+", 0, 0);
    }
    else if (current_char == '-')
    {
        advance();
        return Token(TK::SUB, "-", 0, 0);
    }
    else if (current_char == '*')
    {
        advance();
        return Token(TK::MUL, "*", 0, 0);
    }
    else if (current_char == '/')
    {
        advance();
        return Token(TK::DIV, "/", 0, 0);
    }
    else if (isdigit(current_char))
    {
        return Token(TK::INTEGER, number(), 0, 0);
    }
    return Token(TK::INVALID, "", 0, 0);
}

char COMPILER::Lexer::currentChar() const
{
    return current_char;
}

char COMPILER::Lexer::peek() const
{
    if (pos + 1 < raw_code.size())
        return raw_code[pos + 1];
    else
        return EOF;
}
