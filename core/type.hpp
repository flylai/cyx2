#ifndef CORE_TYPE_HPP
#define CORE_TYPE_HPP

#include "object.hpp"

#include <utility>

namespace CVM
{
    using byte  = std::int8_t;
    using int8  = std::int8_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    class Int : public RUNTIME::Object
    {
      public:
        explicit Int() = default;
        explicit Int(int32 value) : value(value){};
        std::string toString() override
        {
            return std::to_string(value);
        };

      public:
        int value{ 0 };
    };
    //
    class Double : public RUNTIME::Object
    {
      public:
        explicit Double() = default;
        explicit Double(double value) : value(value){};
        std::string toString() override
        {
            return std::to_string(value);
        };

      public:
        double value{ 0.0 };
    };
    //
    class String : public RUNTIME::Object
    {
      public:
        explicit String() = default;
        explicit String(std::string value) : value(std::move(value)){};
        std::string toString() override
        {
            return value;
        }

      public:
        std::string value;
    };
} // namespace CVM

#endif
