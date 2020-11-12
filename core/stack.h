#ifndef CORE_STACK_H
#define CORE_STACK_H

#include "type.hpp"
#include "vm_config.h"

#include <stdexcept>

namespace CVM::CORE
{
    class Stack
    {
      public:
        Stack() = default;
        Type *top();
        Type *pop();
        void pop(int n);
        void push(Type *elem);
        Type *&operator[](int n);
        //
        int pos() const;
        Stack *preStack();
        void setPreStack(Stack *preStack);

      private:
        int position     = 0;
        Stack *pre_stack = nullptr;
        Type *stack[CVM::CONFIG::STACK_SIZE]{};
    };
} // namespace CVM::CORE

#endif