
#include "stack.h"

void CYX::Stack::push(CYX::Value *elem)
{
    stack.push_back(elem);
}

CYX::Value *CYX::Stack::top()
{
    if (stack.empty()) return nullptr;
    return stack.back();
}

CYX::Value *CYX::Stack::pop()
{
    if (stack.empty()) throw std::out_of_range("Stack out of range! stack pos is 0!");
    CYX::Value *retval = stack.back();
    stack.pop_back();
    return retval;
}

void CYX::Stack::pop(int n)
{
    if (n > stack.size())
        throw std::invalid_argument("Stack out of range! stack pos is " + std::to_string(position) +
                                    "! but you want to pop " + std::to_string(n) + " element(s)!");
    else
    {
        for (int i = 0; i < n; i++)
        {
            stack.pop_back();
        }
    }
}

CYX::Value *&CYX::Stack::operator[](int n)
{
    if (n > stack.size())
        throw std::out_of_range("Stack out of range! stack pos is " + std::to_string(position) + "!");
    else
        return stack[n];
}

int CYX::Stack::pos() const
{
    return stack.size();
}
