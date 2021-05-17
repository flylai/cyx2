#ifndef CVM_LOG_H
#define CVM_LOG_H

#include <dbg.h>
#include <iostream>

#ifdef CYX_DEBUG
    #define LOGD(...) dbg(__VA_ARGS__)
    #define CERR(ARG) LOGE(ARG)
#else
    #define LOGD(...) 1
    #define CERR(ARG)                                                                                                  \
        do                                                                                                             \
        {                                                                                                              \
            std::cerr << (ARG) << std::endl;                                                                           \
            exit(1);                                                                                                   \
        } while (false)
#endif

#define UNREACHABLE()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        dbg("This case is unreachable!");                                                                              \
        exit(1);                                                                                                       \
    } while (false)

#define LOGE(msg)                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        dbg(msg);                                                                                                      \
        exit(1);                                                                                                       \
    } while (false)

#endif // CVM_LOG_H
