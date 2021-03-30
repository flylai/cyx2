#include "common/config.h"
#include "compiler/ast/ast.hpp"
#include "compiler/ast/ast_visualize.h"
#include "compiler/bytecode/bytecode_generator.h"
#include "compiler/bytecode/bytecode_writer.h"
#include "compiler/ir/basicblock.hpp"
#include "compiler/ir/cfg.h"
#include "compiler/ir/ir_generator.h"
#include "compiler/parser.h"
#include "compiler/token.hpp"
#include "core/bytecode_reader.h"
#include "core/vm.hpp"

#include <iostream>
#include <string>

void showHelp()
{
    std::string str = "Usage:\n";
    addSpace(str, 4);
    str += "cyx2 [options] [src file]\n";
    addSpace(str, 4);
    str += "cyx2 -i-bytecode <bytecode file>\n";
    const std::vector<std::vector<std::string>> usage = {
        { "-ssa", "enable SSA mode, default is disabled" },                                              //
        { "-constant-folding", "enable constant folding(SSA based)" },                                   //
        { "-constant-propagation", "enable constant propagation(constant folding and SSA based)" },      //
        { "-no-code-simplify", "disable clearing temporary variables(after normal ir construction). " }, //
        { "-no-cfg-simplify", "disable clearing redundant basicblocks(empty and useless basicblocks)" }, //
        { "-remove-unused-code", "remove unused variable definitions(aggressively)" },                   //
        { "-dump-cfg", "dump CFG(Graphviz), dump to stdout if `-dump-as-file` is not set" },             //
        { "-dump-ir", "dump IR, dump to stdout if `-dump-as-file` is not set" },                         //
        { "-dump-ast", "dump AST(Graphviz), dump to stdout if `-dump-as-file` is not set" },             //
        { "-dump-vm-inst", "dump vm instructions, dump to stdout if `-dump-as-file` is not set" },       //
        { "-dump-as-file", "dump (cfg, ir, ast) as file, if not set `-o-`, `cyx.TYPE` is default" },     //
        { "-o-ast", "<destination  file> dump AST to file" },                                            //
        { "-o-ir", "<destination file> dump IR to file" },                                               //
        { "-o-cfg", "<destination file> dump CFG to file" },                                             //
        { "-o-vm-inst", "<destination file> dump vm instructions(text) to file" },                       //
        { "-o-bytecode", "<destination file> dump bytecode(binary) to file" },                           //
        { "-i-bytecode", "<bytecode file> only virtual machine mode, no compiler" }                      //
    };

    str += "where options include:\n";
    for (auto argument : usage)
    {
        addSpace(str, 4);
        str += argument[0] + "\n";
        addSpace(str, 6);
        str += argument[1] + "\n";
    }
    std::cout << str;
}

void readBytecode(CVM::BytecodeReader &bytecode_reader)
{
    bytecode_reader.readFile();
    bytecode_reader.readInsts();
}

void runVM(CVM::VM &vm, const std::vector<CVM::VMInstruction *> &insts, int entry, int entry_end, int global_init_len)
{
    vm.setInsts(insts);
    vm.setEntry(entry);
    vm.setEntryEnd(entry_end);
    vm.setGlobalInitLen(global_init_len);
    vm.run();
}

void writeFile(const std::string &filename, const std::string &content)
{
    std::ofstream out;
    out.open(filename, std::ios::out);
    out << content;
    out.flush();
    out.close();
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        showHelp();
        return 0;
    }
    std::vector<std::string> args(argv + 1, argv + argc);
    std::string ast_output     = "cyx.ast";  // text
    std::string vm_inst_output = "cyx.inst"; // text
    std::string ir_output      = "cyx.ir";   // text
    std::string cfg_output     = "cyx.cfg";  // text
    std::string bytecode_output;             // binary
    std::string bytecode_input;              // binary
    std::string src_input = args.back();
    //
    bool dump_as_file = false;
    //

