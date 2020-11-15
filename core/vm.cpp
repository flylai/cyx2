#include "vm.hpp"

#include <utility>

void CVM::ASM::VM::run()
{
    while (pc < 28)
    {
        CVM::ASM::Instruction inst = code[pc];

        switch (inst.opcode)
        {
            case Opcode::MOV: mov(inst); break;

            case Opcode::ADD: arithmetic<Opcode::ADD>(inst); break;
            case Opcode::SUB: arithmetic<Opcode::SUB>(inst); break;
            case Opcode::MUL: arithmetic<Opcode::MUL>(inst); break;
            case Opcode::DIV: arithmetic<Opcode::DIV>(inst); break;

            case Opcode::PUSH: push(inst); break;
            case Opcode::POP: pop(inst); break;
            case Opcode::CALL: call(inst); break;
            case Opcode::RET: ret(inst); break;
            case Opcode::JMP: jmp(inst); break;

            case Opcode::NE: comparison<Opcode::NE>(inst); break;
            case Opcode::EQ: comparison<Opcode::EQ>(inst); break;
            case Opcode::LE: comparison<Opcode::LE>(inst); break;
            case Opcode::LT: comparison<Opcode::LT>(inst); break;
            case Opcode::GE: comparison<Opcode::GE>(inst); break;
            case Opcode::GT: comparison<Opcode::GT>(inst); break;

            case Opcode::JIF: jif(inst); break;
            case Opcode::UNKNOWN:
            default: break;
        }

        pc++;
    }
    dbg(reg[1]->value<int>());
}

void CVM::ASM::VM::setCode(const std::vector<ASM::Instruction> &insts)
{
    VM::code = insts;
}

void CVM::ASM::VM::mov(const CVM::ASM::Instruction &instruction)
{

    if (instruction.operand_target1 == ASM::OperandTarget::REGISTER)
    {
        int dest_reg_idx = instruction.idx1;
        if (instruction.operand_target2 == ASM::OperandTarget::RAW)
        {
            delete reg[dest_reg_idx];
            reg[dest_reg_idx] = new Type(instruction.operand2->value<int>());
        }
        else if (instruction.operand_target2 == ASM::OperandTarget::REGISTER)
        {
            int src_reg_idx   = instruction.idx2;
            reg[dest_reg_idx] = new Type(reg[src_reg_idx]->value<int>());
        }
    }
}

void CVM::ASM::VM::jmp(const CVM::ASM::Instruction &instruction)
{
    pc = instruction.idx1;
}

void CVM::ASM::VM::jif(const CVM::ASM::Instruction &instruction)
{
    if (state == 1) jmp(instruction);
    state = 0;
}

void CVM::ASM::VM::call(const CVM::ASM::Instruction &instruction)
{
    stack.push(new Type(pc));
    pc = instruction.idx1;
}

void CVM::ASM::VM::ret(const CVM::ASM::Instruction &instruction)
{
    auto *pc_ptr = stack.pop();
    pc           = pc_ptr->value<int>();
    delete pc_ptr;
}

void CVM::ASM::VM::push(const CVM::ASM::Instruction &instruction)
{
    if (instruction.operand_target1 == ASM::OperandTarget::REGISTER) stack.push(reg[instruction.idx1]);
    if (instruction.operand_target1 == ASM::OperandTarget::STACK) stack.push(stack[instruction.idx1]);
    if (instruction.operand_target1 == ASM::OperandTarget::RAW)
        stack.push(new Type(instruction.operand1->value<int>()));
}

void CVM::ASM::VM::pop(const CVM::ASM::Instruction &instruction)
{
    if (instruction.operand_target1 == ASM::OperandTarget::REGISTER) reg[instruction.idx1] = stack.pop();
    if (instruction.operand_target1 == ASM::OperandTarget::STACK) stack[instruction.idx1] = stack.pop();
    if (instruction.operand_target1 == ASM::OperandTarget::RAW) stack.pop(instruction.idx1);
}

