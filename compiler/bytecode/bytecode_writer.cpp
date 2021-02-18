#include "bytecode_writer.h"

void BytecodeWriter::writeHeader(long long int entry)
{
    // magic number
    writeByte(0xc2);
    // version
    writeByte(0x01);
    // entry point
    writeInt(entry);
}

void BytecodeWriter::writeByte(unsigned char val)
{
    buffer.push_back(val);
}

void BytecodeWriter::writeInt(long long val)
{
    auto x = reinterpret_cast<unsigned long long &>(val);
    for (int i = 0; i < 8; i++)
    {
        writeByte(x & 0xff);
        x >>= 8;
    }
}

void BytecodeWriter::writeDouble(double val)
{
    writeInt(reinterpret_cast<long long &>(val));
}

void BytecodeWriter::writeString(const std::string &val)
{
    writeInt(val.size());
    for (auto x : val)
    {
        writeByte(x);
    }
}

void BytecodeWriter::writeToFile()
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
