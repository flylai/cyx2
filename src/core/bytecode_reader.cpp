#include "bytecode_reader.h"

void CVM::BytecodeReader::readFile()
{
    std::ifstream in;
    in.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
    size = in.tellg();
    in.seekg(0);
    buffer = new unsigned char[size];
    in.read((char *) buffer, size);
    in.close();
    pos = 0;
}

void CVM::BytecodeReader::readInsts()
{
    readHeader();
    while (pos < size)
    {
        cur_opcode = readOpcode();
        switch (cur_opcode)
        {
            case CVM::Opcode::ADD:
            case CVM::Opcode::SUB:
            case CVM::Opcode::MUL:
            case CVM::Opcode::DIV:
            case CVM::Opcode::MOD:
            case CVM::Opcode::EXP:
            case CVM::Opcode::BAND:
            case CVM::Opcode::BOR:
            case CVM::Opcode::BXOR:
            case CVM::Opcode::SHL:
            case CVM::Opcode::SHR:
            case CVM::Opcode::LOR:
            case CVM::Opcode::NE:
            case CVM::Opcode::EQ:
            case CVM::Opcode::LT:
            case CVM::Opcode::LE:
            case CVM::Opcode::GT:
            case CVM::Opcode::GE:
            case CVM::Opcode::LAND: readBinary(); break;
            case CVM::Opcode::LNOT:
            case CVM::Opcode::BNOT: readUnary(); break;
            case CVM::Opcode::LOADA: readLoadA(); break;
            case CVM::Opcode::LOADXA: readLoadXA(); break;
            case CVM::Opcode::LOADI: readLoad<int>(); break;
            case CVM::Opcode::STOREI: readStore<int>(); break;
            case CVM::Opcode::LOADD: readLoad<double>(); break;
            case CVM::Opcode::STORED: readStore<double>(); break;
            case CVM::Opcode::LOADS: readLoad<std::string>(); break;
            case CVM::Opcode::STORES: readStore<std::string>(); break;
            case CVM::Opcode::LOADX: readLoadX(); break;
            case CVM::Opcode::STOREX: readStoreX(); break;
            case CVM::Opcode::STOREA: readStoreA(); break;
            case CVM::Opcode::CALL: readCall(); break;
            case CVM::Opcode::FUNC: readFunc(); break;
            case CVM::Opcode::ARG: readArg(); break;
            case CVM::Opcode::PARAM: readParam(); break;
            case CVM::Opcode::RET: readRet(); break;
            case CVM::Opcode::JMP: readJmp(); break;
            case CVM::Opcode::JIF: readJif(); break;
            default: logX("unknown opcode"); break;
        }
    }
}

void CVM::BytecodeReader::readHeader()
{
    auto magic_number = readByte();
    auto version      = readByte();
    entry             = readInt();
    entry_end         = readInt();
    global_var_len    = readInt();
    if (magic_number != 0xc2 && version != 0x01) ERROR("bytecode file error!");
}

unsigned char CVM::BytecodeReader::readByte()
{
    if (pos < size) return buffer[pos++];
    ERROR("out of range.");
}

long long CVM::BytecodeReader::readInt()
{
    unsigned long long ret = 0;
    int base               = 256;
    int k                  = 1;
    for (int i = 0; i < 8; i++)
    {
        auto byte = readByte();
        if (byte != 0) ret += byte * k;
        k *= base;
    }
    return reinterpret_cast<long long &>(ret);
}

double CVM::BytecodeReader::readDouble()
{
    auto x = readInt();
    return reinterpret_cast<double &>(x);
}

std::string CVM::BytecodeReader::readString()
{
    auto str_len = readInt();
    std::string ret;
    for (int i = 0; i < str_len; i++)
    {
        ret.push_back((char) readByte());
    }
    return ret;
}

CVM::Opcode CVM::BytecodeReader::readOpcode()
{
    return CVM::uchar2Opcode(readByte());
}

