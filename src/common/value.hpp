#ifndef CORE_TYPE_HPP
#define CORE_TYPE_HPP

#include "../utility/log.h"

#include <list>
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
        explicit Value(T value) : _value(value)
        {
        }
        explicit operator bool()
        {
            if (is<int>() || is<double>()) return as<double>() != 0;
            if (is<std::string>()) return as<std::string>().size() > 0;
            UNREACHABLE();
        }
        template<typename T>
        Value &operator=(const T &rhs)
        {
            _value = rhs;

            return *this;
        }
        Value &operator=(const Value &rhs)
        {
            _value = rhs._value;

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
            UNREACHABLE();
        }
        Value operator-(Value &rhs)
        {
            if (is<std::string>() || rhs.is<std::string>())
            {
                UNREACHABLE();
            }
            if (is<double>() || rhs.is<double>())
            {
                return Value(as<double>() - rhs.as<double>());
            }
            else if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() - rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator-()
        {
            if (is<double>())
            {
                _value = -as<double>();
                return *this;
            }
            else if (is<int>())
            {
                _value = -as<int>();
                return *this;
            }
            else
                UNREACHABLE();
        }
        Value operator*(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>()) // "a" * "b"
            {
                UNREACHABLE();
            }
            if ((is<std::string>() || rhs.is<std::string>()) && (is<int>() || rhs.is<int>())) // "a" * 3 || 3 * "a"
            {
                std::string ret;
                std::string str;
                int len;
                if (is<int>())
                {
                    str = rhs.as<std::string>();
                    len = as<int>();
                }
                else
                {
                    str = as<std::string>();
                    len = rhs.as<int>();
                }
                for (int i = 0; i < len; i++)
                {
                    ret += str;
                }
                return Value(ret);
            }
            if (is<double>() || rhs.is<double>()) // 3 * 3.14 || 3.14 * 3
            {
                return Value(as<double>() * rhs.as<double>());
            }
            else if (is<int>() && rhs.is<int>()) // 3 * 3
            {
                return Value(as<int>() * rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator/(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                UNREACHABLE();
            }
            if (is<double>() || rhs.is<double>()) // 3.14 * 1 || 1 * 3.14
            {
                return Value(as<double>() / rhs.as<double>());
            }
            else if (is<int>() && rhs.is<int>()) // 1 * 1
            {
                return Value(as<int>() / rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator%(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() % rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator&(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() & rhs.as<int>());
            }
            UNREACHABLE();
        }
        bool operator&&(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return as<int>() != 0 && rhs.as<int>() != 0;
            }
            UNREACHABLE();
        }
        Value operator|(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() | rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator^(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() ^ rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator>>(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() >> rhs.as<int>());
            }
            UNREACHABLE();
        }
        Value operator<<(Value &rhs)
        {
            if (is<int>() && rhs.is<int>())
            {
                return Value(as<int>() << rhs.as<int>());
            }
            UNREACHABLE();
        }
        bool operator!=(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                return as<std::string>() != rhs.as<std::string>();
            }
            else if (!is<std::string>() && !rhs.is<std::string>())
            {
                return as<double>() != rhs.as<double>();
            }
            else
                return true;
        }
        bool operator==(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                return as<std::string>() == rhs.as<std::string>();
            }
            else if (!is<std::string>() && !rhs.is<std::string>())
            {
                return as<double>() == rhs.as<double>();
            }
            else
                return true;
        }
        //
        bool operator>(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                return as<std::string>() > rhs.as<std::string>();
            }
            else if (!is<std::string>() && !rhs.is<std::string>())
            {
                return as<double>() > rhs.as<double>();
            }
            else
                return false;
        }
        bool operator<(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                return as<std::string>() < rhs.as<std::string>();
            }
            else if (!is<std::string>() && !rhs.is<std::string>())
            {
                return as<double>() < rhs.as<double>();
            }
            else
                return false;
        }
        bool operator>=(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                return as<std::string>() >= rhs.as<std::string>();
            }
            else if (!is<std::string>() && !rhs.is<std::string>())
            {
                return as<double>() >= rhs.as<double>();
            }
            else
                return false;
        }
        bool operator<=(Value &rhs)
        {
            if (is<std::string>() && rhs.is<std::string>())
            {
                return as<std::string>() <= rhs.as<std::string>();
            }
            else if (!is<std::string>() && !rhs.is<std::string>())
            {
                return as<double>() <= rhs.as<double>();
            }
            else
                return false;
        }
        //
        Value operator~()
        {
            if (is<int>())
            {
                return Value(~as<int>());
            }
            UNREACHABLE();
        }
        Value operator!()
        {
            if (is<int>())
            {
                return Value(!as<int>());
            }
            UNREACHABLE();
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
            else if (is<std::vector<Value>>())
            {
                std::string str = "[";
                auto arr        = asArray();
                for (int i = 0; i < arr->size(); i++)
                {
                    str += (*arr)[i].as<std::string>();
                    if (i != arr->size() - 1) str += ",";
                }
                return str + "]";
            }
            else
                return "";
        }

        int asInt()
        {
            if (is<double>())
            {
                return static_cast<int>(value<double>());
            }
            else if (is<std::string>())
            {
                try
                {
                    return std::stoi(as<std::string>());
                }
                catch (const std::exception &)
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        double asDouble()
        {
            if (is<int>())
            {
                return static_cast<double>(value<int>());
            }
            else if (is<std::string>())
            {
                try
                {
                    return std::stod(as<std::string>());
                }
                catch (const std::exception &)
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
            return 0;
        }
        // array specific
      public:
        Value &operator[](int n)
        {
            if (isArray())
                return asArray()->at(n);
            else if (is<std::string>())
            {
            }
            else
            {
                UNREACHABLE();
            }
        }
        bool isArray()
        {
            return is<std::vector<Value>>();
        }
        std::vector<Value> *asArray()
        {
            return valuePtr<std::vector<Value>>();
        }

      private:
        std::variant<std::monostate, int, double, std::string, std::vector<Value>> _value;
    };

} // namespace CYX

#endif
