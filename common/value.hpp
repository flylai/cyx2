#ifndef CORE_TYPE_HPP
#define CORE_TYPE_HPP

#include "../utility/log.h"

#include <string>
#include <utility>
#include <variant>

namespace CYX
{

    class Value
    {
      public:
        Value() = default;
        //
        template<typename T>
        explicit Value(T value) : _value(value){};
        template<typename T>
        Value &operator=(const T &rhs)
        {
            _value = rhs;
            return *this;
        }
        //
        Value operator+(Value &rhs)
        {
            if (is<int>() && rhs.is<int>()) // 1 + 2
            {
                return Value(as<int>() + rhs.as<int>());
            }
            else if (!is<std::string>() && !rhs.is<std::string>() &&
                     (is<double>() || rhs.is<double>())) // 1 + 2.0 || 2.0 + 1
            {
                return Value(as<double>() + rhs.as<double>());
            }
            else if (is<std::string>() || rhs.is<std::string>()) // "a" + 1 || 1 + "a"
            {
                return Value(as<std::string>() + rhs.as<std::string>());
            }
        }
        Value operator-(Value &rhs)
        {
            if (is<std::string>() || rhs.is<std::string>())
            {
                UNREACHABLE()
            }
            if (is<double>() || rhs.is<double>())
            {
                return Value(as<double>() - rhs.as<double>());
            }
            else if (is<int> && rhs.is<int>)
            {
                return Value(as<int>() - rhs.as<int>());
            }
        }
        Value operator*(Value &rhs)
        {
            if (is<std::string> && rhs.is<std::string>) // "a" * "b"
            {
                UNREACHABLE()
            }
            if ((is<std::string>() || rhs.is<std::string>()) && (is<int>() || rhs.is<int>())) // "a" * 3 || 3 * "a"
            {
                std::string ret;
                std::string str;
                int len;
                if (is<int>)
                {
                    str = rhs.as<std::string>;
                    len = as<int>;
                }
                else
                {
                    str = as<std::string>();
                    len = as<int>;
                }
                for (int i = 0; i < len; i++)
                {
                    ret += rhs;
                }
                return str;
            }
            if (is<double>() || rhs.is<double>()) // 3 * 3.14 || 3.14 * 3
            {
                return Value(as<double>() * rhs.as<double>());
            }
            else if (is<int> && rhs.is<int>) // 3 * 3
            {
                return Value(as<int>() - rhs.as<int>());
            }
        }
        Value operator/(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                UNREACHABLE()
            }
            if (is<double>() || rhs.is<double>()) // 3.14 * 1 || 1 * 3.14
            {
                return Value(as<double>() / rhs.as<double>());
            }
            else if (is<int> && rhs.is<int>) // 1 * 1
            {
                return Value(as<int>() / rhs.as<int>());
            }
        }
        //
        template<typename T>
        bool is()
        {
            return std::holds_alternative<T>(_value);
        }
        //
        template<typename T>
        T value()
        {
            return std::get<T>(_value);
        }
        template<typename T>
        T *valuePtr()
        {
            return std::get_if<T>(&_value);
        }
        bool isSameType(const Value &that)
        {
            return _value.index() == that._value.index();
        }
        // type conversion.
        template<typename T>
        T as()
        {
            if (is<T>()) return value<T>();
            if constexpr (std::is_same<T, std::string>::value) return asString();
            if constexpr (std::is_same<T, int>::value) return asInt();
            if constexpr (std::is_same<T, double>::value) return asDouble();
        }
        bool hasValue()
        {
            return _value.index() != 0;
        }
        void reset()
        {
            _value = std::monostate();
        }

      private:
        std::string asString()
        {
            if (is<int>())
                return std::to_string(value<int>());
            else if (is<double>())
                return std::to_string(value<double>());
            else
                return "";
        }

        int asInt()
        {
            if (is<double>())
                return static_cast<int>(value<double>());
            else
                return 0;
        }
        double asDouble()
        {
            if (is<int>()) return static_cast<double>(value<int>());
            return 0;
        }

      private:
        std::variant<std::monostate, int, double, std::string> _value;
    };

} // namespace CYX

#endif
