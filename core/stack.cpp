
#include "stack.h"

void CVM::CORE::Stack::push(Type *elem)
{
    stack[position++] = elem;
}

CVM::Type *CVM::CORE::Stack::top()
{
    if (position == 0)
        return nullptr;
    else
        return stack[position - 1];
}

CVM::Type *CVM::CORE::Stack::pop()
{
    if (position == 0) throw std::out_of_range("Stack out of range! stack pos is 0!");
    Type *retval = stack[--position];
    return retval;
}

void CVM::CORE::Stack::pop(int n)
{
    if (n > position)
        throw std::invalid_argument("Stack out of range! stack pos is " + std::to_string(position) +
                                    "! but you want to pop " + std::to_string(n) + " element(s)!");
    else
        position -= n;
}

CVM::Type *&CVM::CORE::Stack::operator[](int n)
{
    if (n > position)
        throw std::out_of_range("Stack out of range! stack pos is " + std::to_string(position) + "!");
    else
        return stack[n];
}

CVM::CORE::Stack *CVM::CORE::Stack::preStack()
{
    return pre_stack;
}

void CVM::CORE::Stack::setPreStack(CVM::CORE::Stack *preStack)
{
    pre_stack = preStack;
}

int CVM::CORE::Stack::pos() const
{
    return position;
}