template<CVM::ASM::Opcode Op>
void CVM::ASM::VM::arithmetic(const CVM::ASM::Instruction &instruction)
{
    Type *obj1 = nullptr, *obj2 = nullptr, *result = nullptr;
    if (instruction.operand_target1 == ASM::OperandTarget::REGISTER) obj1 = reg[instruction.idx1];
    if (instruction.operand_target2 == ASM::OperandTarget::REGISTER)
        obj2 = reg[instruction.idx2];
    else
        obj2 = instruction.operand2.get();

    switch (Op)
    {
        case Opcode::ADD:
            if (obj1->is<std::string>() || obj2->is<std::string>())
            {
                result = new Type(obj1->as<std::string>() + obj2->as<std::string>());
            }
            if (obj1->is<double>() || obj2->is<double>())
            {
                result = new Type(obj1->as<double>() + obj2->as<double>());
            }
            if (obj1->is<int>() && obj2->is<int>())
            {
                result = new Type(obj1->as<int>() + obj2->as<int>());
            }
            break;
        case Opcode::SUB:
            if (obj1->is<double>() || obj2->is<double>())
            {
                result = new Type(obj1->as<double>() - obj2->as<double>());
            }
            if (obj1->is<int>() && obj2->is<int>())
            {
                result = new Type(obj1->as<int>() - obj2->as<int>());
            }
            break;
        case Opcode::MUL:
            if (obj1->is<double>() || obj2->is<double>())
            {
                result = new Type(obj1->as<double>() * obj2->as<double>());
            }
            if (obj1->is<int>() && obj2->is<int>())
            {
                result = new Type(obj1->as<int>() * obj2->as<int>());
            }
            if (obj1->is<std::string>() && !obj2->is<std::string>() || //
                obj2->is<std::string>() && !obj1->is<std::string>())
            {
                std::string origin_str, tmp;
                int times;
                if (obj1->is<std::string>())
                {
                    origin_str = obj1->value<std::string>();
                    times      = obj2->as<int>();
                }
                else
                {
                    origin_str = obj2->value<std::string>();
                    times      = obj1->as<int>();
                }
                for (int i = 0; i < times; i++)
                {
                    tmp += origin_str;
                }
                result = new Type(tmp);
            }
            break;
        case Opcode::DIV:
            if (obj1->is<double>() || obj2->is<double>())
            {
                result = new Type(obj1->as<double>() / obj2->as<double>());
            }
            if (obj1->is<int>() && obj2->is<int>())
            {
                result = new Type(obj1->as<int>() / obj2->as<int>());
            }
            break;
    }

    delete reg[instruction.idx1];
    reg[instruction.idx1] = result;
}

template<CVM::ASM::Opcode Op>
void CVM::ASM::VM::comparison(const CVM::ASM::Instruction &instruction)
{
    state = 0; // initialize

    Type *obj1 = nullptr, *obj2 = nullptr;
    if (instruction.operand_target1 == ASM::OperandTarget::REGISTER)
        obj1 = reg[instruction.idx1];
    else if (instruction.operand_target1 == ASM::OperandTarget::RAW)
        obj1 = instruction.operand1.get();

    if (instruction.operand_target2 == ASM::OperandTarget::REGISTER)
        obj2 = reg[instruction.idx2];
    else if (instruction.operand_target2 == ASM::OperandTarget::RAW)
        obj2 = instruction.operand2.get();

    switch (Op)
    {
        case Opcode::NE:
            if (!obj1->is<std::string>() && !obj2->is<std::string>())
            {
                state = obj1->as<double>() != obj2->as<double>();
            }
            else if (obj1->is<std::string>() && obj2->is<std::string>())
            {
                state = obj1->as<std::string>() != obj2->as<std::string>();
            }
            break;
        case Opcode::EQ:
            if (!obj1->is<std::string>() && !obj2->is<std::string>())
            {
                state = obj1->as<double>() == obj2->as<double>();
            }
            else if (obj1->is<std::string>() && obj2->is<std::string>())
            {
                state = obj1->as<std::string>() == obj2->as<std::string>();
            }
            break;
        case Opcode::LE:
            if (!obj1->is<std::string>() && !obj2->is<std::string>())
            {
                state = obj1->as<double>() <= obj2->as<double>();
            }
            else if (obj1->is<std::string>() && obj2->is<std::string>())
            {
                state = obj1->as<std::string>() <= obj2->as<std::string>();
            }
            break;
        case Opcode::LT:
            if (!obj1->is<std::string>() && !obj2->is<std::string>())
            {
                state = obj1->as<double>() < obj2->as<double>();
            }
            else if (obj1->is<std::string>() && obj2->is<std::string>())
            {
                state = obj1->as<std::string>() < obj2->as<std::string>();
            }
            break;
        case Opcode::GE:
            if (!obj1->is<std::string>() && !obj2->is<std::string>())
            {
                state = obj1->as<double>() >= obj2->as<double>();
            }
            else if (obj1->is<std::string>() && obj2->is<std::string>())
            {
                state = obj1->as<std::string>() >= obj2->as<std::string>();
            }
            break;
        case Opcode::GT:
            if (!obj1->is<std::string>() && !obj2->is<std::string>())
            {
                state = obj1->as<double>() > obj2->as<double>();
            }
            else if (obj1->is<std::string>() && obj2->is<std::string>())
            {
                state = obj1->as<std::string>() > obj2->as<std::string>();
            }
            break;
    }
}

void CVM::ASM::VM::setEntry(int i)
{
    pc = i;
}
