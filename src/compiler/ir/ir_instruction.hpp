#ifndef CVM_IR_INSTRUCTION_HPP
#define CVM_IR_INSTRUCTION_HPP

#include "../../common/config.h"
#include "../../common/value.hpp"
#include "../../utility/log.h"
#include "../../utility/utility.hpp"
#include "../token.hpp"
#include "basicblock.hpp"

#include <list>
#include <string>
#include <vector>

// part of `../token.hpp`
namespace COMPILER
{
    enum IROpcode : char
    {
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
        { IROpcode::IR_ADD, "+" },     { IROpcode::IR_SUB, "-" },       { IROpcode::IR_MUL, "*" },
        { IROpcode::IR_DIV, "/" },     { IROpcode::IR_MOD, "%" },       { IROpcode::IR_BAND, "&" },
        { IROpcode::IR_BXOR, "^" },    { IROpcode::IR_BOR, "|" },       { IROpcode::IR_EXP, "**" },
        { IROpcode::IR_SHL, "<<" },    { IROpcode::IR_SHR, ">>" },

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
            case Keyword::ADD_ASSIGN:
            case Keyword::ADD: return IROpcode::IR_ADD;
            case Keyword::SUB_ASSIGN:
            case Keyword::SUB: return IROpcode::IR_SUB;
            case Keyword::MUL_ASSIGN:
            case Keyword::MUL: return IROpcode::IR_MUL;
            case Keyword::DIV_ASSIGN:
            case Keyword::DIV: return IROpcode::IR_DIV;
            case Keyword::MOD_ASSIGN:
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

    class IR;
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
    class IRParams;
    class IRAssign;

    class IR
    {
      public:
        IR()                           = default;
        virtual ~IR()                  = default;
        virtual std::string toString() = 0;

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
            FUNC,   // function decl
            PARAM,  // function params
            BRANCH, // if then else.
            ASSIGN, // a = binary / constant / call / var / vardef
            PHI,    // phi node
            ARRAY,  // [1,a,[a,2]]
            INDEX,  // a[1], a[f()][a]
        } tag{ Tag::INVALID };
    };

    // IR conversion
    template<typename T, COMPILER::IR::Tag TAG>
    static inline T *as(COMPILER::IR *inst)
    {
        if (inst == nullptr) return nullptr;
        if (inst->tag == TAG) return static_cast<T *>(inst);
        return nullptr;
    }
    //

    class IRValue : public IR
    {
      public:
        using IR::IR;
        IRInst *belong_inst{ nullptr };
    };

    class IRInst : public IR
    {
      public:
        using IR::IR;
        COMPILER::BasicBlock *block{ nullptr };
        int id{ -1 }; // instruction ID, for compute lifetime interval in `cfg.cpp`
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
            std::string retval = value.as<std::string>() + "(";
            if (value.is<long long>())
                retval += "int";
            else if (value.is<double>())
                retval += "double";
            else
                retval += "string";
            retval += ")";
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
            return retval;
        }

