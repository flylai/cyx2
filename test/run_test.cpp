#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

#if (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64))
    #define popen _popen
    #define pclose _pclose
    #define CYX_WINDOWS
#endif

namespace fs = std::filesystem;

class CYXTest
{
  public:
    std::string execute(const std::string &path, const std::string &options)
    {
        const std::string raw_path   = "/" + path;
        const std::string src_file   = testcase_dir + raw_path + ".cyx";
        const std::string input_file = test_in_dir + raw_path + ".txt";
        std::string res;
        const std::string command = executable_file + " " + options + " " + src_file +
                                    (fs::is_regular_file(input_file) ? " < " + input_file : "");
        if (auto fp = popen(command.c_str(), "r"); fp != nullptr)
        {
            while (fgets(buffer, sizeof(buffer), fp) != nullptr)
            {
                res += std::string(buffer);
            }
            pclose(fp);
        }
        return res;
    }

    std::string executeBytecode(const std::string &path)
    {
        const std::string raw_path      = "/" + path;
        const std::string src_file      = testcase_dir + raw_path + ".cyx";
        const std::string input_file    = test_in_dir + raw_path + ".txt";
        const std::string bytecode_file = test_tmp_dir + raw_path;
        std::string res;
        const std::string build_cmd = executable_file + " -o-bytecode " + bytecode_file + " " + src_file;
        system(build_cmd.c_str());
        const std::string command = executable_file + " -i-bytecode " + bytecode_file +
                                    (fs::is_regular_file(input_file) ? " < " + input_file : "");
        if (auto fp = popen(command.c_str(), "r"); fp != nullptr)
        {
            while (fgets(buffer, sizeof(buffer), fp) != nullptr)
            {
                res += std::string(buffer);
            }
            pclose(fp);
        }
        return res;
    }

    std::string readfile(const std::string &file)
    {
        std::ifstream in(test_out_dir + "/" + file + ".txt", std::ios::in);
        return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }

    std::vector<std::string> listDir(const std::string &dir)
    {
        std::vector<std::string> test_files;
        fs::directory_entry dir_entry(testcase_dir + "/" + dir);
        auto dir_iter = fs::directory_iterator(dir_entry);
        for (const auto &file : dir_iter)
        {
            if (file.is_regular_file())
            {
                const std::string relative_path      = file.path().lexically_relative(testcase_dir).string();
                const std::string relative_file_path = relative_path.substr(0, relative_path.size() - 4);
                test_files.push_back(relative_file_path);
            }
        }
        return test_files;
    }

  public:
    const fs::path build_dir = fs::current_path();
#ifdef GITHUB_ACTIONS
    const fs::path root_dir = build_dir.parent_path().string() + "/cyx2";
#else
    const fs::path root_dir           = build_dir.parent_path();
#endif

    const std::string test_dir     = root_dir.string() + "/test";
    const std::string test_in_dir  = test_dir + "/in";
    const std::string test_out_dir = test_dir + "/out";
    const std::string testcase_dir = test_dir + "/testcase";
    const std::string test_tmp_dir = test_dir + "/tmp";

#ifdef CYX_WINDOWS
    const std::string executable_file = build_dir.string() + "/cyx2.exe";
#else
    const std::string executable_file = build_dir.string() + "/cyx2";
#endif

  private:
    char buffer[2048]{};
};

// TEST(Test, basic)
//{
//    CYXTest test;
//    auto file_list = test.listDir("basic");
//    for (const auto &file : file_list)
//    {
//        EXPECT_EQ(test.execute(file, ""), test.readfile(file));
//    }
//}

TEST(Basic, array)
{
    CYXTest test;
    const std::string file = "basic/array";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Basic, num_calc)
{
    CYXTest test;
    const std::string file = "basic/num_calc";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Basic, string_calc)
{
    CYXTest test;
    const std::string file = "basic/string_calc";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Basic, var)
{
    CYXTest test;
    const std::string file = "basic/var";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Basic, loop)
{
    CYXTest test;
    const std::string file = "basic/loop";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Basic, func_overload)
{
    CYXTest test;
    const std::string file = "basic/func_overload";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(Overall, quick_sort)
{
    CYXTest test;
    const std::string file = "overall/quick_sort";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Overall, fibonacci)
{
    CYXTest test;
    const std::string file = "overall/fibonacci";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Overall, fibonacci2)
{
    CYXTest test;
    const std::string file = "overall/fibonacci2";
    EXPECT_EQ(test.execute(file, ""), test.readfile("overall/fibonacci"));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile("overall/fibonacci"));
    EXPECT_EQ(test.executeBytecode(file), test.readfile("overall/fibonacci"));
}

TEST(Overall, prime)
{
    CYXTest test;
    const std::string file = "overall/prime";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Overall, swap)
{
    CYXTest test;
    const std::string file = "overall/swap";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Overall, fast_pow_mod)
{
    CYXTest test;
    const std::string file = "overall/fast_pow_mod";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(Overall, daffodil_number)
{
    CYXTest test;
    const std::string file = "overall/daffodil_number";
    EXPECT_EQ(test.execute(file, ""), test.readfile(file));
    EXPECT_EQ(test.execute(file, "-remove-unused-code"), test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(SSA, daffodil_number)
{
    CYXTest test;
    const std::string file = "overall/daffodil_number";
    EXPECT_EQ(test.execute(file, "-ssa -constant-folding -constant-propagation -dead-code-elimination"),
              test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(SSA, ssa1)
{
    CYXTest test;
    const std::string file = "ssa/ssa1";
    EXPECT_EQ(test.execute(file, "-ssa -constant-folding -constant-propagation -dead-code-elimination"),
              test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(SSA, constant_folding1)
{
    CYXTest test;
    const std::string file = "ssa/constant_folding1";
    EXPECT_EQ(test.execute(file, "-ssa -constant-folding -constant-propagation -dead-code-elimination"),
              test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

TEST(SSA, constant_folding2)
{
    CYXTest test;
    const std::string file = "ssa/constant_folding1";
    EXPECT_EQ(test.execute(file, "-ssa -constant-folding -constant-propagation -dead-code-elimination"),
              test.readfile(file));
    EXPECT_EQ(test.executeBytecode(file), test.readfile(file));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}