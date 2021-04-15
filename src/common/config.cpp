#include "config.h"

const std::string ENTRY_FUNC = "main";
bool NO_SSA                  = true;
bool NO_CODE_SIMPLIFY        = false;
bool NO_CFG_SIMPLIFY         = false;
bool CONSTANT_FOLDING        = false;
bool CONSTANT_PROPAGATION    = false;
bool REMOVE_UNUSED_DEFINE    = false;
bool PEEPHOLE                = false;
//
const int STATE_REGISTER = 0;
// debug output
bool DUMP_AST_STR     = false;
bool DUMP_CFG_STR     = false;
bool DUMP_IR_STR      = false;
bool DUMP_VM_INST_STR = false;