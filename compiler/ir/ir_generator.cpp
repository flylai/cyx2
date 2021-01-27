#include "ir_generator.h"

#define LINK(PRE, SUCC)                                                                                                \
    PRE->addSucc(SUCC);                                                                                                \
    SUCC->addPre(PRE)

void COMPILER::IRGenerator::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
    auto *assign = new IRAssign;
    auto *binary = new IRBinary;

    check_var_exist = true;
    ptr->rhs->visit(this);
    check_var_exist = false;

    if (inOr(ptr->op.keyword, SELFADD_PREFIX, SELFSUB_PREFIX, SELFADD_SUFFIX, SELFSUB_SUFFIX))
    {
        auto *self      = consumeVariable();
        auto *constant  = new IRConstant;
        constant->value = 1;

        assign->dest   = self;
        binary->lhs    = self;
        binary->opcode = token2IROp(ptr->op.keyword);
        binary->rhs    = constant;
        assign->dest   = self;
        assign->src    = binary;

        if (inOr(ptr->op.keyword, SELFSUB_SUFFIX, SELFADD_SUFFIX))
        {
            auto *assign2   = new IRAssign;
            auto *binary2   = new IRBinary;
            binary2->lhs    = self;
            binary2->opcode = token2IROp(ptr->op.keyword);
            assign2->dest   = newVariable();
            assign2->src    = binary2;
            cur_basic_block->addInst(assign2);
        }
    }
    else
    {
        binary->opcode = token2IROp(ptr->op.keyword);
        binary->rhs    = consumeVariable();

        assign->dest = newVariable();
        assign->src  = binary;
    }
    cur_basic_block->addInst(assign);
}

void COMPILER::IRGenerator::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
    auto *binary   = new IRBinary;
    auto *assign   = new IRAssign;
    binary->opcode = token2IROp(ptr->op.keyword);
    assign->src    = binary;
    //
    check_var_exist = true;
    ptr->lhs->visit(this);
    check_var_exist = false;
    if (cur_value.hasValue())
    {
        // int / double / string
        auto *lhs  = new IRConstant;
        lhs->value = std::move(cur_value);
        cur_value.reset();
        binary->lhs = lhs;
    }
    else
    {
        binary->lhs = consumeVariable();
    }
    //
    check_var_exist = true;
    ptr->rhs->visit(this);
    check_var_exist = false;
    if (cur_value.hasValue())
    {
        auto *rhs  = new IRConstant;
        rhs->value = std::move(cur_value);
        cur_value.reset();
        binary->rhs = rhs;
    }
    else
    {
        binary->rhs = consumeVariable();
    }

    assign->dest = newVariable();
    cur_basic_block->addInst(assign);
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
    if (step == 1)
    {
        auto *ir_var = new HIRVar;
        ir_var->name = static_cast<IdentifierExpr *>(ptr->lhs)->value;
        ir_var->rhs  = ptr->rhs;

        if (first_scan_funcs.find(ir_var->name) != first_scan_funcs.end())
        {
            ERROR("twice defined! previous " + ir_var->name + " defined is function!!");
        }

        first_scan_vars[ir_var->name] = ir_var;
        return;
    }
    // actually assign is one of the binary expr
    auto *assign = new IRAssign;
    //
    ptr->lhs->visit(this);
    assign->dest = consumeVariable(false);
    //
    check_var_exist = true;
    ptr->rhs->visit(this);
    check_var_exist = false;
    if (cur_value.hasValue())
    {
        auto *constant  = new IRConstant;
        constant->value = std::move(cur_value);
        cur_value.reset();
        assign->src = constant;
    }
    else
    {
        assign->src = consumeVariable();
    }
    cur_basic_block->addInst(assign);
}

void COMPILER::IRGenerator::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
    auto upval = cur_symbol->query(ptr->value);
    if (upval.type == Symbol::Type::VAR)
    {
        auto *var  = new IRVar;
        var->name  = ptr->value;
        var->def   = upval.var;
        var->block = cur_basic_block;
        upval.var->use->addUse(var);

        tmp_vars.push(var);
    }
    else if (upval.type == Symbol::Type::INVALID)
    {
        if (check_var_exist)
        {
            ERROR("cant find definition of " + ptr->value);
        }
        auto *var_def  = new IRVarDef;
        var_def->name  = ptr->value;
        var_def->block = cur_basic_block;

        Symbol symbol;
        symbol.type = Symbol::Type::VAR;
        symbol.var  = var_def;
        cur_symbol->upsert(var_def->name, symbol);
        tmp_vars.push(var_def);
        //
        check_var_exist = false;
    }
}

