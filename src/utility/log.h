#ifndef CVM_LOG_H
#define CVM_LOG_H

#include "debug_helper.hpp"

#include <dbg.h>

#ifdef CYX_DEBUG
    #define logX(...) dbg(__VA_ARGS__)
#else
    #define logX(...) 1
#endif

#define UNREACHABLE()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        dbg("This case is unreachable!");                                                                              \
        exit(1);                                                                                                       \
    } while (false)

#define ERROR(msg)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        dbg(msg);                                                                                                      \
        exit(1);                                                                                                       \
    } while (false)

#endif // CVM_LOG_H
