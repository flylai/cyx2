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
            if (!inst.empty()) raw_insts.push_back(inst); // TODO rewrite this
            inst.clear();
        }
    }
}

void CVM::ASM::Parser::parse()
{
    initRawCode();
    parseFunctions();

    for (parse_index = 0; parse_index < raw_insts.size(); parse_index++)
    {
        int code_pos = 0;
        raw_inst     = std::forward<std::string>(raw_insts[parse_index]);
        Instruction inst;
        inst.opcode = parseOpcode(code_pos);

        // CALL func / JIF func / JMP func
        if (inst.opcode == ASM::Opcode::CALL || //
            inst.opcode == ASM::Opcode::JIF ||  //
            inst.opcode == ASM::Opcode::JMP)
            inst.idx1 = funcs[parseString(code_pos)];

        inst.operand_target1 = parseOperandTarget(code_pos);

        // as MOV %1 2, `%` is register `$` is stack `!` is global and just parse int as index.
        if (inst.operand_target1 == OperandTarget::REGISTER || //
            inst.operand_target1 == OperandTarget::STACK ||    //
            inst.operand_target1 == OperandTarget::GLOBAL)
            inst.idx1 = parseInt(code_pos);
        else
            inst.operand1 = std::shared_ptr<Type>(parseNumber(code_pos));

        if (is_binary(inst.opcode))
        {
            inst.operand_target2 = parseOperandTarget(code_pos);

            if (inst.operand_target2 == OperandTarget::REGISTER || //
                inst.operand_target2 == OperandTarget::STACK ||    //
                inst.operand_target2 == OperandTarget::GLOBAL)
                inst.idx2 = parseInt(code_pos);
            else
                inst.operand2 = std::shared_ptr<Type>(parseNumber(code_pos));
        }

        if (inst.opcode != ASM::Opcode::UNKNOWN) insts.push_back(inst);
    }
}

void CVM::ASM::Parser::parseFunctions()
{
    for (int i = 0; i < raw_insts.size(); i++)
    {
        raw_inst = raw_insts[i];
        int cnt  = 1;
        if (raw_inst[0] == '@')
        {
            funcs.insert(std::pair(parseString(cnt), i));
        }
    }
}

int CVM::ASM::Parser::entry()
{
    return funcs["main"];
}

CVM::ASM::Opcode CVM::ASM::Parser::parseOpcode(int &start_pos)
{
    using CVM::ASM::Opcode;
    skipBlank(start_pos);

    if (raw_inst[start_pos] == '@') return Opcode::FUNC;

    std::string opcode = parseString(start_pos);

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
    if (opcode == "eq") return Opcode::EQ;
    if (opcode == "le") return Opcode::LE;
    if (opcode == "lt") return Opcode::LT;
    if (opcode == "ge") return Opcode::GE;
    if (opcode == "gt") return Opcode::GT;
    if (opcode == "jif") return Opcode::JIF;

    return CVM::ASM::Opcode::UNKNOWN;
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

int CVM::ASM::Parser::parseInt(int &start_pos)
{
    skipBlank(start_pos);
    int retval    = 0;
    bool is_minus = false;
    if (raw_inst[start_pos] == '-')
    {
        is_minus = true;
        start_pos++;
    }
    while (start_pos < raw_inst.size() && raw_inst[start_pos] >= '0' && raw_inst[start_pos] <= '9')
    {
        retval = retval * 10 + raw_inst[start_pos++] - '0';
    }
    return is_minus ? -retval : retval;
}

CVM::Type *CVM::ASM::Parser::parseNumber(int &start_pos)
{
    Type *retval  = nullptr;
    double sum    = 0.0;
    bool is_int   = true;
    bool is_minus = false;
    int dot_count = 1;
    skipBlank(start_pos);
    if (raw_inst[start_pos] == '-')
    {
        is_minus = true;
        start_pos++;
    }
    while (start_pos < raw_inst.size() && raw_inst[start_pos] >= '0' && raw_inst[start_pos] <= '9' ||
           raw_inst[start_pos] == '.')
    {
        if (raw_inst[start_pos] != '.')
        {
            if (is_int)
                sum = sum * 10 + raw_inst[start_pos] - '0';
            else
            {
                sum += (raw_inst[start_pos] - '0' + 0.0) / (dot_count * 10);
                dot_count *= 10;
            }
        }
        else
        {
            is_int = false;
        }
        start_pos++;
    }

    if (is_minus) sum = -sum;
    if (is_int)
        retval = new Type(static_cast<int>(sum));
    else
        retval = new Type(sum);

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
        retval.push_back(raw_inst[start_pos]);
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