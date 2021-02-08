#ifndef CVM_UTILITY_HPP
#define CVM_UTILITY_HPP

#include "../compiler/ir/ir_instruction.hpp"

#include <sstream>

template<typename T, typename... U>
static inline bool inOr(T lhs, U... args)
{
    return ((lhs == args) || ...);
}

template<typename T, typename... U>
static inline bool inAnd(T lhs, U... args)
{
    return ((lhs == args) && ...);
}

template<typename T>
static inline std::string digit2HexStr(T value)
{
    std::string retval;

    std::stringstream ss;
    ss << std::hex << value;

    retval = ss.str();
    return retval;
}

static constexpr void addSpace(std::string &str, int n)
{
    for (int i = 0; i < n; i++)
    {
        str += " ";
    }
}

template<typename T, COMPILER::IR::Tag TAG>
static inline T *as(COMPILER::IR *inst)
{
    if (inst->tag == TAG) return static_cast<T *>(inst);
    return nullptr;
}

#endif // CVM_UTILITY_HPP
