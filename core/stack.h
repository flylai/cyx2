#ifndef CORE_STACK_H
#define CORE_STACK_H

#include "../common/value.hpp"
#include "vm_config.h"

#include <stdexcept>

namespace CYX::CORE
{
    class Stack
    {
      public:
        Stack() = default;
        Value *top();
        Value *pop();
        void pop(int n);
        void push(Value *elem);
        Value *&operator[](int n);
        //
        int pos() const;
        Stack *preStack();
        void setPreStack(Stack *preStack);

      private:
        int position     = 0;
        Stack *pre_stack = nullptr;
        Value *stack[CYX::CONFIG::STACK_SIZE]{};
    };
} // namespace CYX::CORE

#endif