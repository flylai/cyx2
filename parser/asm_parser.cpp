#include "asm_parser.h"

void CVM::ASM::Parser::initRawCode()
{
    std::string inst;
    for (auto x : raw_code)
    {
        if (x != '\n')
        {
            inst.push_back(x);
        }
        else
        {
            raw_insts.push_back(inst);
            inst.clear();
        }
    }
}

void CVM::ASM::Parser::parse()
{
    initRawCode();

    for (parse_index = 0; parse_index < raw_insts.size(); parse_index++)
    {
        int code_pos = 0;
        raw_inst     = std::forward<std::string>(raw_insts[parse_index]);
        Instruction inst;
        inst.opcode = parseOpcode(code_pos);

        if (inst.opcode == CORE::Opcode::CALL)
        {
            inst.idx1 = funcs[parseString(code_pos)];
            return;
        }

        inst.operand_target1 = parseOperandTarget(code_pos);
        if (inst.operand_target1 != OperandTarget::UNKNOWN && inst.operand_target1 != OperandTarget::RAW ||
            inst.opcode == CORE::Opcode::JIF)
            inst.idx1 = parseNumber(code_pos);
        else
            inst.operand1 = std::make_shared<Type>(parseNumber(code_pos));

        inst.operand_target2 = parseOperandTarget(code_pos);
        if (inst.operand_target2 != OperandTarget::UNKNOWN && inst.operand_target2 != OperandTarget::RAW)
            inst.idx2 = parseNumber(code_pos);
        else
            inst.operand2 = std::make_shared<Type>(parseNumber(code_pos));

        insts.push_back(inst);
    }
}

CVM::CORE::Opcode CVM::ASM::Parser::parseOpcode(int &start_pos)
{
    skipBlank(start_pos);

    bool is_func = false;

    if (raw_inst[start_pos] == '@')
    {
        is_func = true;
        start_pos++;
    }

    std::string opcode = parseString(start_pos);

    if (is_func)
    {
        funcs.insert(std::pair(opcode, parse_index));
        return CORE::Opcode::UNKNOWN;
    }

    using CVM::CORE::Opcode;
    if (opcode == "mov") return Opcode::MOV;
    if (opcode == "add") return Opcode::ADD;
    if (opcode == "sub") return Opcode::SUB;
    if (opcode == "mul") return Opcode::MUL;
    if (opcode == "div") return Opcode::DIV;
    if (opcode == "push") return Opcode::PUSH;
    if (opcode == "pop") return Opcode::POP;
    if (opcode == "call") return Opcode::CALL;
    if (opcode == "ret") return Opcode::RET;
    if (opcode == "jmp") return Opcode::JMP;
    if (opcode == "ne") return Opcode::NE;
    if (opcode == "jif") return Opcode::JIF;

    return CVM::CORE::Opcode::UNKNOWN;
}

CVM::ASM::OperandTarget CVM::ASM::Parser::parseOperandTarget(int &start_pos)
{
    skipBlank(start_pos);

    auto retval = OperandTarget::UNKNOWN;

    if (raw_inst[start_pos] == '%')
        retval = OperandTarget::REGISTER;
    else if (raw_inst[start_pos] == '$')
        retval = OperandTarget::STACK;
    else if (raw_inst[start_pos] == '!')
        retval = OperandTarget::GLOBAL;
    else
        retval = OperandTarget::RAW;

    if (retval != OperandTarget::RAW) start_pos++;

    return retval;
}

int CVM::ASM::Parser::parseNumber(int &start_pos)
{
    skipBlank(start_pos);
    int retval = 0;
    while (start_pos < raw_inst.size() && raw_inst[start_pos] >= '0' && raw_inst[start_pos] <= '9')
    {
        retval = retval * 10 + raw_inst[start_pos++] - '0';
    }
    return retval;
}

std::string CVM::ASM::Parser::parseString(int &start_pos)
{
    skipBlank(start_pos);
    std::string retval;
    for (; start_pos < raw_inst.size()     //
           && raw_inst[start_pos] != ' '   //
           && raw_inst[start_pos] != '\n'  //
           && raw_inst[start_pos] != '\t'; //
         start_pos++)
        retval.push_back(raw_inst[start_pos++]);
    return retval;
}

void CVM::ASM::Parser::skipBlank(int &start_pos)
{
    while (start_pos < raw_inst.size() && (raw_inst[start_pos] == ' ' ||  //
                                           raw_inst[start_pos] == '\t' || //
                                           raw_inst[start_pos] == '\n'))
        start_pos++;
}

void CVM::ASM::Parser::setRawCode(const std::string &code)
{
    raw_code = code;
}

const std::vector<CVM::ASM::Instruction> &CVM::ASM::Parser::instructions() const
{
    return insts;
}