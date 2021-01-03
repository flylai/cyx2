#include "ir_generator.h"
void COMPILER::IRGenerator::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
    IRInstruction inst;
    ptr->rhs->visit(this);
    if (cur_value.hasValue())
    {
        inst.operand1 = cur_value;
        cur_value.reset();
    }
    else
        inst.operand1 = consumeVariable();
    inst.operand1_type = IROperandType::VARIABLE;
    inst.opcode        = COMPILER::IRInstruction::token2IROp(ptr->op.keyword);

    inst.dest = newVariable();
    instructions.push_back(inst);
}

void COMPILER::IRGenerator::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
    IRInstruction inst;
    inst.opcode = COMPILER::IRInstruction::token2IROp(ptr->op.keyword);

    ptr->lhs->visit(this);
    if (cur_value.hasValue())
        inst.operand1 = cur_value, cur_value.reset();
    else
        inst.operand1 = consumeVariable();
    inst.operand1_type = IROperandType::VARIABLE;

    ptr->rhs->visit(this);
    if (cur_value.hasValue())
        inst.operand2 = cur_value, cur_value.reset();
    else
        inst.operand2 = consumeVariable();
    inst.operand2_type = IROperandType::VARIABLE;

    inst.dest = newVariable();
    instructions.push_back(inst);
}

void COMPILER::IRGenerator::visitIntExpr(COMPILER::IntExpr *ptr)
{
    cur_value = ptr->value;
}

void COMPILER::IRGenerator::visitDoubleExpr(COMPILER::DoubleExpr *ptr)
{
    cur_value = ptr->value;
}

void COMPILER::IRGenerator::visitStringExpr(COMPILER::StringExpr *ptr)
{
    cur_value = ptr->value;
}

void COMPILER::IRGenerator::visitAssignExpr(COMPILER::AssignExpr *ptr)
{
    IRInstruction inst;
    inst.opcode = IROpcode::IR_ASSIGN;

    ptr->lhs->visit(this);
    inst.dest = cur_value.as<std::string>();
    cur_value.reset();

    ptr->rhs->visit(this);
    if (cur_value.hasValue())
    {
        inst.operand1 = cur_value;
        cur_value.reset();
    }
    else
        inst.operand1 = consumeVariable();
    inst.operand1_type = IROperandType::VARIABLE;
    instructions.push_back(inst);
}

void COMPILER::IRGenerator::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
    cur_value = ptr->value;
}

void COMPILER::IRGenerator::visitFuncCallExpr(COMPILER::FuncCallExpr *ptr)
{
    IRInstruction inst_var;

    auto var_tmp = newVariable();

    inst_var.dest          = var_tmp;
    inst_var.opcode        = IROpcode::IR_ASSIGN;
    inst_var.operand1      = ptr->func_name;
    inst_var.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_var);
}

void COMPILER::IRGenerator::visitExprStmt(COMPILER::ExprStmt *ptr)
{
    ptr->expr->visit(this);
}

void COMPILER::IRGenerator::visitIfStmt(COMPILER::IfStmt *ptr)
{
    auto label_true  = newLabel();
    auto label_false = newLabel();
    auto label_out   = newLabel();
    //
    // JMP code
    //
    ptr->cond->visit(this);
    instructions.push_back(genIf());
    //
    instructions.push_back(genGoto(label_true));
    //
    instructions.push_back(genGoto(label_false));
    //
    // visit true/false block
    //
    instructions.push_back(genLabel(label_true));
    ptr->true_block->visit(this);
    // end of true block, goto end of if
    instructions.push_back(genGoto(label_out));
    // false block
    instructions.push_back(genLabel(label_false));
    if (ptr->false_block != nullptr) ptr->false_block->visit(this);
    // end of if, exit.
    instructions.push_back(genLabel(label_out));
}

void COMPILER::IRGenerator::visitForStmt(COMPILER::ForStmt *ptr)
{
    /*
    IR code similar to the following code.
    //
    @init
     t0=1
    @cond
     t2=t0<3
     if t2
     goto @body
     goto @out
    @final
     t0=t0+1
     goto @cond
    @body
     t1=t1+1
     goto @final
    @out
     ....
    */

    auto label_init  = newLabel();
    auto label_cond  = newLabel();
    auto label_final = newLabel();
    auto label_body  = newLabel();
    auto label_out   = newLabel();
    // init
    instructions.push_back(genLabel(label_init));
    ptr->init->visit(this);
    // cond
    instructions.push_back(genLabel(label_cond));
    ptr->cond->visit(this);
    // if
    instructions.push_back(genIf());
    // if true
    instructions.push_back(genGoto(label_body));
    // if false
    instructions.push_back(genGoto(label_out));
    // final
    instructions.push_back(genLabel(label_final));
    ptr->final->visit(this);

    instructions.push_back(genGoto(label_cond));
    // body
    instructions.push_back(genLabel(label_body));
    ptr->block->visit(this);

    instructions.push_back(genGoto(label_final));
    // out
    instructions.push_back(genLabel(label_out));
}

