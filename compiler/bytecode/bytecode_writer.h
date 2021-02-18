#ifndef CVM_BYTECODE_WRITER_H
#define CVM_BYTECODE_WRITER_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>

class BytecodeWriter
{
  public:
    explicit BytecodeWriter(std::string filename) : filename(std::move(filename))
    {
    }
    void writeHeader(long long entry);
    void writeByte(unsigned char val);
    void writeInt(long long val);
    void writeDouble(double val);
    void writeString(const std::string &val);
    void writeToFile();

  private:
    std::string filename;
    int pos{ 0 };
    int size{ 0 };
    std::vector<unsigned char> buffer;
};

#endif // CVM_BYTECODE_WRITER_H
