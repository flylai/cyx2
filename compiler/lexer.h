#ifndef CVM_LEXER_H
#define CVM_LEXER_H

#include "token.hpp"

#include <cctype>
#include <string>
#include <utility>

namespace COMPILER
{
    class Lexer
    {
      public:
        explicit Lexer(std::string raw_code) : raw_code(raw_code)
        {
            current_char = raw_code[0];
            pos          = 0;
        };

        void skipBlank();
        std::string number();
        void advance();
        Token nextToken();
        char currentChar() const;
        char peek() const;

      private:
        std::string raw_code;
        char current_char{ 0 };
        int pos{ 0 };
    };

} // namespace COMPILER

#endif // CVM_LEXER_H
