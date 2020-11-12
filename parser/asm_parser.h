#ifndef CVM_ASM_PARSER_H
#define CVM_ASM_PARSER_H

#include "instruction.hpp"
#include "opcode.hpp"
#include "type.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace CVM::ASM
{
    class Parser
    {
      public:
        void parse();
        void setRawCode(const std::string &code);
        const std::vector<Instruction> &instructions() const;

      private:
        void initRawCode();
        CORE::Opcode parseOpcode(int &start_pos);
        OperandTarget parseOperandTarget(int &start_pos);
        int parseNumber(int &start_pos);
        std::string parseString(int &start_pos);
        void skipBlank(int &start_pos);

      private:
        std::string raw_code;
        std::vector<std::string> raw_insts;
        std::vector<Instruction> insts;
        std::unordered_map<std::string, int> funcs;

      private:
        int parse_index{ 0 };
        std::string raw_inst;
    };
} // namespace CVM::ASM

#endif // CVM_ASM_PARSER_H
