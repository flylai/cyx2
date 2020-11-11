#ifndef CORE_TYPE_HPP
#define CORE_TYPE_HPP

#include <string>
#include <utility>
#include <variant>

namespace CVM
{

    class Type
    {
      public:
        Type() = delete;
        //
        template<typename T>
        explicit Type(T value) : _value(value){};
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
        bool isSameType(const Type &that)
        {
            return _value.index() == that._value.index();
        }

      private:
        std::variant<int, double, std::string> _value;
    };

} // namespace CVM

#endif
