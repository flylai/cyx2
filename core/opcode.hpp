#ifndef CORE_OPCODE_H
#define CORE_OPCODE_H

namespace CYX
{
    enum class OperandTarget
    {
        REGISTER = 0x02,
        STACK    = 0x04,
        GLOBAL   = 0x08,
        RAW      = 0x10,
        UNKNOWN  = 0xff
    };

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
        // CMP
        NE,
        EQ,
        LT,
        LE,
        GT,
        GE,
        // unary
        LAND,
        LOR,
        LNOT,
        BNOT,
        //
        LOADI,
        STOREI,
        LOADD,
        STORED,
        LOADS,
        STORES,
        CALL,
        RET,
        JMP, // unconditional jump
        JIF, // conditional jump, dependent on `state`

        FUNC    = 0xfe,
        UNKNOWN = 0xff
    };

    static unsigned char inline constexpr opcode2UChar(Opcode opcode)
    {
        return static_cast<unsigned char>(opcode);
    }

} // namespace CYX

#endif
