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
    IRInstruction inst_cond;
    IRInstruction inst_goto_true_label;
    IRInstruction inst_goto_false_label;
    IRInstruction inst_goto_out_label;
    IRInstruction inst_true_label;
    IRInstruction inst_false_label;
    IRInstruction inst_out_label;

    auto label_true  = newLabel();
    auto label_false = newLabel();
    auto label_out   = newLabel();
    //
    // JMP code
    //
    inst_cond.opcode = IROpcode::IR_IF;
    ptr->cond->visit(this);
    inst_cond.operand1      = consumeVariable();
    inst_cond.operand1_type = IROperandType::STRING;
    instructions.push_back(inst_cond);
    //
    inst_goto_true_label.opcode        = IROpcode::IR_GOTO;
    inst_goto_true_label.operand1      = label_true;
    inst_goto_true_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_true_label);
    //
    inst_goto_false_label.opcode        = IROpcode::IR_GOTO;
    inst_goto_false_label.operand1      = label_false;
    inst_goto_false_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_false_label);
    //
    // visit true/false block
    //
    // true block
    inst_true_label.opcode        = IROpcode::IR_LABEL;
    inst_true_label.operand1      = label_true;
    inst_true_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_true_label);
    ptr->true_block->visit(this);
    // end of true block, goto end of if
    inst_goto_out_label.opcode        = IROpcode::IR_GOTO;
    inst_goto_out_label.operand1      = label_out;
    inst_goto_out_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_out_label);
    // false block
    inst_false_label.opcode        = IROpcode::IR_LABEL;
    inst_false_label.operand1      = label_false;
    inst_false_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_false_label);
    if (ptr->false_block != nullptr) ptr->false_block->visit(this);
    // end of if, exit.
    inst_out_label.opcode        = IROpcode::IR_LABEL;
    inst_out_label.operand1      = label_out;
    inst_out_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_out_label);
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
    IRInstruction inst_init_label;
    IRInstruction inst_cond_label;
    IRInstruction inst_final_label;
    IRInstruction inst_body_label;
    IRInstruction inst_out_label;
    IRInstruction inst_if;
    IRInstruction inst_goto_cond;
    IRInstruction inst_goto_final;
    IRInstruction inst_goto_body;
    IRInstruction inst_goto_out;

    auto label_init  = newLabel();
    auto label_cond  = newLabel();
    auto label_final = newLabel();
    auto label_body  = newLabel();
    auto label_out   = newLabel();
    // init
    inst_init_label.opcode        = IROpcode::IR_LABEL;
    inst_init_label.operand1      = label_init;
    inst_init_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_init_label);
    ptr->init->visit(this);
    // cond
    inst_cond_label.opcode        = IROpcode::IR_LABEL;
    inst_cond_label.operand1      = label_cond;
    inst_cond_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_cond_label);
    ptr->cond->visit(this);
    // if
    inst_if.opcode        = IROpcode::IR_IF;
    inst_if.operand1      = consumeVariable();
    inst_if.operand1_type = IROperandType::STRING;
    instructions.push_back(inst_if);
    // if true
    inst_goto_body.opcode        = IROpcode::IR_GOTO;
    inst_goto_body.operand1      = label_body;
    inst_goto_body.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_body);
    // if false
    inst_goto_out.opcode        = IROpcode::IR_GOTO;
    inst_goto_out.operand1      = label_out;
    inst_goto_out.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_out);
    // final
    inst_final_label.opcode        = IROpcode::IR_LABEL;
    inst_final_label.operand1      = label_final;
    inst_final_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_final_label);
    ptr->final->visit(this);

    inst_goto_cond.opcode        = IROpcode::IR_GOTO;
    inst_goto_cond.operand1      = label_cond;
    inst_goto_cond.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_cond);
    // body
    inst_body_label.opcode        = IROpcode::IR_LABEL;
    inst_body_label.operand1      = label_body;
    inst_body_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_body_label);
    ptr->block->visit(this);

    inst_goto_final.opcode        = IROpcode::IR_GOTO;
    inst_goto_final.operand1      = label_final;
    inst_goto_final.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_final);
    // out
    inst_out_label.opcode        = IROpcode::IR_LABEL;
    inst_out_label.operand1      = label_out;
    inst_out_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_out_label);
}

void COMPILER::IRGenerator::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
    IRInstruction inst_cond_label;
    IRInstruction inst_body_label;
    IRInstruction inst_out_label;
    IRInstruction inst_if;
    IRInstruction inst_goto_body;
    IRInstruction inst_goto_cond;
    IRInstruction inst_goto_out;

    auto label_cond = newLabel();
    auto label_body = newLabel();
    auto label_out  = newLabel();

    inst_cond_label.opcode        = IROpcode::IR_LABEL;
    inst_cond_label.operand1      = label_cond;
    inst_cond_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_cond_label);
    ptr->cond->visit(this);

    // if
    inst_if.opcode        = IROpcode::IR_IF;
    inst_if.operand1      = consumeVariable();
    inst_if.operand1_type = IROperandType::STRING;
    instructions.push_back(inst_if);
    // if true
    inst_goto_body.opcode        = IROpcode::IR_GOTO;
    inst_goto_body.operand1      = label_body;
    inst_goto_body.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_body);
    // if false
    inst_goto_out.opcode        = IROpcode::IR_GOTO;
    inst_goto_out.operand1      = label_out;
    inst_goto_out.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_out);

    // body
    inst_body_label.opcode        = IROpcode::IR_LABEL;
    inst_body_label.operand1      = label_body;
    inst_body_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_body_label);
    ptr->block->visit(this);

    inst_goto_cond.opcode        = IROpcode::IR_GOTO;
    inst_goto_cond.operand1      = label_cond;
    inst_goto_cond.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_goto_cond);
    // out
    inst_out_label.opcode        = IROpcode::IR_LABEL;
    inst_out_label.operand1      = label_out;
    inst_out_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_out_label);
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

    inst_func_name_label.opcode        = IROpcode::IR_LABEL;
    inst_func_name_label.operand1      = newLabel("@" + ptr->func_name->value);
    inst_func_name_label.operand1_type = IROperandType::LABEL;
    instructions.push_back(inst_func_name_label);

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