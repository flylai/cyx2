#ifndef CORE_OPCODE_H
#define CORE_OPCODE_H

namespace CVM
{
    enum class Opcode : unsigned char
    {
        // binary
        ADD = 0x30,
        SUB,
        MUL,
        DIV,
        MOD,
        EXP,
        BAND,
        BOR,
        BXOR,
        SHL,
        SHR,
        LOR,
        // CMP
        NE,
        EQ,
        LT,
        LE,
        GT,
        GE,
        LAND,
        // unary
        LNOT,
        BNOT,
        //
        LOADI,
        LOADD,
        LOADS,
        LOADA,
        LOADX,
        LOADXA,

        STOREI,
        STORED,
        STORES,
        STOREA,
        STOREX,
        CALL,
        FUNC,
        ARG,
        PARAM,
        RET,
        JMP, // unconditional jump
        JIF, // conditional jump, depend on `state`
        //

        UNKNOWN = 0xff,
    };

    static unsigned char inline constexpr opcode2UChar(Opcode opcode)
    {
        return static_cast<unsigned char>(opcode);
    }

    static Opcode inline constexpr uchar2Opcode(unsigned char x)
    {
        return static_cast<Opcode>(x);
    }

} // namespace CVM

#endif