void COMPILER::IRGenerator::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
    auto label_cond = newLabel();
    auto label_body = newLabel();
    auto label_out  = newLabel();

    instructions.push_back(genLabel(label_cond));
    ptr->cond->visit(this);

    // if
    instructions.push_back(genIf());
    // if true
    instructions.push_back(genGoto(label_body));
    // if false
    instructions.push_back(genGoto(label_out));

    // body
    instructions.push_back(genLabel(label_body));
    ptr->block->visit(this);

    instructions.push_back(genGoto(label_cond));
    // out
    instructions.push_back(genLabel(label_out));
}

void COMPILER::IRGenerator::visitSwitchStmt(COMPILER::SwitchStmt *ptr)
{
}

void COMPILER::IRGenerator::visitMatchStmt(COMPILER::MatchStmt *ptr)
{
}

void COMPILER::IRGenerator::visitFuncDeclStmt(COMPILER::FuncDeclStmt *ptr)
{
    IRInstruction inst_func_name_label;

    auto func_name = newLabel(ptr->func_name->value);

    instructions.push_back(genLabel(func_name));

    // TODO: params

    ptr->block->visit(this);
}

void COMPILER::IRGenerator::visitBreakStmt(COMPILER::BreakStmt *ptr)
{
}

void COMPILER::IRGenerator::visitContinueStmt(COMPILER::ContinueStmt *ptr)
{
}

void COMPILER::IRGenerator::visitReturnStmt(COMPILER::ReturnStmt *ptr)
{
}

void COMPILER::IRGenerator::visitImportStmt(COMPILER::ImportStmt *ptr)
{
    IRInstruction inst;
    inst.opcode        = IROpcode::IR_IMPORT;
    inst.operand1      = ptr->path;
    inst.operand1_type = IROperandType::STRING;
    instructions.push_back(inst);
}

void COMPILER::IRGenerator::visitBlockStmt(COMPILER::BlockStmt *ptr)
{
    enterNewScope();

    for (auto &x : ptr->stmts)
    {
        x->visit(this);
    }

    exitScope();
}

void COMPILER::IRGenerator::enterNewScope()
{
    current_symbol = new SymbolTable(current_symbol);
}

void COMPILER::IRGenerator::exitScope()
{
    auto *tmp      = current_symbol;
    current_symbol = current_symbol->pre();
    delete tmp;
}

std::string COMPILER::IRGenerator::newVariable()
{
    tmp_vars.push("t" + std::to_string(var_cnt++));
    return tmp_vars.top();
}

std::string COMPILER::IRGenerator::newLabel()
{
    tmp_labels.push("@l" + std::to_string(label_cnt++));
    return tmp_labels.top();
}

std::string COMPILER::IRGenerator::newLabel(const std::string &label)
{
    std::string tmp = "@" + label;
    tmp_labels.push(tmp);
    return tmp;
}

std::string COMPILER::IRGenerator::consumeVariable()
{
    std::string retval = tmp_vars.top();
    tmp_vars.pop();
    return retval;
}

std::string COMPILER::IRGenerator::consumeLabel()
{
    auto retval = tmp_labels.top();
    tmp_labels.pop();
    return retval;
}

COMPILER::IRGenerator::IRGenerator()
{
    current_symbol = new SymbolTable();
}

std::string COMPILER::IRGenerator::irCodeString()
{
    std::string ir_code;
    for (auto x : instructions)
    {
        ir_code += x.toString() + "\n";
    }
    return ir_code;
}

void COMPILER::IRGenerator::visitTree(COMPILER::Tree *ptr)
{
    for (auto *x : ptr->stmts)
    {
        x->visit(this);
    }
}

COMPILER::IRInstruction COMPILER::IRGenerator::genGoto(const std::string &dest)
{
    IRInstruction inst;
    inst.opcode        = IROpcode::IR_GOTO;
    inst.operand1      = dest;
    inst.operand1_type = IROperandType::LABEL;
    return inst;
}

COMPILER::IRInstruction COMPILER::IRGenerator::genLabel(const std::string &label)
{
    IRInstruction inst;
    inst.opcode        = IROpcode::IR_LABEL;
    inst.operand1      = label;
    inst.operand1_type = IROperandType::LABEL;
    return inst;
}

COMPILER::IRInstruction COMPILER::IRGenerator::genIf()
{
    IRInstruction inst;
    inst.opcode        = IROpcode::IR_IF;
    inst.operand1      = consumeVariable();
    inst.operand1_type = IROperandType::STRING;
    return inst;
}