void CVM::BytecodeReader::readUnary()
{
    auto type = readByte();
    Unary *inst{ nullptr };
    if (cur_opcode == Opcode::LNOT)
        inst = new Lnot;
    else
        inst = new Bnot;

    // type tag. only string and int
    inst->reg_idx = readByte();
    if (type == 0)
    {
        inst->type  = ArgType::RAW;
        inst->value = (int) readInt();
    }
    else if (type == 2)
    {
        inst->type = ArgType::MAP;
        inst->name = readString();
    }
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readBinary()
{
    Binary *tmp{ nullptr };
    switch (cur_opcode)
    {
        case CVM::Opcode::ADD: tmp = new Add; break;
        case CVM::Opcode::SUB: tmp = new Sub; break;
        case CVM::Opcode::MUL: tmp = new Mul; break;
        case CVM::Opcode::DIV: tmp = new Div; break;
        case CVM::Opcode::MOD: tmp = new Mod; break;
        case CVM::Opcode::EXP: tmp = new Exp; break;
        case CVM::Opcode::BAND: tmp = new Band; break;
        case CVM::Opcode::BOR: tmp = new Bor; break;
        case CVM::Opcode::BXOR: tmp = new Bxor; break;
        case CVM::Opcode::SHL: tmp = new Shl; break;
        case CVM::Opcode::SHR: tmp = new Shr; break;
        case CVM::Opcode::LOR: tmp = new Lor; break;
        case CVM::Opcode::NE: tmp = new Ne; break;
        case CVM::Opcode::EQ: tmp = new Eq; break;
        case CVM::Opcode::LT: tmp = new Lt; break;
        case CVM::Opcode::LE: tmp = new Le; break;
        case CVM::Opcode::GT: tmp = new Gt; break;
        case CVM::Opcode::GE: tmp = new Ge; break;
        case CVM::Opcode::LAND: tmp = new Land; break;
        default: ERROR("WTF");
    }
    tmp->reg_idx1 = readByte();
    tmp->reg_idx2 = readByte();
    vm_insts.push_back(tmp);
}

void CVM::BytecodeReader::readLoadX()
{
    auto *inst    = new LoadX;
    inst->reg_idx = readByte();
    inst->name    = readString();
    std::vector<CVM::ArrIdx> arr;
    auto arr_size = readInt();
    for (int i = 0; i < arr_size; i++)
    {
        const auto type = readByte();
        if (type == 0)
        {
            arr.emplace_back((int) readInt());
        }
        else if (type == 2)
        {
            arr.emplace_back(readString());
        }
        else
            UNREACHABLE();
    }
    inst->index = std::move(arr);
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readLoadA()
{
    auto *inst    = new LoadA;
    inst->reg_idx = readByte();
    int idx_size  = readInt();
    std::vector<CYX::Value> arr;
    for (int i = 0; i < idx_size; i++)
    {
        const auto type = readByte();
        if (type == 0)
            arr.emplace_back((int) readInt());
        else if (type == 1)
            arr.emplace_back(readDouble());
        else if (type == 2)
            arr.emplace_back(readString());
        else if (type == 3)
            arr.emplace_back();
    }
    inst->array = std::move(arr);
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readLoadXA()
{
    auto *inst    = new LoadXA;
    inst->reg_idx = readByte();
    inst->index   = readInt();
    inst->name    = readString();
    vm_insts.push_back(inst);
}

template<typename T>
void CVM::BytecodeReader::readLoad()
{
    if constexpr (std::is_same<T, int>())
    {
        auto *inst    = new LoadI;
        inst->reg_idx = readByte();
        inst->val     = readInt();
        vm_insts.push_back(inst);
    }
    else if constexpr (std::is_same<T, double>())
    {
        auto *inst    = new LoadD;
        inst->reg_idx = readByte();
        inst->val     = readDouble();
        vm_insts.push_back(inst);
    }
    else if constexpr (std::is_same<T, std::string>())
    {
        auto *inst    = new LoadS;
        inst->reg_idx = readByte();
        inst->val     = readString();
        vm_insts.push_back(inst);
    }
    else
        UNREACHABLE();
}

void CVM::BytecodeReader::readStoreX()
{
    auto *inst    = new StoreX;
    inst->name    = readString();
    auto arr_size = readInt();
    std::vector<ArrIdx> arr;
    for (int i = 0; i < arr_size; i++)
    {
        auto type = readByte();
        if (type == 0)
            arr.emplace_back((int) readInt());
        else if (type == 2)
            arr.emplace_back(readString());
        else
            UNREACHABLE();
    }
    inst->reg_idx = readByte();
    inst->index   = std::move(arr);
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readStoreA()
{
    auto *inst    = new StoreA;
    inst->name    = readString();
    auto arr_size = readInt();
    std::vector<ArrIdx> arr;
    for (int i = 0; i < arr_size; i++)
    {
        auto idx_type = readByte();
        if (idx_type == 0)
        {
            arr.emplace_back((int) readInt());
        }
        else if (idx_type == 2)
        {
            arr.emplace_back(readString());
        }
    }
    auto val_type = readByte();
    if (val_type == 0)
        inst->value = (int) readInt();
    else if (val_type == 1)
        inst->value = readDouble();
    else if (val_type == 2)
        inst->value = readString();
    vm_insts.push_back(inst);
}

template<typename T>
void CVM::BytecodeReader::readStore()
{
    if constexpr (std::is_same<T, int>())
    {
        auto *inst = new StoreI;
        inst->name = readString();
        inst->val  = readInt();
        vm_insts.push_back(inst);
    }
    else if constexpr (std::is_same<T, double>())
    {
        auto *inst = new StoreD;
        inst->name = readString();
        inst->val  = readDouble();
        vm_insts.push_back(inst);
    }
    else if constexpr (std::is_same<T, std::string>())
    {
        auto *inst = new StoreS;
        inst->name = readString();
        inst->val  = readString();
        vm_insts.push_back(inst);
    }
    else
        UNREACHABLE();
}

void CVM::BytecodeReader::readArg()
{
    auto *inst = new Arg;
    inst->type = readByte() == 0 ? ArgType::MAP : ArgType::RAW;
    if (inst->type == CVM::ArgType::RAW)
    {
        auto type = readByte();
        if (type == 0)
        {
            inst->value = (int) readInt();
        }
        else if (type == 1)
        {
            inst->value = readDouble();
        }
        else if (type == 2)
        {
            inst->value = readString();
        }
    }
    else if (inst->type == CVM::ArgType::MAP)
    {
        inst->name = readString();
    }
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readCall()
{
    auto *inst   = new Call;
    inst->target = readInt();
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readFunc()
{
    auto *inst        = new Func;
    inst->param_count = readByte();
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readParam()
{
    auto *inst = new Param;
    inst->name = readString();
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readRet()
{
    auto *inst = new Ret;
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readJmp()
{
    auto *inst   = new Jmp;
    inst->target = readInt();
    vm_insts.push_back(inst);
}

void CVM::BytecodeReader::readJif()
{
    auto *inst    = new Jif;
    inst->target1 = readInt();
    inst->target2 = readInt();
    vm_insts.push_back(inst);
}

std::string CVM::BytecodeReader::vmInstStr()
{
    std::string str;
    for (auto *inst : vm_insts)
    {
        str += inst->toString() + "\n";
    }
    return str;
}
