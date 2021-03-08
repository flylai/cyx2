#include "vm.hpp"

void CVM::VM::run()
{
    while (fetch())
    {
        switch (cur_inst->opcode)
        {
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MUL:
            case Opcode::DIV:
            case Opcode::MOD:
            case Opcode::EXP:
            case Opcode::BAND:
            case Opcode::BOR:
            case Opcode::BXOR:
            case Opcode::SHL:
            case Opcode::SHR:
            case Opcode::LOR:
            case Opcode::NE:
            case Opcode::EQ:
            case Opcode::LT:
            case Opcode::LE:
            case Opcode::GT:
            case Opcode::GE:
            case Opcode::LAND: binary(); break;

            case Opcode::LNOT: break; // todo
            case Opcode::BNOT: break; // todo
            case Opcode::LOADI:
            case Opcode::LOADD:
            case Opcode::LOADS: load(); break;
            case Opcode::LOADX: loadX(); break;
            case Opcode::STOREI:
            case Opcode::STORED:
            case Opcode::STORES: store(); break;
            case Opcode::STOREX: storeX(); break;
            case Opcode::CALL: call(); break;
            case Opcode::FUNC: break;
            case Opcode::ARG: arg(); break;
            case Opcode::PARAM: param(); break;
            case Opcode::RET: ret(); break;
            case Opcode::JMP: jmp(); break;
            case Opcode::JIF: jif(); break;
            default: UNREACHABLE();
        }
        pc++;
    }
    logX(reg[1].as<std::string>());
}

void CVM::VM::setInsts(const std::vector<VMInstruction *> &insts)
{
    vm_insts = insts;
}

void CVM::VM::setEntry(int i)
{
    pc = i;
}

bool CVM::VM::fetch()
{
    if (pc < vm_insts.size())
    {
        cur_inst = vm_insts[pc];
        return true;
    }
    return false;
}

void CVM::VM::binary()
{
    auto *inst   = static_cast<Binary *>(cur_inst);
    int reg_idx1 = inst->reg_idx1;
    int reg_idx2 = inst->reg_idx2;
    switch (cur_inst->opcode)
    {
        case Opcode::ADD: reg[reg_idx1] = reg[reg_idx1] + reg[reg_idx2]; break;
        case Opcode::SUB: reg[reg_idx1] = reg[reg_idx1] - reg[reg_idx2]; break;
        case Opcode::MUL: reg[reg_idx1] = reg[reg_idx1] * reg[reg_idx2]; break;
        case Opcode::DIV: reg[reg_idx1] = reg[reg_idx1] / reg[reg_idx2]; break;
        case Opcode::MOD: reg[reg_idx1] = reg[reg_idx1] % reg[reg_idx2]; break;
        case Opcode::EXP: /*TODO*/ break;
        case Opcode::BAND: reg[reg_idx1] = reg[reg_idx1] & reg[reg_idx2]; break;
        case Opcode::BOR: reg[reg_idx1] = reg[reg_idx1] | reg[reg_idx2]; break;
        case Opcode::BXOR: reg[reg_idx1] = reg[reg_idx1] ^ reg[reg_idx2]; break;
        case Opcode::SHL: reg[reg_idx1] = reg[reg_idx1] << reg[reg_idx2]; break;
        case Opcode::SHR: reg[reg_idx1] = reg[reg_idx1] >> reg[reg_idx2]; break;
        case Opcode::LOR: /*TODO*/ break;
        case Opcode::LAND: /*TODO*/ break;
        case Opcode::NE: state = reg[reg_idx1] != reg[reg_idx2]; break;
        case Opcode::EQ: state = reg[reg_idx1] == reg[reg_idx2]; break;
        case Opcode::LT: state = reg[reg_idx1] < reg[reg_idx2]; break;
        case Opcode::LE: state = reg[reg_idx1] <= reg[reg_idx2]; break;
        case Opcode::GT: state = reg[reg_idx1] > reg[reg_idx2]; break;
        case Opcode::GE: state = reg[reg_idx1] >= reg[reg_idx2]; break;
        default: UNREACHABLE(); break;
    }
}

