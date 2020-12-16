#ifndef CVM_CFG_HPP
#define CVM_CFG_HPP

#include "basicblock.hpp"

namespace COMPILER
{
    class CFG
    {
      public:
        std::vector<BasicBlock *> basicblocks;
    };
} // namespace COMPILER

#endif // CVM_CFG_HPP
