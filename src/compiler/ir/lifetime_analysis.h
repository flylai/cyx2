#ifndef CVM_LIFETIME_ANALYSIS_H
#define CVM_LIFETIME_ANALYSIS_H

#include "../../utility/utility.hpp"
#include "ir_instruction.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace COMPILER
{
    struct hash
    {
        std::size_t operator()(const IRVar *rhs) const
        {
            return std::hash<std::string>()(rhs->name) ^ std::hash<int>()(rhs->ssa_index);
        }
    };
    struct eq
    {
        bool operator()(const IRVar *lhs, const IRVar *rhs) const
        {
            return lhs->name == rhs->name && lhs->ssa_index == rhs->ssa_index;
        }
    };

    class LifetimeInterval
    {
      public:
        void addRange(int from, int to)
        {
            if (interval.empty())
            {
                interval.emplace_back(from, to);
                return;
            }
            auto &back = interval.back();
            if (to == back.first)
            {
                back.first  = std::min(back.first, from);
                back.second = std::max(back.second, to);
            }
            else
                interval.emplace_back(from, to);
        }
        void setFrom(int from)
        {
            if (interval.empty())
                interval.emplace_back(from, from + 2);
            else
                interval.back().first = from;
        }

      public:
        std::vector<std::pair<int, int>> interval;
    };
    class LifetimeAnalysis
    {
      public:
        void computeGlobalLiveSet() const;
        void debug();
        void buildIntervals();
        void numberingOperations();
        std::string intervalsStr();

      private:
        std::string getName(IRVar *var);

      public:
        IRFunction *function{ nullptr };
        std::unordered_map<std::string, LifetimeInterval> intervals;
    };
} // namespace COMPILER
#endif // CVM_LIFETIME_ANALYSIS_H