#define CASE_TRUE(COND, VAR) else if (args[i] == (COND)) VAR = true;
    for (int i = 0; i < args.size() - 1; i++)
    {
        if (args[i] == "-ssa") NO_SSA = false;
        CASE_TRUE("-constant-folding", CONSTANT_FOLDING)
        CASE_TRUE("-constant-propagation", CONSTANT_PROPAGATION)
        CASE_TRUE("-no-code-simplify", NO_CODE_SIMPLIFY)
        CASE_TRUE("-no-cfg-simplify", NO_CFG_SIMPLIFY)
        CASE_TRUE("-remove-unused-code", REMOVE_UNUSED_DEFINE)
        CASE_TRUE("-dump-cfg", DUMP_CFG_STR)
        CASE_TRUE("-dump-ir", DUMP_IR_STR)
        CASE_TRUE("-dump-ast", DUMP_AST_STR)
        CASE_TRUE("-dump-vm-inst", DUMP_VM_INST_STR)
        CASE_TRUE("-dump-as-file", dump_as_file)
        else if (args[i] == "-o-ast")
        {
            ast_output = args[++i];
        }
        else if (args[i] == "-o-ir")
        {
            ir_output = args[++i];
        }
        else if (args[i] == "-o-cfg")
        {
            cfg_output = args[++i];
        }
        else if (args[i] == "-o-bytecode")
        {
            bytecode_output = args[++i];
        }
        else if (args[i] == "-i-bytecode")
        {
            bytecode_input = args[++i];
        }
        else if (args[i] == "-o-vm-inst")
        {
            vm_inst_output = args[++i];
        }
        else
        {
            std::cerr << "Unsupported option `" + args[i] + "` \n";
            showHelp();
            return 0;
        }
    }

#undef CASE_TRUE

    // only vm mode
    if (!bytecode_input.empty())
    {
        CVM::BytecodeReader bytecode_reader(bytecode_input);
        CVM::VM vm;
        readBytecode(bytecode_reader);
        runVM(vm, bytecode_reader.vm_insts, bytecode_reader.entry, bytecode_reader.entry_end,
              bytecode_reader.global_var_len);
        return 0;
    }

    // read src
    std::ifstream in(src_input, std::ios::in);
    std::string code((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    // parse src
    COMPILER::Parser parser(code);
    auto *ast = parser.parse();
    // build ir
    COMPILER::IRGenerator ir_generator;
    ir_generator.visitTree(ast);
    if (!NO_CODE_SIMPLIFY) ir_generator.simplifyIR();
    if (REMOVE_UNUSED_DEFINE) ir_generator.removeUnusedVarDef();
    // cfg, ssa, optimize related.
    COMPILER::CFG cfg;
    cfg.funcs = ir_generator.funcs;
    if (!NO_CFG_SIMPLIFY) cfg.simplifyCFG();
    if (!NO_SSA) cfg.transformToSSA();
    // vm instruction builder
    COMPILER::BytecodeGenerator bytecode_generator;
    bytecode_generator.funcs       = cfg.funcs;
    bytecode_generator.global_vars = ir_generator.global_var_decl;
    bytecode_generator.ir2VmInst();

    // dump debug str
    if (DUMP_AST_STR)
    {
        COMPILER::ASTVisualize ast_visualizer;
        ast_visualizer.visitTree(ast);
        if (!dump_as_file)
            std::cout << ast_visualizer.astStr();
        else
            writeFile(ast_output, ast_visualizer.astStr());
    }
    if (DUMP_IR_STR)
    {
        if (!dump_as_file)
            std::cout << ir_generator.irStr();
        else
            writeFile(ir_output, ir_generator.irStr());
    }
    if (DUMP_CFG_STR)
    {
        if (!dump_as_file)
            std::cout << cfg.cfgStr();
        else
            writeFile(cfg_output, cfg.cfgStr());
    }
    if (DUMP_VM_INST_STR)
    {
        if (!dump_as_file)
            std::cout << bytecode_generator.vmInstStr();
        else
            writeFile(vm_inst_output, bytecode_generator.vmInstStr());
    }

    if (!bytecode_output.empty())
    {
        COMPILER::BytecodeWriter bytecode_writer(bytecode_output);
        bytecode_writer.entry             = bytecode_generator.entry;
        bytecode_generator.global_var_len = bytecode_generator.global_var_len;
        bytecode_writer.vm_insts          = bytecode_generator.vm_insts;
        bytecode_writer.writeInsts();
        bytecode_writer.writeToFile();
    }

    CVM::VM vm;
    runVM(vm, bytecode_generator.vm_insts, bytecode_generator.entry, bytecode_generator.entry_end,
          bytecode_generator.global_var_len);
    return 0;
}