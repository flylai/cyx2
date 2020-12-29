
#include "stack.h"

void CYX::CORE::Stack::push(Value *elem)
{
    stack[position++] = elem;
}

CYX::Value *CYX::CORE::Stack::top()
{
    if (position == 0)
        return nullptr;
    else
        return stack[position - 1];
}

CYX::Value *CYX::CORE::Stack::pop()
{
    if (position == 0) throw std::out_of_range("Stack out of range! stack pos is 0!");
    Value *retval = stack[--position];
    return retval;
}

void CYX::CORE::Stack::pop(int n)
{
    if (n > position)
        throw std::invalid_argument("Stack out of range! stack pos is " + std::to_string(position) +
                                    "! but you want to pop " + std::to_string(n) + " element(s)!");
    else
        position -= n;
}

CYX::Value *&CYX::CORE::Stack::operator[](int n)
{
    if (n > position)
        throw std::out_of_range("Stack out of range! stack pos is " + std::to_string(position) + "!");
    else
        return stack[n];
}

CYX::CORE::Stack *CYX::CORE::Stack::preStack()
{
    return pre_stack;
}

void CYX::CORE::Stack::setPreStack(CYX::CORE::Stack *preStack)
{
    pre_stack = preStack;
}

int CYX::CORE::Stack::pos() const
{
    return position;
}
