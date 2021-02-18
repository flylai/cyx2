#ifndef CVM_VM_INSTRUCTION_HPP
#define CVM_VM_INSTRUCTION_HPP

#include "../common/value.hpp"
#include "opcode.hpp"

#include <memory>

namespace CVM
{
    struct VMInstruction
    {
        Opcode opcode{ 0xff };
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
        std::string name;
    };

#define LOAD_INST(X, OP, TYPE)                                                                                         \
    struct X : Load                                                                                                    \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
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
        int reg_idx{ -1 };
    };

#define STORE_INST(X, OP, TYPE)                                                                                        \
    struct X : Store                                                                                                   \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
        TYPE val{};                                                                                                    \
    };

    STORE_INST(StoreI, STOREI, long long)
    STORE_INST(StoreD, STORED, double)
    STORE_INST(StoreS, STORES, std::string)

#undef STORE_INST

    struct Cmp : VMInstruction
    {
        int reg_idx1{ -1 };
        int reg_idx2{ -1 };
    };

    struct Call : VMInstruction
    {
    };

    struct Func : VMInstruction
    {
        Func()
        {
            opcode = Opcode::FUNC;
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
        std::string name;
    };

    struct Ret : VMInstruction
    {
    };

    struct Jmp : VMInstruction
    {
        int target{ -1 };
    };

    struct Jif : VMInstruction
    {
        int target1{ -1 };
        int target2{ -1 };
    };

#define CMP_INST(X, OP)                                                                                                \
    struct X : Cmp                                                                                                     \
    {                                                                                                                  \
        X()                                                                                                            \
        {                                                                                                              \
            opcode = Opcode::OP;                                                                                       \
        }                                                                                                              \
    };

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
    ARITHMETIC_INST(Lor, LOR)

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

    UNARY_INST(Land, LAND)
    UNARY_INST(Lnot, LNOT)
    UNARY_INST(Bnot, BNOT)

#undef UNARY_INST

} // namespace CVM

#endif // CVM_VM_INSTRUCTION_HPP
