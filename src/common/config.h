#ifndef CVM_CONFIG_H
#define CVM_CONFIG_H

#include <string>

extern const std::string ENTRY_FUNC;
extern bool NO_SSA;
extern bool NO_CODE_SIMPLIFY;
extern bool NO_CFG_SIMPLIFY;
extern bool CONSTANT_FOLDING;
extern bool CONSTANT_PROPAGATION;
extern bool REMOVE_UNUSED_DEFINE;
extern bool DEAD_CODE_ELIMINATION;
extern bool PEEPHOLE;
//
extern const int STATE_REGISTER;
// debug output
extern bool DUMP_AST_STR;
extern bool DUMP_CFG_STR;
extern bool DUMP_IR_STR;
extern bool DUMP_VM_INST_STR;

#endif // CVM_CONFIG_H
