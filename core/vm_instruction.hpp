#ifndef CVM_VM_INSTRUCTION_HPP
#define CVM_VM_INSTRUCTION_HPP

#include "../common/value.hpp"
#include "opcode.hpp"

#include <memory>
#include <sstream>
#include <string>

namespace CVM
{
    struct VMInstruction
    {
        Opcode opcode{ 0xff };
        virtual std::string toString() = 0;
    };

    struct Load : VMInstruction
    {
        int reg_idx{ -1 };
    };

    struct LoadX : Load
    {
        LoadX()
        {
            opcode = Opcode::LOADX;
        }
        std::string toString() override
        {
            return "LOADX %" + std::to_string(reg_idx) + " " + name;
        }
        std::string name;
    };

#define LOAD_INST(X, OP, TYPE)                                                                                         \
    struct X : Load                                                                                                    \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
        std::string toString() override                                                                                \
        {                                                                                                              \
            std::ostringstream oss;                                                                                    \
            oss << #OP << " %";                                                                                        \
            oss << reg_idx << " " << val;                                                                              \
            return oss.str();                                                                                          \
        }                                                                                                              \
        TYPE val{};                                                                                                    \
    };

    LOAD_INST(LoadI, LOADI, long long)
    LOAD_INST(LoadD, LOADD, double)
    LOAD_INST(LoadS, LOADS, std::string)

#undef LOAD_INST

    struct Store : VMInstruction
    {
        std::string name;
    };

    struct StoreX : Store
    {
        StoreX()
        {
            opcode = Opcode::STOREX;
        }
        std::string toString() override
        {
            return "STOREX " + name + " %" + std::to_string(reg_idx);
        }
        int reg_idx{ -1 };
    };

#define STORE_INST(X, OP, TYPE)                                                                                        \
    struct X : Store                                                                                                   \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
        std::string toString() override                                                                                \
        {                                                                                                              \
            std::ostringstream oss;                                                                                    \
            oss << #OP << " " << name;                                                                                 \
            oss << " " << val;                                                                                         \
            return oss.str();                                                                                          \
        }                                                                                                              \
        TYPE val{};                                                                                                    \
    };

    STORE_INST(StoreI, STOREI, long long)
    STORE_INST(StoreD, STORED, double)
    STORE_INST(StoreS, STORES, std::string)

#undef STORE_INST

    struct Call : VMInstruction
    {
    };

    struct Func : VMInstruction
    {
        Func()
        {
            opcode = Opcode::FUNC;
        }
        std::string toString() override
        {
            return "FUNC " + name + " PARAM COUNT " + std::to_string(param_count);
        }

        std::string name;
        int param_count{ 0 };
    };

    struct Param : VMInstruction
    {
        Param()
        {
            opcode = Opcode::PARAM;
        }
        std::string toString() override
        {
            return "PARAM " + name;
        }
        std::string name;
    };

    struct Ret : VMInstruction
    {
        std::string toString() override
        {
            return "RET";
        }
    };

    struct Jmp : VMInstruction
    {
        int target{ -1 };
        std::string toString() override
        {
            return "JMP " + std::to_string(target);
        }
    };

    struct Jif : VMInstruction
    {
        int target1{ -1 };
        int target2{ -1 };
        std::string toString() override
        {
            return "JIF " + std::to_string(target1) + " " + std::to_string(target2);
        }
    };

    struct Cmp : VMInstruction
    {
        int reg_idx1{ -1 };
        int reg_idx2{ -1 };
    };

#define CMP_INST(X, OP)                                                                                                \
    struct X : Cmp                                                                                                     \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
        std::string toString() override                                                                                \
        {                                                                                                              \
            std::ostringstream oss;                                                                                    \
            oss << #OP << " %";                                                                                        \
            oss << reg_idx1 << " %" << reg_idx2;                                                                       \
            return oss.str();                                                                                          \
        }                                                                                                              \
    };

    CMP_INST(Lor, LOR)
    CMP_INST(Land, LAND)
    CMP_INST(Ge, GE)
    CMP_INST(Gt, GT)
    CMP_INST(Le, LE)
    CMP_INST(Lt, LE)
    CMP_INST(Ne, NE)
    CMP_INST(Eq, EQ)

#undef CMP_INST

    struct Binary : VMInstruction
    {
        int reg_idx1{ -1 };
        int reg_idx2{ -1 };
    };

#define ARITHMETIC_INST(X, OP)                                                                                         \
    struct X : Binary                                                                                                  \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
        std::string toString() override                                                                                \
        {                                                                                                              \
            std::ostringstream oss;                                                                                    \
            oss << #OP << " %" << reg_idx1 << " %" << reg_idx2;                                                        \
            return oss.str();                                                                                          \
        }                                                                                                              \
    };

    ARITHMETIC_INST(Add, ADD)
    ARITHMETIC_INST(Sub, SUB)
    ARITHMETIC_INST(Mul, MUL)
    ARITHMETIC_INST(Div, DIV)
    ARITHMETIC_INST(Mod, MOD)
    ARITHMETIC_INST(Exp, EXP)
    ARITHMETIC_INST(Band, BAND)
    ARITHMETIC_INST(Bor, BOR)
    ARITHMETIC_INST(Bxor, BXOR)
    ARITHMETIC_INST(Shl, SHL)
    ARITHMETIC_INST(Shr, SHR)

#undef ARITHMETIC_INST

    struct Unary : VMInstruction
    {
    };

#define UNARY_INST(X, OP)                                                                                              \
    struct X : Unary                                                                                                   \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
    };

    UNARY_INST(Lnot, LNOT)
    UNARY_INST(Bnot, BNOT)

#undef UNARY_INST

} // namespace CVM

#endif // CVM_VM_INSTRUCTION_HPP