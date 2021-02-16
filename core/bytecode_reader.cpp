#include "bytecode_reader.h"

void BytecodeReader::readFile()
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

void BytecodeReader::readHeader()
{
    auto magic_number = readByte();
    auto version      = readByte();
    auto entry_point  = readInt();
    if (magic_number != 0xc2 && version != 0x01) ERROR("bytecode file error!");
}

unsigned char BytecodeReader::readByte()
{
    if (pos < size) return buffer[pos++];
    ERROR("out of range.");
}

long long BytecodeReader::readInt()
{
    unsigned long long ret = 0;
    int i                  = 0;
    int base               = 256;
    while (i < 8)
    {
        auto byte = readByte();
        if (byte != 0) ret += byte * std::pow(base, i);
        i++;
    }
    return reinterpret_cast<long long &>(ret);
}

double BytecodeReader::readDouble()
{
    auto x = readInt();
    return reinterpret_cast<double &>(x);
}

std::string BytecodeReader::readString()
{
    auto str_len = readInt();
    std::string ret;
    for (int i = 0; i < str_len; i++)
    {
        ret.push_back((char) readByte());
    }
    return ret;
}