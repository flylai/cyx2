#include "lexer.h"

COMPILER::Lexer::Lexer(std::string raw_code) : raw_code(raw_code)
{
    current_char = raw_code[0];
    pos          = 0;
    keyword      = {
        { "if", Keyword::IF },             //
        { "else", Keyword::ELSE },         //
        { "for", Keyword::FOR },           //
        { "while", Keyword::WHILE },       //
        { "switch", Keyword::SWITCH },     //
        { "break", Keyword::BREAK },       //
        { "continue", Keyword::CONTINUE }, //
        { "true", Keyword::TRUE },         //
        { "false", Keyword::FALSE },       //
        { "def", Keyword::DEF },           //
        { "return", Keyword::RETURN },     //
        { "import", Keyword::IMPORT },     //
    };
}

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
    {
        if (current_char == '\n')
        {
            column = 0;
            row++;
        }
        else
        {
            column++;
        }
        current_char = raw_code[++pos];
    }
}

COMPILER::Token COMPILER::Lexer::nextToken()
{
    if (isspace(current_char)) skipBlank();
    if (current_char == '+')
    {
        advance();
        return Token(Keyword::ADD, "+", row, column);
    }
    else if (current_char == '-')
    {
        advance();
        return Token(Keyword::SUB, "-", row, column);
    }
    else if (current_char == '*')
    {
        advance();
        return Token(Keyword::MUL, "*", row, column);
    }
    else if (current_char == '/')
    {
        advance();
        return Token(Keyword::DIV, "/", row, column);
    }
    else if (isdigit(current_char))
    {
        return Token(Keyword::INTEGER, number(), row, column);
    }
    else if (current_char == '_' || isalpha(current_char))
    {
        return identity();
    }

    return Token(Keyword::INVALID, "", row, column);
}

char COMPILER::Lexer::currentChar() const
{
    return current_char;
}

char COMPILER::Lexer::peekNextChar() const
{
    if (pos + 1 < raw_code.size())
        return raw_code[pos + 1];
    else
        return EOF;
}

std::pair<int, int> COMPILER::Lexer::position() const
{
    return std::pair<int, int>(row, column);
}

COMPILER::Token COMPILER::Lexer::identity()
{
    std::string tk;

    while (isalnum(current_char) || current_char == '_')
    {
        tk.push_back(current_char);
        advance();
    }

    try
    {
        return Token(keyword.at(tk), "", row, column);
    }
    catch (const std::out_of_range &ignore)
    {
        return Token(Keyword::IDENTITY, tk, row, column);
    }
}
