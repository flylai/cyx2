#ifndef CORE_STACK_H
#define CORE_STACK_H

#include "../common/value.hpp"
#include "vm_config.h"

#include <stdexcept>
#include <vector>

namespace CYX
{
    class Stack
    {
      public:
        Stack() = default;
        CYX::Value *top();
        CYX::Value *pop();
        void pop(int n);
        void push(CYX::Value *elem);
        CYX::Value *&operator[](int n);
        //
        int pos() const;

      private:
        int position = 0;
        std::vector<CYX::Value *> stack;
    };
} // namespace CYX

#endif