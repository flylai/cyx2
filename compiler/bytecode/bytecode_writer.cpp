#include "bytecode_writer.h"

void COMPILER::BytecodeWriter::writeInsts()
{
    writeHeader(entry);
    for (auto *inst : vm_insts)
    {
        writeOpcode(inst->opcode);
        cur_inst = inst;
        switch (inst->opcode)
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
            case CVM::Opcode::LAND: writeBinary(); break;
            case CVM::Opcode::LNOT:
            case CVM::Opcode::BNOT: break;
            case CVM::Opcode::LOADI: writeLoad<int>(); break;
            case CVM::Opcode::STOREI: writeStore<int>(); break;
            case CVM::Opcode::LOADD: writeLoad<double>(); break;
            case CVM::Opcode::STORED: writeStore<double>(); break;
            case CVM::Opcode::LOADS: writeLoad<std::string>(); break;
            case CVM::Opcode::STORES: writeStore<std::string>(); break;
            case CVM::Opcode::LOADX: writeLoadX(); break;
            case CVM::Opcode::STOREX: writeStoreX(); break;
            case CVM::Opcode::CALL: writeCall(); break;
            case CVM::Opcode::FUNC: writeFunc(); break;
            case CVM::Opcode::ARG: writeArg(); break;
            case CVM::Opcode::PARAM: writeParam(); break;
            case CVM::Opcode::RET: writeRet(); break;
            case CVM::Opcode::JMP: writeJmp(); break;
            case CVM::Opcode::JIF: writeJif(); break;
            default: ERROR("unsupported inst"); break;
        }
    }
}

void COMPILER::BytecodeWriter::writeHeader(long long int entry)
{
    // magic number
    writeByte(0xc2);
    // version
    writeByte(0x01);
    // entry point
    writeInt(entry);
}

void COMPILER::BytecodeWriter::writeByte(unsigned char val)
{
    buffer.push_back(val);
}

void COMPILER::BytecodeWriter::writeInt(long long val)
{
    auto x = reinterpret_cast<unsigned long long &>(val);
    for (int i = 0; i < 8; i++)
    {
        writeByte(x & 0xff);
        x >>= 8;
    }
}

void COMPILER::BytecodeWriter::writeDouble(double val)
{
    writeInt(reinterpret_cast<long long &>(val));
}

void COMPILER::BytecodeWriter::writeString(const std::string &val)
{
    writeInt(val.size());
    for (auto x : val)
    {
        writeByte(x);
    }
}

void COMPILER::BytecodeWriter::writeToFile()
{
    std::ofstream out;
    out.open(filename, std::ios::out | std::ios::binary);
    for (auto x : buffer)
    {
        out << x;
    }
    out.flush();
    out.close();
}

void COMPILER::BytecodeWriter::writeOpcode(CVM::Opcode opcode)
{
    writeByte(CVM::opcode2UChar(opcode));
}

void COMPILER::BytecodeWriter::writeBinary()
{
    auto *tmp = static_cast<CVM::Binary *>(cur_inst);
    writeByte(tmp->reg_idx1);
    writeByte(tmp->reg_idx2);
}

void COMPILER::BytecodeWriter::writeLoadX()
{
    // LOAD DEST SRC
    auto *tmp = static_cast<CVM::LoadX *>(cur_inst);
    writeByte(tmp->reg_idx);
    writeString(tmp->name);
}

template<typename T>
void COMPILER::BytecodeWriter::writeLoad()
{
    if constexpr (std::is_same<T, int>())
    {
        auto *tmp = static_cast<CVM::LoadI *>(cur_inst);
        writeByte(tmp->reg_idx);
        writeInt(tmp->val);
    }
    else if constexpr (std::is_same<T, double>())
    {
        auto *tmp = static_cast<CVM::LoadD *>(cur_inst);
        writeByte(tmp->reg_idx);
        writeDouble(tmp->val);
    }
    else if constexpr (std::is_same<T, std::string>())
    {
        auto *tmp = static_cast<CVM::LoadS *>(cur_inst);
        writeByte(tmp->reg_idx);
        writeString(tmp->val);
    }
    else
        UNREACHABLE();
}

void COMPILER::BytecodeWriter::writeStoreX()
{
    // STORE DEST SRC
    auto *tmp = static_cast<CVM::StoreX *>(cur_inst);
    writeByte(tmp->reg_idx);
    writeString(tmp->name);
}

template<typename T>
void COMPILER::BytecodeWriter::writeStore()
{
    if constexpr (std::is_same<T, int>())
    {
        auto *tmp = static_cast<CVM::StoreI *>(cur_inst);
        writeString(tmp->name);
        writeInt(tmp->val);
    }
    else if constexpr (std::is_same<T, double>())
    {
        auto *tmp = static_cast<CVM::StoreD *>(cur_inst);
        writeString(tmp->name);
        writeDouble(tmp->val);
    }
    else if constexpr (std::is_same<T, std::string>())
    {
        auto *tmp = static_cast<CVM::StoreS *>(cur_inst);
        writeString(tmp->name);
        writeString(tmp->val);
    }
    else
        UNREACHABLE();
}

void COMPILER::BytecodeWriter::writeArg()
{
    auto *arg = static_cast<CVM::Arg *>(cur_inst);
    // write target type
    if (arg->type == CVM::Arg::Type::MAP)
    {
        writeByte(0);
        writeString(arg->name);
    }
    else if (arg->type == CVM::Arg::Type::RAW)
    {
        writeByte(1);
        // int = 0 / double = 1 / string = 2
        if (arg->value.is<int>())
        {
            writeByte(0);
            writeInt(arg->value.as<int>());
        }
        else if (arg->value.is<double>())
        {
            writeByte(1);
            writeDouble(arg->value.as<double>());
        }
        else if (arg->value.is<std::string>())
        {
            writeByte(2);
            writeString(arg->value.as<std::string>());
        }
    }
}

void COMPILER::BytecodeWriter::writeCall()
{
    auto *tmp = static_cast<CVM::Call *>(cur_inst);
    writeInt(tmp->target); // func line no
}

void COMPILER::BytecodeWriter::writeFunc()
{
    auto *tmp = static_cast<CVM::Func *>(cur_inst);
    writeByte(tmp->param_count); // argument count
}

void COMPILER::BytecodeWriter::writeParam()
{
    auto *tmp = static_cast<CVM::Param *>(cur_inst);
    writeString(tmp->name);
}

void COMPILER::BytecodeWriter::writeRet()
{
    // TODO none void return value
}

void COMPILER::BytecodeWriter::writeJmp()
{
    auto *tmp = static_cast<CVM::Jmp *>(cur_inst);
    writeInt(tmp->target);
}

void COMPILER::BytecodeWriter::writeJif()
{
    auto *tmp = static_cast<CVM::Jif *>(cur_inst);
    writeInt(tmp->target1);
    writeInt(tmp->target2);
}
