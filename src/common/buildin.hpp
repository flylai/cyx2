#ifndef CYX_BUILDIN_HPP
#define CYX_BUILDIN_HPP

#include "value.hpp"

#include <iostream>
#include <string>

static void buildin_print(CYX::Value *arg)
{
    std::cout << arg->as<std::string>();
}

static void buildin_println(CYX::Value *arg)
{
    std::cout << arg->as<std::string>() << std::endl;
}

static void buildin_read(CYX::Value *target)
{
    std::string input;
    std::cin >> input;
    *target = input;
}

static void buildin_int(CYX::Value *target)
{
    *target = target->as<int>();
}

static void buildin_double(CYX::Value *target)
{
    *target = target->as<double>();
}

static void buildin_string(CYX::Value *target)
{
    *target = target->as<std::string>();
}

#define BUILDIN_NAME(IDX, X)                                                                                           \
    {                                                                                                                  \
        #X,                                                                                                            \
        {                                                                                                              \
            &X, IDX                                                                                                    \
        }                                                                                                              \
    }

#define BUILDIN_IDX(IDX, X)                                                                                            \
    {                                                                                                                  \
        IDX, &X                                                                                                        \
    }

static const std::unordered_map<std::string, std::pair<void (*)(CYX::Value *), int>> buildin_functions = {
    BUILDIN_NAME(1, buildin_print),   //
    BUILDIN_NAME(2, buildin_println), //
    BUILDIN_NAME(3, buildin_read),    //
    BUILDIN_NAME(4, buildin_int),     //
    BUILDIN_NAME(5, buildin_double),  //
    BUILDIN_NAME(6, buildin_string),  //
};

static const std::unordered_map<int, void (*)(CYX::Value *)> buildin_functions_index = {
    BUILDIN_IDX(1, buildin_print),   //
    BUILDIN_IDX(2, buildin_println), //
    BUILDIN_IDX(3, buildin_read),    //
    BUILDIN_IDX(4, buildin_int),     //
    BUILDIN_IDX(5, buildin_double),  //
    BUILDIN_IDX(6, buildin_string),  //
};

#undef BUILDIN_IDX
#undef BUILDIN_NAME

#endif // CYX_BUILDIN_H
