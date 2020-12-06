#ifndef CVM_UTILITY_HPP
#define CVM_UTILITY_HPP

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

#endif // CVM_UTILITY_HPP
