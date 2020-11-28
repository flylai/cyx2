#ifndef CVM_SYMBOL_HPP
#define CVM_SYMBOL_HPP

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

namespace COMPILER
{
    enum class SymbolType
    {
        INVALID,
        INT,
        DOUBLE,
        STRING,
    };
    class Symbol
    {
      public:
        Symbol() = default;
        template<typename T>
        Symbol(const std::string &name, T value)
        {
            this->name = name;
            _value     = value;
            if constexpr (std::is_same<int, T>()) type = SymbolType::INT;
            if constexpr (std::is_same<double, T>()) type = SymbolType::DOUBLE;
            if constexpr (std::is_same<std::string, T>()) type = SymbolType::STRING;
        };

        SymbolType type{ INVALID };
        std::string name;

        template<typename T>
        T value()
        {
            return std::any_cast<T>(_value);
        }

      private:
        std::any _value;
    };

    //
    //

    class SymbolTable
    {
      public:
        SymbolTable()
        {
            _pre = nullptr;
        };
        explicit SymbolTable(SymbolTable *pre) : _pre(pre), depth(pre->depth + 1){};

        std::pair<int, std::optional<Symbol>> query(const std::string &key)
        {
            SymbolTable *tmp = this;
            while (tmp != nullptr && tmp->table.find(key) == tmp->table.end())
            {
                tmp = tmp->_pre;
            }
            if (tmp->table.find(key) != tmp->table.end()) return { tmp->depth, table[key] };
            return { -1, {} };
        };

        void upsert(const std::string &key, const Symbol &symbol)
        {
            table[key] = symbol;
        };

        SymbolTable *pre() const
        {
            return _pre;
        }

      private:
        int depth{ 0 };
        std::unordered_map<std::string, Symbol> table;
        SymbolTable *_pre{ nullptr };
    };
} // namespace COMPILER

#endif // CVM_SYMBOL_HPP
