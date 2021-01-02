#ifndef CVM_IR_INSTRUCTION_HPP
#define CVM_IR_INSTRUCTION_HPP

#include "../common/value.hpp"

#include <any>
#include <string>

// part of `../token.hpp`
namespace COMPILER
{
    enum IROpcode : char
    {
        IR_IF = 0,

        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_MOD,
        IR_BAND,
        IR_BXOR,
        IR_BOR,
        IR_EXP,
        IR_SHL,
        IR_SHR,

        IR_LAND,
        IR_LOR,

        IR_EQ,
        IR_NE,
        IR_LE,
        IR_LT,
        IR_GE,
        IR_GT,

        IR_ASSIGN,

        IR_LNOT,
        IR_BNOT,

        IR_GOTO,
        IR_LABEL,
        IR_IMPORT,
        IR_INVALID,
    };

    enum class IROperandType : char
    {
        INVALID = 0x00,
        LABEL,
        VARIABLE,
        PARAMS,
        INTEGER,
        DOUBLE,
        STRING,
    };

    struct IROpcodeStr
    {
        IROpcode opcode;
        std::string opcode_str;
    };

    const static IROpcodeStr opcode_str[27] = {
        { IROpcode::IR_IF, "if" },     { IROpcode::IR_ADD, "+" },       { IROpcode::IR_SUB, "-" },
        { IROpcode::IR_MUL, "*" },     { IROpcode::IR_DIV, "/" },       { IROpcode::IR_MOD, "%" },
        { IROpcode::IR_BAND, "&" },    { IROpcode::IR_BXOR, "^" },      { IROpcode::IR_BOR, "|" },
        { IROpcode::IR_EXP, "**" },    { IROpcode::IR_SHL, "<<" },      { IROpcode::IR_SHR, ">>" },

        { IROpcode::IR_LAND, "&&" },   { IROpcode::IR_LOR, "||" },

        { IROpcode::IR_EQ, "==" },     { IROpcode::IR_NE, "!=" },       { IROpcode::IR_LE, "<=" },
        { IROpcode::IR_LT, "<" },      { IROpcode::IR_GE, ">=" },       { IROpcode::IR_GT, ">" },

        { IROpcode::IR_ASSIGN, "=" },

        { IROpcode::IR_LNOT, "!" },    { IROpcode::IR_BNOT, "!" },

        { IROpcode::IR_GOTO, "goto" }, { IROpcode::IR_LABEL, "label" }, { IROpcode::IR_IMPORT, "import" },
        { IROpcode::IR_INVALID, "" },

    };

    class IRInstruction
    {
      public:
        IROperandType operand1_type{ IROperandType::INVALID };
        IROperandType operand2_type{ IROperandType::INVALID };
        IROpcode opcode{ IR_INVALID };
        CYX::Value operand1;
        CYX::Value operand2;
        std::string dest;
        std::string comment;

        std::string toString()
        {
            std::string retval;
            if (!dest.empty() || opcode == IROpcode::IR_ASSIGN) retval += dest + "=";
            if (opcode == IROpcode::IR_IF || opcode == IROpcode::IR_GOTO) retval += opcode_str[opcode].opcode_str + " ";
            if (operand1.hasValue())
            {
                if (operand1_type == IROperandType::PARAMS)
                {
                    //    retval += "PARAMS ";
                    //    auto params = std::any_cast<std::vector<std::string>>(operand1);
                    //    for (const auto &param : params)
                    //    {
                    //        retval += param + " ";
                    //    }
                }
                else
                {
                    retval += operand1.as<std::string>();
                }
                if (!comment.empty())
                {
                    retval += "    ;" + comment;
                }
            }

            if (operand2.hasValue())
            {
                retval += opcode_str[opcode].opcode_str;
                retval += operand2.as<std::string>();
            }
            return retval;
        }

        static inline constexpr IROpcode token2IROp(Keyword keyword)
        {
            switch (keyword)
            {
                case Keyword::ADD: return IROpcode::IR_ADD;
                case Keyword::SUB: return IROpcode::IR_SUB;
                case Keyword::MUL: return IROpcode::IR_MUL;
                case Keyword::DIV: return IROpcode::IR_DIV;
                case Keyword::MOD: return IROpcode::IR_MOD;
                case Keyword::BAND: return IROpcode::IR_BAND;
                case Keyword::BXOR: return IROpcode::IR_BXOR;
                case Keyword::BOR: return IROpcode::IR_BOR;
                case Keyword::EXP: return IROpcode::IR_EXP;
                case Keyword::SHL: return IROpcode::IR_SHL;
                case Keyword::SHR: return IROpcode::IR_SHR;

                case Keyword::LAND: return IROpcode::IR_LAND;
                case Keyword::LOR: return IROpcode::IR_LOR;

                case Keyword::EQ: return IROpcode::IR_EQ;
                case Keyword::NE: return IROpcode::IR_NE;
                case Keyword::LE: return IROpcode::IR_LE;
                case Keyword::LT: return IROpcode::IR_LT;
                case Keyword::GE: return IROpcode::IR_GE;
                case Keyword::GT: return IROpcode::IR_GT;

                case Keyword::ASSIGN: return IROpcode::IR_ASSIGN;

                case Keyword::LNOT: return IROpcode::IR_LNOT;
                case Keyword::BNOT: return IROpcode::IR_BNOT;
                default: return IROpcode::IR_INVALID;
            }
        }
    };

} // namespace COMPILER

#endif // CVM_IR_INSTRUCTION_HPP