void COMPILER::IRGenerator::visitFuncCallExpr(COMPILER::FuncCallExpr *ptr)
{
    //    auto *inst = new IRInstruction;
    //
    //    auto var_tmp = newVariable();
    //
    //    inst->dest          = var_tmp;
    //    inst->opcode        = IROpcode::IR_ASSIGN;
    //    inst->lhs      = ptr->func_name;
    //    inst->operand1_type = IROperandType::LABEL;
    //    instructions.push_back(inst);
}

void COMPILER::IRGenerator::visitExprStmt(COMPILER::ExprStmt *ptr)
{
    ptr->expr->visit(this);
}

void COMPILER::IRGenerator::visitIfStmt(COMPILER::IfStmt *ptr)
{
    auto *out_block  = newBasicBlock();
    auto *cond_block = newBasicBlock();
    LINK(cur_basic_block, cond_block);
    cur_basic_block = cond_block;
    ptr->cond->visit(this);
    //
    auto *true_block = newBasicBlock();
    LINK(cond_block, true_block);
    cur_basic_block = true_block;
    ptr->true_block->visit(this);
    LINK(cur_basic_block, out_block);
    //
    auto *false_block = newBasicBlock();
    LINK(cond_block, false_block);
    cur_basic_block = false_block;
    if (ptr->false_block != nullptr) ptr->false_block->visit(this);
    LINK(cur_basic_block, out_block);
    //
    cur_basic_block = out_block;
    //
    auto *branch        = new IRBranch;
    branch->true_block  = true_block;
    branch->false_block = false_block;
    branch->block       = cond_block;
    branch->cond        = consumeVariable();
    cond_block->addInst(branch);
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
    auto *init_block  = newBasicBlock();
    auto *cond_block  = newBasicBlock();
    auto *body_block  = newBasicBlock();
    auto *final_block = newBasicBlock();
    auto *out_block   = newBasicBlock();
    // CFG LINK
    LINK(cur_basic_block, init_block);
    LINK(init_block, cond_block);
    LINK(cond_block, body_block);
    LINK(body_block, final_block);
    LINK(cond_block, out_block);
    //
    cur_basic_block = init_block;
    ptr->init->visit(this);
    //
    cur_basic_block = cond_block;
    ptr->cond->visit(this);
    auto *branch        = new IRBranch;
    branch->block       = cur_basic_block;
    branch->cond        = consumeVariable();
    branch->true_block  = body_block;
    branch->false_block = out_block;
    cur_basic_block->addInst(branch);
    //
    cur_basic_block = final_block;
    ptr->final->visit(this);
    auto *final_jmp   = new IRJump;
    final_jmp->target = cond_block;
    cur_basic_block->addInst(final_jmp);
    //
    cur_basic_block = body_block;
    ptr->block->visit(this);
    auto *body_jmp   = new IRJump;
    body_jmp->target = final_block;
    cur_basic_block->addInst(body_jmp);
    //
    cur_basic_block = out_block;
}

void COMPILER::IRGenerator::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
    auto *cond_block = newBasicBlock();
    LINK(cur_basic_block, cond_block);
    cur_basic_block = cond_block;
    ptr->cond->visit(this);
    auto *branch  = new IRBranch;
    branch->block = cur_basic_block;
    branch->cond  = consumeVariable();

    auto *body_block = newBasicBlock();
    LINK(cond_block, body_block);
    cur_basic_block = body_block;
    ptr->block->visit(this);
    auto *jmp   = new IRJump;
    jmp->target = cond_block;
    cur_basic_block->addInst(jmp);
    LINK(cur_basic_block, cond_block);

    auto *out_block = newBasicBlock();
    LINK(cond_block, out_block);

    branch->true_block  = body_block;
    branch->false_block = out_block;
    cond_block->addInst(branch);

    cur_basic_block = out_block;
}

void COMPILER::IRGenerator::visitSwitchStmt(COMPILER::SwitchStmt *ptr)
{
}

void COMPILER::IRGenerator::visitMatchStmt(COMPILER::MatchStmt *ptr)
{
}

void COMPILER::IRGenerator::visitFuncDeclStmt(COMPILER::FuncDeclStmt *ptr)
{
    // first scan...
    if (step == 1)
    {
        auto *ir_func  = new HIRFunction;
        ir_func->name  = ptr->func_name->value;
        ir_func->block = ptr->block;
        for (const auto &param : ptr->params)
        {
            auto *var = new IRVarDef;
            var->name = param;
            ir_func->params.push_back(var);
        }

        if (first_scan_vars.find(ir_func->name) != first_scan_vars.end())
        {
            ERROR("twice defined! previous " + ir_func->name + " defined is variable!!");
        }
        first_scan_funcs[ir_func->name] = ir_func;
    }
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
    cur_symbol = new SymbolTable(cur_symbol);
}

void COMPILER::IRGenerator::exitScope()
{
    auto *tmp  = cur_symbol;
    cur_symbol = cur_symbol->pre;
    delete tmp;
}

