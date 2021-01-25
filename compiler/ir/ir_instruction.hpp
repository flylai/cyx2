#ifndef CVM_IR_INSTRUCTION_HPP
#define CVM_IR_INSTRUCTION_HPP

#include "../common/value.hpp"
#include "../utility/linkedlist.hpp"
#include "basicblock.hpp"

#include <list>
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
            case Keyword::SELFADD_PREFIX:
            case Keyword::SELFADD_SUFFIX: return IROpcode::IR_ADD;
            case Keyword::SELFSUB_PREFIX:
            case Keyword::SELFSUB_SUFFIX: return IROpcode::IR_SUB;

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

    class BasicBlock;
    class Use;
    class IRValue;
    class IRInst;
    class IRBinary;
    class IRConstant;
    class IRArray;
    class IRReturn;
    class IRJump;
    class IRCall;
    class IRFunction;
    class IRBranch;
    class IRPhi;
    class IRVar;
    class IRVarDef;
    class IRParams;
    class IRAssign;

    class Use
    {
      public:
        Use() = default;
        void addUse(IRVar *value)
        {
            if (std::find(uses.begin(), uses.end(), value) == uses.end())
            {
                uses.push_back(value);
            }
        }
        void killUse(IRVar *value)
        {
            uses.remove(value);
        }

      public:
        IRVarDef *def{ nullptr };
        std::list<IRVar *> uses;
    };

    class IRValue
    {
      public:
        enum class Tag
        {
            INVALID,
            CONST,  // constant included above
            BINARY, // binary expr => a+b
            RETURN, // return
            JMP,
            CALL,   // function call
            VAR,    // variable
            VARDEF, // variable definition
            FUNC,   // function decl
            PARAM,  // function params
            BRANCH, // if then else.
            ASSIGN, // a = binary / constant / call / var / vardef
            PHI,    // phi node
        } tag{ Tag::INVALID };
        virtual std::string toString() = 0;

      public:
        IRValue() = default;
    };

    class IRInst : public IRValue
    {
      public:
        using IRValue::IRValue;
        LIST(IRInst)
        COMPILER::BasicBlock *block{ nullptr };
    };

    class IRConstant : public IRValue
    {
      public:
        using IRValue::IRValue;
        IRConstant()
        {
            tag = Tag::CONST;
        }
        CYX::Value value;

        std::string toString() override
        {
            std::string retval = value.as<std::string>() + "(IRConstant[";
            if (value.is<int>())
                retval += "int";
            else if (value.is<double>())
                retval += "double";
            else
                retval += "string";
            retval += "])";
            return retval;
        }
    };

    class IRBinary : public IRValue
    {
      public:
        using IRValue::IRValue;
        IRBinary()
        {
            tag = Tag::BINARY;
        }
        std::string toString() override
        {
            std::string retval;
            if (lhs != nullptr) retval += lhs->toString();
            retval += " " + opcode_str[opcode].opcode_str + " ";
            if (rhs != nullptr) retval += rhs->toString();
            retval += "(IRBinary)";
            return retval;
        }

      public:
        COMPILER::IROpcode opcode{ IROpcode::IR_INVALID };
        IRValue *lhs{ nullptr };
        IRValue *rhs{ nullptr };
    };

    class IRBranch : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRBranch()
        {
            tag = Tag::BRANCH;
        }
        COMPILER::IRValue *cond{ nullptr };
        COMPILER::BasicBlock *true_block{ nullptr };
        COMPILER::BasicBlock *false_block{ nullptr };
        std::string toString() override
        {
            std::string retval = "if ";
            if (cond) retval += cond->toString() + " ";
            if (true_block != nullptr) retval += "then goto " + true_block->name + " ";
            if (false_block != nullptr) retval += "else goto " + false_block->name + " ";
            return retval + "(IRBranch)";
        }
    };

    class IRReturn : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRReturn()
        {
            tag = Tag::RETURN;
        }
        std::string toString() override
        {
            return "(IRReturn)";
        }

      public:
        IRInst *ret{ nullptr };
    };

    class IRJump : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRJump()
        {
            tag = Tag::JMP;
        }
        std::string toString() override
        {
            return "jmp " + target->name + "(IRJump)";
        }

      public:
        BasicBlock *target{ nullptr };
    };

    class IRCall : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRCall()
        {
            tag = Tag::CALL;
        }
        std::string toString() override
        {
            return "(IRCall)";
        }

      public:
        IRFunction *func{ nullptr };
        std::vector<IRInst *> args;
    };

    class IRFunction : public IRValue
    {
      public:
        using IRValue::IRValue;
        IRFunction()
        {
            tag = IRValue::Tag::FUNC;
        }
        std::string toString() override
        {
            return "@" + name + "(IRFunction)";
        }

      public:
        std::string name;
        std::vector<IRVarDef *> params;
        std::vector<BasicBlock *> blocks;
    };

    class IRVar : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRVar()
        {
            tag = IRValue::Tag::VAR;
        }
        std::string toString() override
        {
            return name + "(IRVar)";
        }

      public:
        std::string name;
        IRVarDef *def{ nullptr };
    };

    class IRVarDef : public IRVar
    {
      public:
        using IRVar::IRVar;
        IRVarDef()
        {
            tag = IRValue::Tag::VARDEF;
            use = new Use;
        }
        std::string toString() override
        {
            return name + "(IRVarDef)";
        }

      public:
        bool is_ir_gen{ false };
        Use *use{ nullptr };
    };

    class IRParams : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRParams()
        {
            tag = IRValue::Tag::PARAM;
        }
        std::vector<std::string> params;

      public:
        std::string toString() override
        {
            std::string retval = "PARAMS ";
            for (const auto &param : params)
            {
                retval += param + " ";
            }
            return retval;
        }
    };

    class IRAssign : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRAssign()
        {
            tag = IRValue::Tag::ASSIGN;
        }
        std::string toString() override
        {
            std::string retval;
            if (dest != nullptr) retval += dest->toString();
            if (src != nullptr) retval += " = " + src->toString();
            return retval + "(IRAssign)";
        }

      public:
        IRVar *dest{ nullptr };  // a.k.a lhs
        IRValue *src{ nullptr }; // a.k.a rhs
    };

    //    class IRPhi : public IRInst
    //    {
    //      public:
    //        using IRInst::IRInst;
    //        IRPhi()
    //        {
    //            tag = Tag::PHI;
    //        }
    //    };
} // namespace COMPILER

#endif // CVM_IR_INSTRUCTION_HPP