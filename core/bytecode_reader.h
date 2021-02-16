#ifndef CVM_BYTECODE_READER_H
#define CVM_BYTECODE_READER_H

#include "instruction.hpp"
#include "opcode.hpp"

#include <cmath>
#include <fstream>
#include <utility>

class BytecodeReader
{
  public:
    BytecodeReader() = delete;
    explicit BytecodeReader(std::string filename) : filename(std::move(filename))
    {
    }
    void readFile();

  public:
    void readHeader();
    unsigned char readByte();
    long long readInt();
    double readDouble();
    std::string readString();

  private:
    std::string filename;
    int pos{ 0 };
    int size{ 0 };
    unsigned char *buffer{ nullptr };
};

#endif // CVM_BYTECODE_READER_H
