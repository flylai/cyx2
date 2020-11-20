#ifndef CVM_TOKEN_HPP
#define CVM_TOKEN_HPP

#include <ostream>
#include <string>

namespace COMPILER
{

    enum Keyword : char
    {
        INVALID,  //
        IDENTITY, // variable name => a
        IF,       // if
        ELSE,     // else
        FOR,      // for
        WHILE,    // while
        SWITCH,   // switch
        BREAK,    // break
        CONTINUE, // continue
        TRUE,     // true
        FALSE,    // false
        DEF,      // def
        RETURN,   // return
        IMPORT,   // import

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

        INTEGER,
        DOUBLE,
        STRING,
    };

#define STR(X) (#X)

    class Token
    {
      public:
        Token() = default;
        Token(Keyword keyword, std::string value, int row, int column)
            : keyword(keyword), value(std::move(value)), row(row), column(column){};
        Keyword keyword{ 0 };
        std::string value;
        int row{ 0 };
        int column{ 0 };

      public:
        std::string keywordName() const
        {
            const char *keywords[] = {
                STR(INVALID),  //
                STR(IDENTITY), // variable name => a
                STR(IF),       // if
                STR(ELSE),     // else
                STR(FOR),      // for
                STR(WHILE),    // while
                STR(SWITCH),   // switch
                STR(BREAK),    // break
                STR(CONTINUE), // continue
                STR(TRUE),     // true
                STR(FALSE),    // false
                STR(DEF),      // def
                STR(RETURN),   // return
                STR(IMPORT),   // import

                // Binary opcode

                // Arithmetic
                STR(ADD),  // + => a + b
                STR(SUB),  // - => a - b
                STR(MUL),  // * => a * b
                STR(DIV),  // / => a / b
                STR(MOD),  // % => a % b
                STR(BAND), // & => a & b
                STR(BXOR), // ^ => a ^ b
                STR(BOR),  // | => a | b

                // Logic
                STR(LAND), // && => a && b
                STR(LOR),  // || => a || b

                // Comparison
                STR(EQ), // == => a == b
                STR(NE), // != => a != b
                STR(LE), // <= => a <= b
                STR(LT), // < => a < b
                STR(GE), // >= => a >= b
                STR(GT), // > => a > b

                // Other
                STR(ASSIGN), // = => a = b

                // Unary opcode

                STR(SELFADD), // ++ => a++ / ++a
                STR(SELFSUB), // -- => a-- / --a
                STR(LNOT),    // ! => !a
                STR(BNOT),    // ~ => ~a

                // Other
                STR(LPAREN),    // (
                STR(RPAREN),    // )
                STR(LBRACE),    // {
                STR(RBRACE),    // }
                STR(LBRACKET),  // [
                STR(RBRACKET),  // ]
                STR(SEMICOLON), // ;
                STR(COLON),     // :

                STR(INTEGER), //
                STR(DOUBLE),  //
                STR(STRING),  //
            };
            return keywords[keyword];
        };
    };
} // namespace COMPILER

#endif // CVM_TOKEN_HPP
