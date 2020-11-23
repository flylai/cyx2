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

COMPILER::Token COMPILER::Lexer::number()
{
    std::string retval;
    bool is_double = false;
    while (isdigit(current_char) || (!is_double && current_char == '.'))
    {
        if (current_char == '.') is_double = true;
        retval.push_back(current_char);
        advance();
    }
    return Token(is_double ? Keyword::DOUBLE : Keyword::INTEGER, retval, row, column);
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
#define MK_TOKEN(KW) Token((KW), row, column)
#define MK_TOKEN2(KW, ID) Token((KW), ID, row, column)
#define CASE_TOKEN(ID, KW)                                                                                             \
    case ID: advance(); return MK_TOKEN(KW)

    if (isspace(current_char)) skipBlank();

    switch (current_char)
    {
        CASE_TOKEN('+', Keyword::ADD);
        CASE_TOKEN('-', Keyword::SUB);
        CASE_TOKEN('/', Keyword::DIV);
        CASE_TOKEN('%', Keyword::MOD);
        CASE_TOKEN('(', Keyword::LPAREN);
        CASE_TOKEN(')', Keyword::RPAREN);
        CASE_TOKEN('{', Keyword::LBRACKET);
        CASE_TOKEN('}', Keyword::RBRACKET);
        CASE_TOKEN('^', Keyword::BXOR);
        CASE_TOKEN('~', Keyword::BNOT);
        CASE_TOKEN('!', Keyword::LNOT);
        CASE_TOKEN(';', Keyword::SEMICOLON);
        CASE_TOKEN(':', Keyword::COLON);
        case '\'':
        case '"': return string();
        case '&':
            advance();
            if (peekNextChar() == '&')
            {
                advance();
                return MK_TOKEN2(Keyword::LAND, "&&");
            }
            return MK_TOKEN2(Keyword::BAND, "&");
        case '|':
            advance();
            if (peekNextChar() == '|')
            {
                advance();
                return MK_TOKEN2(Keyword::LOR, "||");
            }
            return MK_TOKEN2(Keyword::BOR, "|");
        case '*':
            advance();
            if (peekNextChar() == '*')
            {
                advance();
                return MK_TOKEN2(Keyword::EXP, "**");
            }
            return MK_TOKEN2(Keyword::MUL, "*");
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_': return identifier();
        case '0' ... '9': return number();
        case '=':
            advance();
            if (peekNextChar() == '=')
            {
                advance();
                return MK_TOKEN2(Keyword::EQ, "==");
            }
            return MK_TOKEN2(Keyword::ASSIGN, "=");
        case '<':
            advance();
            if (peekNextChar() == '=')
            {
                advance();
                return MK_TOKEN2(Keyword::LE, "<=");
            }
            else if (peekNextChar() == '<')
            {
                advance();
                return MK_TOKEN2(Keyword::SHL, "<<");
            }
            return MK_TOKEN2(Keyword::LT, "<");
        case '>':
            advance();
            if (peekNextChar() == '=')
            {
                advance();
                return MK_TOKEN2(Keyword::GE, ">=");
            }
            else if (peekNextChar() == '>')
            {
                advance();
                return MK_TOKEN2(Keyword::SHR, ">>");
            }
            return MK_TOKEN2(Keyword::GT, ">");
        default: return Token(Keyword::INVALID, "", -1, -1);
    }
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

COMPILER::Token COMPILER::Lexer::identifier()
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
        return Token(Keyword::IDENTIFIER, tk, row, column);
    }
}

COMPILER::Token COMPILER::Lexer::string()
{
    std::string retval;
    char target = '\'';
    if (current_char == '"') target = '"';
    advance();
    while (current_char != target)
    {
        retval.push_back(current_char);
        advance();
    }
    advance();

    return COMPILER::Token(Keyword::STRING, retval, row, column);
}
