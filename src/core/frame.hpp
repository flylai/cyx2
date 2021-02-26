#ifndef CVM_FRAME_HPP
#define CVM_FRAME_HPP

#include "../common/value.hpp"

#include <unordered_map>

namespace CVM
{
    class Frame
    {
      public:
        std::unordered_map<std::string, CYX::Value> symbols;
        int pc{ -1 };
    };
} // namespace CVM

#endif // CVM_FRAME_HPP
