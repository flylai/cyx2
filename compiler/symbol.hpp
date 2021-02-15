#ifndef CVM_SYMBOL_HPP
#define CVM_SYMBOL_HPP

#include "ir/ir_instruction.hpp"

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

namespace COMPILER
{
    class Symbol
    {
      public:
        enum class Type
        {
            INVALID,
            FUNC,
            VAR,
        } type{ INVALID };
        COMPILER::IRFunction *func{ nullptr };
        COMPILER::IRVar *var{ nullptr };
    };

    class SymbolTable
    {
      public:
        SymbolTable() = default;
        SymbolTable(SymbolTable *pre_table)
        {
            pre = pre_table;
        }
        Symbol query(std::string name)
        {
            auto *cur_table = this;
            while (cur_table != nullptr)
            {
                if (cur_table->table.find(name) != cur_table->table.end()) return cur_table->table[name];
                cur_table = cur_table->pre;
            }
            return Symbol();
        }

        void upsert(const std::string &name, Symbol symbol)
        {
            auto *cur_table = this;
            while (cur_table != nullptr)
            {
                if (cur_table->table.find(name) != cur_table->table.end())
                {
                    cur_table = cur_table->pre;
                }
                else
                {
                    table[name] = symbol;
                    return;
                }
            }
        }

        void del(const std::string &name)
        {
            auto *cur_table = this;
            while (cur_table != nullptr)
            {
                if (auto tmp = cur_table->table.find(name); tmp != cur_table->table.end())
                {
                    delete tmp->second.func;
                    delete tmp->second.var;
                    table.erase(tmp);
                }
                else
                {
                    cur_table = cur_table->pre;
                }
            }
        }

      public:
        std::unordered_map<std::string, Symbol> table;
        SymbolTable *pre{ nullptr };
    };

} // namespace COMPILER

#endif // CVM_SYMBOL_HPP
