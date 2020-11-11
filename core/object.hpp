#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

#include <iostream>
#include <string>

namespace CVM
{
    enum class Type
    {
        INT     = 0x00,
        DOUBLE  = 0x01,
        STRING  = 0x02,
        UNKNOWN = 0xff
    };

    class Object
    {
      public:
        Object()          = default;
        virtual ~Object() = default;
        virtual Type type()
        {
            return Type::UNKNOWN;
        }
        virtual std::string toString() = 0;
    };

} // namespace CVM::RUNTIME

#endif
