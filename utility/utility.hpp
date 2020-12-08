#ifndef CVM_UTILITY_HPP
#define CVM_UTILITY_HPP

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

#endif // CVM_UTILITY_HPP
