#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

#include <iostream>
#include <string>

namespace CVM::RUNTIME
{
    class Object
    {
      public:
        Object()          = default;
        virtual ~Object() = default;
        virtual std::string toString()
        {
            return "";
        };
    };

} // namespace CVM::RUNTIME

#endif
