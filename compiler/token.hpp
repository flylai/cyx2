#ifndef CVM_TOKEN_HPP
#define CVM_TOKEN_HPP

#include <ostream>
#include <string>

namespace COMPILER
{

#define STR(X) (#X)

    enum Keyword : char
    {
        INVALID,    //
        IDENTIFIER, // variable name => a
        IF,         // if
        ELSE,       // else
        FOR,        // for
        WHILE,      // while
        SWITCH,     // switch
        BREAK,      // break
        CONTINUE,   // continue
        TRUE,       // true
        FALSE,      // false
        DEF,        // def
        RETURN,     // return
        IMPORT,     // import

        // Binary opcode

        // Arithmetic
        ADD,  // + => a + b
        SUB,  // - => a - b
        MUL,  // * => a * b
        DIV,  // / => a / b
        MOD,  // % => a % b
        BAND, // & => a & b
        BXOR, // ^ => a ^ b
        BOR,  // | => a | b
        EXP,  // ** => a ** b
        SHL,  // << => a << b
        SHR,  // >> => a >> b

        // Logic
        LAND, // && => a && b
        LOR,  // || => a || b

        // Comparison
        EQ, // == => a == b
        NE, // != => a != b
        LE, // <= => a <= b
        LT, // < => a < b
        GE, // >= => a >= b
        GT, // > => a > b

        // Other
        ASSIGN, // = => a = b

        // Unary opcode

        SELFADD, // ++ => a++ / ++a
        SELFSUB, // -- => a-- / --a
        LNOT,    // ! => !a
        BNOT,    // ~ => ~a

        // Other
        LPAREN,    // (
        RPAREN,    // )
        LBRACE,    // {
        RBRACE,    // }
        LBRACKET,  // [
        RBRACKET,  // ]
        SEMICOLON, // ;
        COLON,     // :
        SQUO,      // '
        DQUO,      // "

        INTEGER,
        DOUBLE,
        STRING,
    };
    struct Keywords
    {
        Keyword keyword;
        std::string identifier;
        std::string keyword_name;
    };

    static const Keywords keyword_table[] = {
        { INVALID, "__INVALID__", STR(INVALID) },          //
        { IDENTIFIER, "__IDENTIFIER__", STR(IDENTIFIER) }, // variable name => a
        { IF, "if", STR(IF) },                             // if
        { ELSE, "else", STR(ELSE) },                       // else
        { FOR, "for", STR(FOR) },                          // for
        { WHILE, "while", STR(WHILE) },                    // while
        { SWITCH, "switch", STR(SWITCH) },                 // switch
        { BREAK, "break", STR(BREAK) },                    // break
        { CONTINUE, "continue", STR(CONTINUE) },           // continue
        { TRUE, "true", STR(TRUE) },                       // true
        { FALSE, "false", STR(FALSE) },                    // false
        { DEF, "def", STR(DEF) },                          // def
        { RETURN, "return", STR(RETURN) },                 // return
        { IMPORT, "import", STR(IMPORT) },                 // import

        // Binary opcode

        // Arithmetic
        { ADD, "+", STR(ADD) },   // + => a + b
        { SUB, "-", STR(SUB) },   // - => a - b
        { MUL, "*", STR(MUL) },   // * => a * b
        { DIV, "/", STR(DIV) },   // / => a / b
        { MOD, "%", STR(MOD) },   // % => a % b
        { BAND, "&", STR(BAND) }, // & => a & b
        { BXOR, "^", STR(BXOR) }, // ^ => a ^ b
        { BOR, "|", STR(BOR) },   // | => a | b
        { EXP, "**", STR(EXP) },  // ** => a ** b
        { SHL, "<<", STR(SHL) },  // << => a << b
        { SHR, ">>", STR(SHR) },  // >> => a >> b

        // Logic
        { LAND, "&&", STR(LAND) }, // && => a && b
        { LOR, "||", STR(LOR) },   // || => a || b

        // Comparison
        { EQ, "==", STR(EQ) }, // == => a == b
        { NE, "!=", STR(NE) }, // != => a != b
        { LE, "<=", STR(LE) }, // <= => a <= b
        { LT, "<", STR(LT) },  // < => a < b
        { GE, ">=", STR(GE) }, // >= => a >= b
        { GT, ">", STR(GT) },  // > => a > b

        // Other
        { ASSIGN, "=", STR(ASSIGN) }, // = => a = b

        // Unary opcode

        { SELFADD, "++", STR(SELFADD) }, // ++ => a++ / ++a
        { SELFSUB, "--", STR(SELFSUB) }, // -- => a-- / --a
        { LNOT, "!", STR(LNOT) },        // ! => !a
        { BNOT, "~", STR(BNOT) },        // ~ => ~a

        // Other
        { LPAREN, "(", STR(LPAREN) },       // (
        { RPAREN, ")", STR(RPAREN) },       // )
        { LBRACE, "{", STR(LBRACE) },       // {
        { RBRACE, "}", STR(RBRACE) },       // }
        { LBRACKET, "[", STR(LBRACKET) },   // [
        { RBRACKET, "]", STR(RBRACKET) },   // ]
        { SEMICOLON, ";", STR(SEMICOLON) }, // ;
        { COLON, ":", STR(COLON) },         // :
        { SQUO, "'", STR(SQUO) },
        { DQUO, "\"", STR(SQUO) },

        { INTEGER, "__INTEGER__", STR(INTEGER) },
        { DOUBLE, "__DOUBLE__", STR(DOUBLE) },
        { STRING, "__STRING__", STR(STRING) },
    };

    class Token
    {
      public:
        Token() = default;
        Token(Keyword keyword, std::string value, int row, int column)
            : keyword(keyword), value(std::move(value)), row(row), column(column){};
        Token(Keyword keyword, int row, int column)
            : keyword(keyword), value(keyword_table[keyword].identifier), row(row), column(column){};
        Keyword keyword{ 0 };
        std::string value;
        int row{ 0 };
        int column{ 0 };

      public:
        std::string keywordName() const
        {
            return keyword_table[keyword].keyword_name;
        };
    };
} // namespace COMPILER

#endif // CVM_TOKEN_HPP
