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
            case Opcode::LNOT:
            case Opcode::BNOT: unary(); break;
            case Opcode::LOADI:
            case Opcode::LOADD:
            case Opcode::LOADA:
            case Opcode::LOADS: load(); break;
            case Opcode::LOADX: loadX(); break;
            case Opcode::LOADXA: loadXA(); break;
            case Opcode::STOREI:
            case Opcode::STORED:
            case Opcode::STORES:
            case Opcode::STOREA: store(); break;
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
}

void CVM::VM::setInsts(const std::vector<VMInstruction *> &insts)
{
    vm_insts = insts;
}

void CVM::VM::setEntry(int i)
{
    entry = i;
}

void CVM::VM::setEntryEnd(int i)
{
    entry_end = i;
}

void CVM::VM::setGlobalInitLen(int i)
{
    global_var_init_len = i;
}

bool CVM::VM::fetch()
{
    if (pc == global_var_init_len && mode == Mode::INIT)
    {
        pc   = entry;
        mode = Mode::MAIN;
        // frame[0] is global var decl table
        frame.emplace_back();
    }
    if (pc == entry_end) return false;
    if (pc < vm_insts.size())
    {
        cur_inst = vm_insts[pc];
        return true;
    }
    return false;
}

void CVM::VM::unary()
{
    auto *inst   = static_cast<Unary *>(cur_inst);
    auto &target = reg[inst->reg_idx];
    if (inst->opcode == Opcode::LNOT)
        target = !target;
    else // bnot
        target = ~target;
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
        case Opcode::LAND: state = reg[reg_idx1] && reg[reg_idx2]; break;
        case Opcode::NE: state = reg[reg_idx1] != reg[reg_idx2]; break;
        case Opcode::EQ: state = reg[reg_idx1] == reg[reg_idx2]; break;
        case Opcode::LT: state = reg[reg_idx1] < reg[reg_idx2]; break;
        case Opcode::LE: state = reg[reg_idx1] <= reg[reg_idx2]; break;
        case Opcode::GT: state = reg[reg_idx1] > reg[reg_idx2]; break;
        case Opcode::GE: state = reg[reg_idx1] >= reg[reg_idx2]; break;
        default: UNREACHABLE(); break;
    }
}

void CVM::VM::loadXA()
{
    auto *inst                      = static_cast<LoadXA *>(cur_inst);
    reg[inst->reg_idx][inst->index] = *findSymbol(inst->name);
}

void CVM::VM::loadX()
{
    auto *inst  = static_cast<LoadX *>(cur_inst);
    auto target = findSymbol(inst->name);
    // MAGIC, DO NOT TOUCH...
    for (auto idx : inst->index)
    {
        if (std::holds_alternative<long long>(idx))
            target = &target->asArray()->at(std::get<long long>(idx));
        else
        {
            const auto i = findSymbol(std::get<std::string>(idx))->as<long long>();
            target       = &target->asArray()->at(i);
        }
    }
    reg[inst->reg_idx] = *target;
}

void CVM::VM::load()
{
    auto op = cur_inst->opcode;
    if (op == Opcode::LOADI)
    {
        auto *inst         = static_cast<LoadI *>(cur_inst);
        reg[inst->reg_idx] = inst->val;
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
    else if (op == Opcode::LOADA)
    {
        auto *inst         = static_cast<LoadA *>(cur_inst);
        reg[inst->reg_idx] = inst->array;
    }
    else
        UNREACHABLE();
}

void CVM::VM::storeX()
{
    auto *inst  = static_cast<StoreX *>(cur_inst);
    auto target = findSymbol(inst->name);
    // MAGIC, DO NOT TOUCH...
    for (auto idx : inst->index)
    {
        if (std::holds_alternative<long long>(idx))
            target = &target->asArray()->at(std::get<long long>(idx));
        else
        {
            const auto i = findSymbol(std::get<std::string>(idx))->as<long long>();
            target       = &target->asArray()->at(i);
        }
    }
    *target = reg[inst->reg_idx];
}

void CVM::VM::store()
{
    auto op = cur_inst->opcode;
    if (op == Opcode::STOREI)
    {
        auto *inst                       = static_cast<StoreI *>(cur_inst);
        frame.back().symbols[inst->name] = inst->val;
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
    else if (op == Opcode::STOREA)
    {
        auto *inst  = static_cast<StoreA *>(cur_inst);
        auto target = findSymbol(inst->name);
        for (auto idx : inst->index)
        {
            if (std::holds_alternative<long long>(idx))
                target = &target->asArray()->at(std::get<long long>(idx));
            else
            {
                const auto i = findSymbol(std::get<std::string>(idx))->as<long long>();
                target       = &target->asArray()->at(i);
            }
        }
        *target = inst->value;
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
    auto *call         = static_cast<Call *>(cur_inst);
    auto *buildin_func = buildin_functions_index.at(-call->target);
    CYX::Value *retval = nullptr;
    // TODO: some bugs here...
    if (vm_insts[pc + 1]->opcode == Opcode::ARG)
    {
        pc++;
        auto *arg = static_cast<Arg *>(vm_insts[pc]);
        if (arg->type == ArgType::MAP)
        {
            auto target = findSymbol(arg->name);
            if (!arg->index.empty())
            {
                // MAGIC, DO NOT TOUCH...
                for (auto idx : arg->index)
                {
                    if (std::holds_alternative<long long>(idx))
                        target = &target->asArray()->at(std::get<long long>(idx));
                    else
                    {
                        const auto i = findSymbol(std::get<std::string>(idx))->as<long long>();
                        target       = &target->asArray()->at(i);
                    }
                }
            }
            retval = buildin_func(target);
        }
        else if (arg->type == ArgType::RAW)
        {
            retval = buildin_func(&arg->value);
        }
    }
    else
    {
        retval = buildin_func(nullptr);
    }
    if (retval != nullptr) reg[1] = *retval;
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
    if (arg->type == ArgType::MAP)
    {
        if (!arg->index.empty())
        {
        }
        frame.back().symbols[inst->name] = pre_frame->symbols[arg->name];
    }
    else if (arg->type == ArgType::RAW)
    {
        frame.back().symbols[inst->name] = arg->value;
    }
    else
        UNREACHABLE();
}

void CVM::VM::ret()
{
    frame.pop_back();
    if (frame.size() > 1) pc = frame.back().pc;
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

CYX::Value *CVM::VM::findSymbol(const std::string &name)
{
    if (frame.back().symbols.find(name) != frame.back().symbols.end()) return &frame.back().symbols[name];
    if (frame[0].symbols.find(name) == frame[0].symbols.end()) return &frame.back().symbols[name];
    return &frame[0].symbols[name];
}
