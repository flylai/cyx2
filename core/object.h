#ifndef CORE_OBJECT_H
#define CORE_OBJECT_H

#include "global.h"

namespace CVM::RUNTIME
{
    class Object
    {
      public:
        Object()                       = default;
        virtual ~Object()              = default;
        virtual std::string toString() = 0;
    };
} // namespace CVM::RUNTIME

#endif