      public:
        COMPILER::IROpcode opcode{ IROpcode::IR_INVALID };
        IRValue *lhs{ nullptr };
        IRValue *rhs{ nullptr };
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
            return "RETURN " + (ret ? ret->toString() : "");
        }

      public:
        IRValue *ret{ nullptr };
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
            return (id >= 0 ? std::to_string(id) + " " : "") + "jmp " + target->name;
        }

      public:
        BasicBlock *target{ nullptr };
    };

    class IRFunction : public IR
    {
      public:
        using IR::IR;
        IRFunction()
        {
            tag = IR::Tag::FUNC;
        }
        std::string toString() override
        {
            return "@" + name + "";
        }

      public:
        std::string name;
        std::vector<IRVar *> params;
        std::list<BasicBlock *> blocks;
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
            std::string retval = (func ? func->toString() : name) + "(";
            for (auto *arg : args)
            {
                retval += arg->toString() + ", ";
            }
            return retval + ")";
        }

      public:
        std::string name;
        IRFunction *func{ nullptr };
        std::vector<IR *> args;
    };

    class IRArray : public IRValue
    {
      public:
        using IRValue::IRValue;
        IRArray()
        {
            tag = IR::Tag::ARRAY;
        }
        // [IRVar,IRArray,IRCall,IRConstant]
        std::vector<IR *> content;
        std::string toString() override
        {
            std::string str = "[";
            for (auto *x : content)
            {
                str += x->toString() + ", ";
            }
            return str + "]";
        }
    };

    class IRVar : public IRValue
    {
      public:
        using IRValue::IRValue;
        IRVar()
        {
            tag = IR::Tag::VAR;
        }
        std::string ssaName()
        {
            return NO_SSA || is_ir_gen ? name : name + std::to_string(ssa_index);
        }
        std::string toString() override
        {
            std::string str = name;
            if (is_array)
            {
                for (auto x : index)
                {
                    str += "[" + x->toString() + "]";
                }
            }
            else
            {
                str += (is_ir_gen || NO_SSA ? "" : std::to_string(ssa_index));
            }
            return str;
        }
        //
        void addUse(IRVar *value)
        {
            if (std::find(use.begin(), use.end(), value) == use.end())
            {
                use.push_back(value);
            }
        }
        void killUse(IRVar *value)
        {
            use.remove(value);
        }
        bool operator<(const IRVar &var) const
        {
            return false;
        }

      public:
        bool is_array{ false };
        std::vector<IR *> index;
        //
        bool is_ir_gen{ false };
        std::string name;
        int ssa_index{ 0 };
        IRVar *def{ nullptr };
        std::list<IRVar *> use;
    };

    class IRParams : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRParams()
        {
            tag = IR::Tag::PARAM;
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

    class IRPhi : public IRValue
    {
      public:
        using IRValue::IRValue;
        IRPhi()
        {
            tag = Tag::PHI;
        }
        std::string toString() override
        {
            std::string str = "phi(";
            for (const auto &arg : args)
            {
                str += arg->toString() + " ";
            }
            return str + ")";
        }
        // var / constant
        std::vector<IRValue *> args;
    };

    class IRAssign : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRAssign()
        {
            tag = IR::Tag::ASSIGN;
        }
        void setDest(IRVar *dest)
        {
            this->_dest       = dest;
            dest->belong_inst = this;
        }
        void setSrc(IR *src)
        {
            if (inOr(src->tag, IR::Tag::BINARY, IR::Tag::VAR, IR::Tag::PHI))
            {
                auto *binary = as<IRBinary, IR::Tag::BINARY>(src);
                auto *var    = as<IRVar, IR::Tag::VAR>(src);
                auto *phi    = as<IRPhi, IR::Tag::PHI>(src);
                if (binary != nullptr)
                {
                    binary->belong_inst = this;
                    auto *lhs_const     = as<IRConstant, IR::Tag::CONST>(binary->lhs);
                    auto *lhs_var       = as<IRVar, IR::Tag::VAR>(binary->lhs);
                    auto *rhs_const     = as<IRConstant, IR::Tag::CONST>(binary->rhs);
                    auto *rhs_var       = as<IRVar, IR::Tag::VAR>(binary->rhs);
                    if (lhs_const != nullptr) lhs_const->belong_inst = this;
                    if (lhs_var != nullptr) lhs_var->belong_inst = this;
                    if (rhs_const != nullptr) rhs_const->belong_inst = this;
                    if (rhs_var != nullptr) rhs_var->belong_inst = this;
                }
                else if (var != nullptr)
                {
                    var->belong_inst = this;
                }
                else if (phi != nullptr)
                {
                    phi->belong_inst = this;
                    for (auto *v : phi->args)
                    {
                        v->belong_inst = this;
                    }
                }
            }
            _src = src;
        }
        IRVar *dest()
        {
            return _dest;
        }
        IR *src()
        {
            return _src;
        }

        std::string toString() override
        {
            std::string retval = (id >= 0 ? std::to_string(id) + " " : "");
            if (_dest != nullptr) retval += _dest->toString();
            if (_src != nullptr) retval += " = " + _src->toString();
            return retval;
        }

      private:
        IRVar *_dest{ nullptr }; // a.k.a lhs
        IR *_src{ nullptr };     // a.k.a rhs
    };

    class IRBranch : public IRInst
    {
      public:
        using IRInst::IRInst;
        IRBranch()
        {
            tag = Tag::BRANCH;
        }
        COMPILER::IRVar *cond{ nullptr };
        COMPILER::BasicBlock *true_block{ nullptr };
        COMPILER::BasicBlock *false_block{ nullptr };
        std::string toString() override
        {
            std::string retval = (id >= 0 ? std::to_string(id) + " " : "") + "if ";
            if (cond) retval += cond->toString() + " ";
            if (true_block != nullptr) retval += "then goto " + true_block->name + " ";
            if (false_block != nullptr) retval += "else goto " + false_block->name + " ";
            return retval;
        }
    };

    static bool forceRemoveVar(IRVar *var)
    {
        if (var == nullptr) return false;
        if (!var->use.empty())
        {
            // if this is vardef. set it uses's def to nullptr.
            for (auto x : var->use)
            {
                x->def = nullptr;
            }
        }
        if (var->def != nullptr)
        {
            var->def->killUse(var);
        }
        delete var;
        return true;
    };

} // namespace COMPILER

#endif // CVM_IR_INSTRUCTION_HPP