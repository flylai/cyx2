#ifndef CVM_LEXER_H
#define CVM_LEXER_H

#include "token.hpp"

#include <cctype>
#include <string>
#include <unordered_map>
#include <utility>

namespace COMPILER
{
    class Lexer
    {
      public:
        explicit Lexer(std::string raw_code);
        void skipBlank();
        void advance();
        char currentChar() const;
        char peekNextChar() const;
        std::pair<int, int> position() const;
        Token nextToken();

      public:
        Token number();
        Token identifier();
        Token string();

      private:
        std::string raw_code;
        std::unordered_map<std::string, Keyword> keyword;
        char current_char{ 0 };
        int pos{ 0 };
        //
        int row{ 0 };
        int column{ 0 };
    };

} // namespace COMPILER

#endif // CVM_LEXER_H