COMPILER::IRVar *COMPILER::IRGenerator::newVariable()
{
    Symbol symbol;
    symbol.type = Symbol::Type::VAR;

    auto *ir_var  = new IRVarDef;
    ir_var->name  = "t" + std::to_string(var_cnt++);
    ir_var->block = cur_basic_block;

    symbol.var = ir_var;

    cur_symbol->upsert(ir_var->name, symbol);
    tmp_vars.push(ir_var);
    return tmp_vars.top();
}

std::string COMPILER::IRGenerator::newLabel()
{
    return "L" + std::to_string(label_cnt++);
}

COMPILER::IRVar *COMPILER::IRGenerator::consumeVariable(bool force_IRVar)
{
    if (!force_IRVar)
    {
        auto *retval = tmp_vars.top();
        tmp_vars.pop();
        return retval;
    }

    auto *var_def = tmp_vars.top(); // variable definition
    tmp_vars.pop();

    auto *retval  = new IRVar;
    retval->block = cur_basic_block;
    retval->name  = var_def->name;

    // def-use
    // if *var_def* is IRVarDef.
    if (typeid(*var_def) == typeid(IRVarDef))
    {
        ((IRVarDef *) var_def)->use->addUse(retval);
        retval->def = static_cast<IRVarDef *>(var_def);
    }
    // else -> var_def is IRVar, that is use, not definition. no need to add def-use chain

    return retval;
}

COMPILER::IRGenerator::IRGenerator()
{
    cur_symbol = new SymbolTable(global_table);
}

std::string COMPILER::IRGenerator::irCodeString()
{
    std::string ir_code = "There are " + std::to_string(global_var_decl->insts.size()) + " variable(s) declared!\n";
    for (auto *var : global_var_decl->insts)
    {
        addSpace(ir_code, 2);
        ir_code += var->toString() + "\n";
    }
    ir_code += "\n";

    ir_code += "There are " + std::to_string(funcs.size()) + " function(s) declared!\n";
    for (const auto *func : funcs)
    {
        ir_code += "@FUNC_" + func->name + " ";
        ir_code += std::to_string(func->params.size()) + " param(s) -> (";
        for (const auto *param : func->params)
        {
            ir_code += param->name + ", ";
        }
        ir_code += ")\n";
        for (auto *block : func->blocks)
        {
            addSpace(ir_code, 2);
            ir_code += "@" + block->name;
            ir_code += " " + std::to_string(block->insts.size()) + " inst(s)\n";
            for (auto *inst : block->insts)
            {
                addSpace(ir_code, 4);
                ir_code += inst->toString() + "\n";
            }
        }
        ir_code += "\n";
    }
    return ir_code;
}

void COMPILER::IRGenerator::visitTree(COMPILER::Tree *ptr)
{
    if (cur_basic_block == nullptr) cur_basic_block = newBasicBlock();
    for (auto *x : ptr->stmts)
    {
        x->visit(this);
    }
    step            = 0;
    global_var_decl = new BasicBlock();
    for (const auto &x : first_scan_vars)
    {
        Symbol symbol;
        symbol.type = Symbol::Type::VAR;

        x.second->rhs->visit(this);
        auto *var_def = new IRVarDef;
        var_def->name = x.first;

        auto *assign = new IRAssign;
        assign->dest = var_def;

        if (cur_value.hasValue())
        {
            auto *constant  = new IRConstant;
            constant->value = std::move(cur_value);
            cur_value.reset();
            assign->src = constant;

            symbol.var = var_def;
        }
        else
        {
            symbol.var  = static_cast<IRVarDef *>(consumeVariable(false));
            assign->src = symbol.var;
        }
        cur_symbol->upsert(x.first, symbol);
        global_var_decl->addInst(assign);
    }
    for (const auto &x : first_scan_funcs)
    {
        auto *func = new IRFunction;
        func->name = x.first;
        Symbol symbol;
        symbol.type = Symbol::Type::FUNC;
        symbol.func = func;
        cur_symbol->upsert(func->name, symbol);

        cur_func = func;
        funcs.push_back(func);

        cur_basic_block = newBasicBlock();

        enterNewScope();
        for (auto param : x.second->params)
        {
            Symbol param_symbol_table;
            param_symbol_table.type = Symbol::Type::VAR;
            param_symbol_table.var  = param;
            cur_symbol->upsert(param->name, param_symbol_table);

            func->params.push_back(param);
        }

        x.second->block->visit(this);
        exitScope();
    }
}

COMPILER::IRGenerator::~IRGenerator()
{
    // TODO
}
COMPILER::BasicBlock *COMPILER::IRGenerator::newBasicBlock(const std::string &name)
{
    auto *bb = name.empty() ? new BasicBlock(newLabel()) : new BasicBlock(name);
    if (cur_func != nullptr)
    {
        cur_func->blocks.push_back(bb);
    }
    return bb;
}

#undef LINK