void CVM::VM::loadX()
{
    auto *inst         = static_cast<LoadX *>(cur_inst);
    reg[inst->reg_idx] = frame.back().symbols[inst->name];
}

void CVM::VM::load()
{
    auto op = cur_inst->opcode;
    if (op == Opcode::LOADI)
    {
        auto *inst         = static_cast<LoadI *>(cur_inst);
        reg[inst->reg_idx] = (int) inst->val;
    }
    else if (op == Opcode::LOADD)
    {
        auto *inst         = static_cast<LoadD *>(cur_inst);
        reg[inst->reg_idx] = inst->val;
    }
    else if (op == Opcode::LOADS)
    {
        auto *inst         = static_cast<LoadS *>(cur_inst);
        reg[inst->reg_idx] = inst->val;
    }
    else
        UNREACHABLE();
}

void CVM::VM::storeX()
{
    auto *inst                       = static_cast<StoreX *>(cur_inst);
    frame.back().symbols[inst->name] = reg[inst->reg_idx];
}

void CVM::VM::store()
{
    auto op = cur_inst->opcode;
    if (op == Opcode::STOREI)
    {
        auto *inst                       = static_cast<StoreI *>(cur_inst);
        frame.back().symbols[inst->name] = (int) inst->val;
    }
    else if (op == Opcode::STORED)
    {
        auto *inst                       = static_cast<StoreD *>(cur_inst);
        frame.back().symbols[inst->name] = inst->val;
    }
    else if (op == Opcode::STORES)
    {
        auto *inst                       = static_cast<StoreS *>(cur_inst);
        frame.back().symbols[inst->name] = inst->val;
    }
    else
        UNREACHABLE();
}

void CVM::VM::arg()
{
    // pass
}

void CVM::VM::call()
{
    auto *inst = static_cast<Call *>(cur_inst);
    if (inst->target < 0)
    {
        callBuildin();
        return;
    }
    frame.back().pc = pc;
    frame.emplace_back(Frame());
    pc = inst->target - 1;
}

void CVM::VM::callBuildin()
{
    auto *call = static_cast<Call *>(cur_inst);
    pc++;
    auto *buildin_func = buildin_functions_index.at(-call->target);
    // TODO: some bugs here...
    while (vm_insts[pc]->opcode == Opcode::ARG)
    {
        auto *arg = static_cast<Arg *>(vm_insts[pc]);
        if (arg->type == Arg::Type::MAP)
        {
            buildin_func(&frame.back().symbols[arg->name]);
        }
        else if (arg->type == Arg::Type::RAW)
        {
            buildin_func(&arg->value);
        }
        pc++;
    }
}

void CVM::VM::func()
{
    // pass
}

void CVM::VM::param()
{
    auto *inst      = static_cast<Param *>(cur_inst);
    auto *pre_frame = &frame[frame.size() - 2];
    auto *arg       = static_cast<Arg *>(vm_insts[++pre_frame->pc]);
    if (arg->type == Arg::Type::MAP)
    {
        frame.back().symbols[inst->name] = pre_frame->symbols[arg->name];
    }
    else if (arg->type == Arg::Type::RAW)
    {
        frame.back().symbols[inst->name] = arg->value;
    }
    else
        UNREACHABLE();
}

void CVM::VM::ret()
{
    frame.pop_back();
    pc = frame.back().pc;
}

void CVM::VM::jmp()
{
    auto *inst = static_cast<Jmp *>(cur_inst);
    pc         = inst->target - 1;
}

void CVM::VM::jif()
{
    auto *inst = static_cast<Jif *>(cur_inst);
    if (state)
        pc = inst->target1 - 1;
    else
        pc = inst->target2 - 1;
    state = false;
}
