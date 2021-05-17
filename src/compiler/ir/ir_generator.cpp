#include "ir_generator.h"

#define LINK(PRE, SUCC)                                                                                                \
    PRE->addSucc(SUCC);                                                                                                \
    SUCC->addPre(PRE)

#define POS(PTR) (std::to_string((PTR)->row) + ":" + std::to_string((PTR)->column))

void COMPILER::IRGenerator::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
    auto *assign = new IRAssign;

    assign->block = cur_basic_block;

    check_var_exist = true;
    ptr->rhs->visit(this);
    check_var_exist = false;

    if (inOr(ptr->op.keyword, SELFADD_PREFIX, SELFSUB_PREFIX, SELFADD_SUFFIX, SELFSUB_SUFFIX))
    {
        auto *binary    = new IRBinary;
        auto *self      = consumeVariable();
        auto *constant  = new IRConstant;
        constant->value = 1;

        // make a copy
        auto *self_copy        = new IRVar;
        self_copy->def         = self;
        self_copy->name        = self->name;
        self_copy->belong_inst = assign;
        self->addUse(self_copy);

        binary->lhs    = self_copy;
        binary->opcode = token2IROp(ptr->op.keyword);
        binary->rhs    = constant;
        assign->setDest(self);
        assign->setSrc(binary);

        if (inOr(ptr->op.keyword, SELFSUB_SUFFIX, SELFADD_SUFFIX))
        {
            auto *assign2  = new IRAssign;
            assign2->block = cur_basic_block;

            // make a copy
            auto *self_copy2        = new IRVar;
            self_copy2->def         = self;
            self_copy2->name        = self->name;
            self_copy2->belong_inst = assign2;
            self->addUse(self_copy2);

            assign2->setDest(newVariable());
            assign2->setSrc(self_copy2);
            cur_basic_block->addInst(assign2);
        }
        else
        {
            // must add a `use` to tmp_vars
            // irSimplify() will remove redundant var
            auto *use = new IRVar;
            use->name = self->name;
            use->def  = self;
            self->addUse(use);
            tmp_vars.push(use);
        }
    }
    else
    {
        if (cur_value.hasValue())
        {
            if (ptr->op.keyword == Keyword::SUB)
                cur_value = -cur_value;
            else if (ptr->op.keyword == Keyword::LNOT)
                cur_value = !cur_value;
            else if (ptr->op.keyword == Keyword::BNOT)
                cur_value = ~cur_value;
            else
                UNREACHABLE();

            auto *constant  = new IRConstant;
            constant->value = cur_value;
            cur_value.reset();
            assign->setSrc(constant);
        }
        else
        {
            auto binary = new IRBinary;
            if (ptr->op.keyword == Keyword::SUB)
            {
                auto *constant  = new IRConstant;
                constant->value = 0;
                binary->lhs     = constant;
            }

            binary->opcode = token2IROp(ptr->op.keyword);
            binary->rhs    = consumeVariable();
            assign->setSrc(binary);
        }
        assign->setDest(newVariable());
    }
    cur_basic_block->addInst(assign);
}

void COMPILER::IRGenerator::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
    auto *binary   = new IRBinary;
    auto *assign   = new IRAssign;
    binary->opcode = token2IROp(ptr->op.keyword);
    assign->setSrc(binary);
    assign->block = cur_basic_block;
    // lhs
    check_var_exist = true;
    ptr->lhs->visit(this);
    check_var_exist = false;
    // += -= *= /= %=
    if (inOr(ptr->op.keyword, Keyword::ADD_ASSIGN, Keyword::SUB_ASSIGN, Keyword::MUL_ASSIGN, Keyword::DIV_ASSIGN,
             Keyword::MOD_ASSIGN))
    {
        // ugly implementation, I will explain why I wrote that
        // for example
        // a = 1
        // a += 1
        // tmp_var -> [... a(origin)]
        // after following code execute tmp_var -> [...]
        auto *tmp = consumeVariable(false);
        // tmp_var -> [... a(origin)]
        tmp_vars.push(tmp);
        // tmp_var -> [...], assign->dest has a copy of a(origin)
        assign->setDest(consumeVariable());
        // SAME! tmp_var -> [... a(origin)]
        tmp_vars.push(tmp);
        // SAME! tmp_var -> [...], binary->lhs has a copy of a(origin)
        binary->lhs     = consumeVariable();
        check_var_exist = true;
        ptr->rhs->visit(this);
        check_var_exist = false;
        if (cur_value.hasValue())
        {
            auto *rhs   = new IRConstant;
            rhs->value  = cur_value;
            binary->rhs = rhs;
            cur_value.reset();
        }
        else
        {
            auto *rhs   = consumeVariable();
            binary->rhs = rhs;
        }
        cur_basic_block->addInst(assign);
        return;
    }
    //
    if (cur_value.hasValue())
    {
        // int / double / string
        auto *lhs   = new IRConstant;
        lhs->value  = cur_value;
        binary->lhs = lhs;
        cur_value.reset();
    }
    else
    {
        auto *lhs   = consumeVariable();
        binary->lhs = lhs;
    }
    // rhs
    check_var_exist = true;
    ptr->rhs->visit(this);
    check_var_exist = false;
    if (cur_value.hasValue())
    {
        auto *rhs   = new IRConstant;
        rhs->value  = cur_value;
        binary->rhs = rhs;
        cur_value.reset();
    }
    else
    {
        auto *rhs   = consumeVariable();
        binary->rhs = rhs;
    }

    assign->setDest(newVariable());
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
            CERR("twice defined! previous `" + ir_var->name + "` defined is function!!");
        }

        first_scan_vars[ir_var->name] = ir_var;
        return;
    }
    // actually assign is one of the binary expr
    auto *assign  = new IRAssign;
    assign->block = cur_basic_block;
    //
    check_var_exist = true;
    ptr->rhs->visit(this);
    check_var_exist = false;
    if (cur_value.hasValue())
    {
        auto *constant  = new IRConstant;
        constant->value = cur_value;
        assign->setSrc(constant);

        cur_value.reset();
    }
    else
    {
        auto *src = consumeVariable();
        assign->setSrc(src);
    }
    //
    ptr->lhs->visit(this);
    auto *dest = consumeVariable(false);
    assign->setDest(dest);

    cur_basic_block->addInst(assign);
}

void COMPILER::IRGenerator::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
    auto upval = cur_symbol->query(ptr->value);
    if (upval.type == Symbol::Type::VAR)
    {
        auto *var = new IRVar;
        var->name = ptr->value;
        var->def  = upval.var;

        tmp_vars.push(var);
    }
    else if (upval.type == Symbol::Type::INVALID)
    {
        if (check_var_exist)
        {
            CERR("cant find definition of `" + ptr->value + "` in " + POS(ptr));
        }
        auto *var_def = new IRVar;
        var_def->name = ptr->value;

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
    auto *inst = new IRCall;
    inst->name = ptr->func_name;

    bool is_buildin = buildin_functions.find("buildin_" + ptr->func_name) != buildin_functions.end();

    if (!is_buildin)
    {
        inst->name += "#" + std::to_string(ptr->args.size());
        if (first_scan_funcs.find(inst->name) == first_scan_funcs.end())
            CERR("can't find function definition of `" + ptr->func_name + "` in " + POS(ptr));

        inst->func = first_scan_funcs[inst->name]->ir_func;
    }

    int arg_cnt = 0;
    std::vector<IR *> tmp_arg;

    for (auto *arg : ptr->args)
    {
        check_var_exist = true;
        arg->visit(this);
        check_var_exist = false;

        if (cur_value.hasValue())
        {
            auto *constant  = new IRConstant;
            constant->value = cur_value;
            cur_value.reset();
            tmp_arg.push_back(constant);
            arg_cnt++;
        }
        else
        {
            tmp_arg.push_back(consumeVariable());
            arg_cnt++;
        }
    }
    // args
    for (; arg_cnt < ptr->args.size(); arg_cnt++)
    {
        inst->args.push_back(cur_basic_block->insts.back());
        cur_basic_block->insts.pop_back();
    }
    for (auto *arg : tmp_arg)
    {
        inst->args.push_back(arg);
    }

    inst->block  = cur_basic_block;
    auto *assign = new IRAssign;
    assign->setDest(newVariable());
    assign->setSrc(inst);
    cur_basic_block->insts.push_back(assign);
}

void COMPILER::IRGenerator::visitExprStmt(COMPILER::ExprStmt *ptr)
{
    if (ptr->expr == nullptr) return;
    ptr->expr->visit(this);
}

void COMPILER::IRGenerator::visitIfStmt(COMPILER::IfStmt *ptr)
{
    auto *cond_block = newBasicBlock();
    LINK(cur_basic_block, cond_block);
    cur_basic_block = cond_block;
    ptr->cond->visit(this);
    auto *cond = consumeVariable();
    //
    auto *true_block = newBasicBlock();
    LINK(cond_block, true_block);
    cur_basic_block = true_block;
    ptr->true_block->visit(this);
    auto *out_of_true = cur_basic_block;
    //
    auto *false_block = newBasicBlock();
    LINK(cond_block, false_block);
    cur_basic_block = false_block;
    if (ptr->false_block != nullptr) ptr->false_block->visit(this);
    auto *out_of_false = cur_basic_block;
    auto *out_block    = newBasicBlock();
    LINK(out_of_true, out_block);
    LINK(out_of_false, out_block);
    //
    cur_basic_block = out_block;
    //
    auto *branch        = new IRBranch;
    branch->true_block  = true_block;
    branch->false_block = false_block;
    branch->block       = cond_block;
    branch->cond        = cond;
    cond_block->addInst(branch);

    if (inOr(true_block->insts.back()->tag, IR::Tag::RETURN, IR::Tag::JMP)) return;
    // jump to the out block after executed true block instructions.
    auto *true_jmp   = new IRJump;
    true_jmp->block  = true_block;
    true_jmp->target = out_block;
    true_block->addInst(true_jmp);
}

void COMPILER::IRGenerator::visitForStmt(COMPILER::ForStmt *ptr)
{
    /*
    IR vm_insts similar to the following vm_insts.
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
    auto *init_block = newBasicBlock();
    //
    loop_stack.push_back(init_block);
    //
    LINK(cur_basic_block, init_block);
    cur_basic_block = init_block;
    ptr->init->visit(this);
    //
    auto *cond_block = newBasicBlock();
    LINK(cur_basic_block, cond_block);
    cur_basic_block = cond_block;
    ptr->cond->visit(this);

    auto *branch      = new IRBranch;
    auto *cond        = consumeVariable();
    cond->belong_inst = branch;
    branch->cond      = cond;
    cur_basic_block->addInst(branch);
    //
    auto *body_block = newBasicBlock();
    LINK(cur_basic_block, body_block);
    cur_basic_block = body_block;
    ptr->block->visit(this);
    //
    auto *final_block = newBasicBlock();
    LINK(cur_basic_block, final_block);
    LINK(final_block, cond_block);
    cur_basic_block = final_block;
    ptr->final->visit(this);

    branch->true_block = body_block;
    auto *jmp          = new IRJump;
    cur_basic_block->addInst(jmp);

    jmp->target = cond_block;
    //
    auto *out_block = newBasicBlock();
    LINK(cond_block, out_block);
    cur_basic_block     = out_block;
    branch->false_block = out_block;
    // loop
    out_block->loop_start = init_block;
    init_block->loop_end  = out_block;
    //
    loop_stack.pop_back();
}

void COMPILER::IRGenerator::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
    auto *cond_block = newBasicBlock();
    //
    loop_stack.push_back(cond_block);
    //
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
    //
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
    // loop
    cond_block->loop_end  = out_block;
    out_block->loop_start = cond_block;
    //
    loop_stack.pop_back();
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
    if (step != 1) return;

    auto *func    = new HIRFunction;
    func->ir_func = new IRFunction;
    func->name    = ptr->func_name->value;
    func->block   = ptr->block;
    for (const auto &param : ptr->params)
    {
        auto *var = new IRVar;
        var->name = param;
        func->params.push_back(var);
    }

    if (first_scan_vars.find(func->toString()) != first_scan_vars.end())
    {
        CERR("twice defined! previous `" + func->name + "` defined is variable!!");
    }
    if (first_scan_funcs.find(func->toString()) != first_scan_funcs.end())
    {
        CERR("twice defined! previous `" + func->name + "` has same signature!!");
    }
    first_scan_funcs[func->name == ENTRY_FUNC ? func->name : func->toString()] = func;
}

void COMPILER::IRGenerator::visitBreakStmt(COMPILER::BreakStmt *ptr)
{
    if (loop_stack.empty()) CERR("unexpected `break` in " + POS(ptr));
    auto *inst   = new IRJump;
    inst->target = loop_stack.back();
    fix_break_wait_list.push_back(inst);
    inst->block = cur_basic_block;
    cur_basic_block->addInst(inst);
}

void COMPILER::IRGenerator::visitContinueStmt(COMPILER::ContinueStmt *ptr)
{
    if (loop_stack.empty()) CERR("unexpected `continue` in " + POS(ptr));
    auto *inst   = new IRJump;
    inst->target = loop_stack.back();
    fix_continue_wait_list.push_back(inst);
    inst->block = cur_basic_block;
    cur_basic_block->addInst(inst);
}

void COMPILER::IRGenerator::visitReturnStmt(COMPILER::ReturnStmt *ptr)
{
    ptr->retval->visit(this);
    auto *inst = new IRReturn;
    if (cur_value.hasValue())
    {
        auto *constant  = new IRConstant;
        constant->value = cur_value;
        cur_value.reset();
        inst->ret = constant;
    }
    else if (ptr->retval->expr != nullptr)
    {
        auto *var = consumeVariable();
        inst->ret = var;
    }
    else
    {
        // return void
    }
    cur_basic_block->addInst(inst);
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

    auto *ir_var      = new IRVar;
    ir_var->name      = "t" + std::to_string(var_cnt++);
    ir_var->is_ir_gen = true;

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
    auto *tmp = tmp_vars.top();
    tmp_vars.pop();
    if (!force_IRVar || tmp->is_array)
    {
        return tmp;
    }
    auto *retval      = new IRVar;
    retval->name      = tmp->name;
    retval->is_ir_gen = tmp->is_ir_gen;

    // def-use.
    if (tmp->def == nullptr)
    {
        tmp->addUse(retval);
        retval->def = tmp;
    }
    else
    {
        retval->def = tmp->def;
        tmp->def->addUse(retval);
    }

    return retval;
}

COMPILER::IRGenerator::IRGenerator()
{
    cur_symbol = new SymbolTable(global_table);
}

std::string COMPILER::IRGenerator::irStr()
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
            ir_code += " " + std::to_string(block->phis.size()) + " phis and " + std::to_string(block->insts.size()) +
                       " inst(s)\n";

            for (auto *phi : block->phis)
            {
                addSpace(ir_code, 4);
                ir_code += phi->toString() + "\n";
            }

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
        symbol.type     = Symbol::Type::VAR;
        cur_basic_block = global_var_decl;
        x.second->rhs->visit(this);
        auto *var_def = new IRVar;
        var_def->name = x.first;

        auto *assign  = new IRAssign;
        assign->block = cur_basic_block;
        assign->setDest(var_def);

        if (cur_value.hasValue())
        {
            auto *constant  = new IRConstant;
            constant->value = cur_value;
            cur_value.reset();
            assign->setSrc(constant);

            symbol.var = var_def;
        }
        else
        {
            symbol.var = consumeVariable(false);
            assign->setSrc(symbol.var);
        }
        cur_symbol->upsert(x.first, symbol);
        global_var_decl->addInst(assign);
    }
    // func_name is only used to verify if the current func-name is ENTRY_FUNC
    for (const auto &[func_name, func_pointer] : first_scan_funcs)
    {
        loop_stack.clear();

        auto *func = func_pointer->ir_func;
        func->name = func_name != ENTRY_FUNC ? func_pointer->toString() : func_name;
        Symbol symbol;
        symbol.type = Symbol::Type::FUNC;
        symbol.func = func;
        cur_symbol->upsert(func->name, symbol);

        cur_func = func;
        funcs.push_back(func);

        cur_basic_block = newBasicBlock();

        enterNewScope();
        for (auto param : func_pointer->params)
        {
            Symbol param_symbol_table;
            param_symbol_table.type = Symbol::Type::VAR;
            param_symbol_table.var  = param;
            cur_symbol->upsert(param->name, param_symbol_table);

            func->params.push_back(param);
        }

        func_pointer->block->visit(this);
        exitScope();
        // fix continue stmt, when visit continue stmt, we cant known the out block of the loop
        fixBreakTarget();
        fixContinueTarget();
    }
    if (!NO_CODE_SIMPLIFY) simplifyIR();
    fixEdges();
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

void COMPILER::IRGenerator::simplifyIR()
{
    for (auto *func : funcs)
    {
        for (auto *block : func->blocks)
        {
            if (block->insts.size() < 2) continue;
            for (auto it = block->insts.begin(); it != block->insts.end();)
            {
                auto *tmp_cur  = *it;
                auto *tmp_next = ++it != block->insts.end() ? *it : nullptr; // next inst iterator
                if (tmp_next == nullptr) break;
                /* remove redundant ir like
                 * t0 = 1 + 2
                 * t1 = t0
                 */
                // two insts must be IRAssign
                // MAGIC
                if (tmp_cur->tag == IR::Tag::ASSIGN && tmp_next->tag == IR::Tag::ASSIGN)
                {
                    auto *cur  = static_cast<IRAssign *>(tmp_cur);
                    auto *next = static_cast<IRAssign *>(tmp_next);
                    if (next->src()->tag == IR::Tag::VAR && cur->dest()->def == nullptr)
                    {
                        auto *var = static_cast<IRVar *>(next->src());
                        if (static_cast<IRVar *>(cur->dest())->is_ir_gen && var->name == cur->dest()->name)
                        {
                            next->setSrc(cur->src());
                            it = block->insts.erase(--it); // it is pointing to `next` before --it.
                        }
                    }
                }
                else if (tmp_cur->tag == IR::Tag::JMP && tmp_next->tag == IR::Tag::JMP)
                {
                    // remove two identical IRJump stmt
                    /**
                     * jmp L1
                     * jmp L1 (should be removed)
                     */
                    auto *cur  = static_cast<IRJump *>(tmp_cur);
                    auto *next = static_cast<IRJump *>(tmp_next);
                    if (cur->target == next->target)
                    {
                        it = block->insts.erase(--it);
                    }
                }
                else if (tmp_cur->tag == IR::Tag::JMP)
                {
                    /**
                     * jmp L1 (following stmt should be removed)
                     * a = 1
                     * b = 2
                     */
                    for (auto &after_jmp_it = it; after_jmp_it != block->insts.end();)
                    {
                        // all instructions will be removed.
                        if (auto *tmp = as<IRAssign, IR::Tag::ASSIGN>(*after_jmp_it); tmp != nullptr)
                        {
                            COMPILER::forceRemoveVar(tmp->dest());
                            auto *binary = as<IRBinary, IR::Tag::BINARY>(tmp->src());
                            if (binary != nullptr)
                            {
                                // maybe lhs and rhs are not var but IRConstant.
                                if (!COMPILER::forceRemoveVar(as<IRVar, IR::Tag::VAR>(binary->lhs))) delete binary->lhs;
                                if (!COMPILER::forceRemoveVar(as<IRVar, IR::Tag::VAR>(binary->rhs))) delete binary->rhs;
                                delete binary;
                            }
                            delete tmp;
                        }
                        else if (auto *tmp = as<IRBranch, IR::Tag::BRANCH>(*after_jmp_it); tmp != nullptr)
                        {
                            COMPILER::forceRemoveVar(tmp->cond);
                            delete tmp;
                        }
                        else
                        {
                            delete *after_jmp_it;
                        }
                        after_jmp_it = block->insts.erase(after_jmp_it);
                    }
                }
            }
        }
    }
}

void COMPILER::IRGenerator::removeUnusedVarDef()
{
    // TODO: `branch` inst may use value
    for (auto *func : funcs)
    {
        // reverse traversal
        for (auto block_it = func->blocks.crbegin(); block_it != func->blocks.crend(); block_it++)
        {
            auto *block = *block_it;
            for (auto inst_it = block->insts.rbegin(); inst_it != block->insts.rend();)
            {
                auto *inst = *inst_it;
                if (inst->tag != IR::Tag::ASSIGN) return;
                if (auto *assign = as<IRAssign, IR::Tag::ASSIGN>(inst);
                    !assign->dest()->is_array && assign->dest()->def == nullptr && assign->dest()->use.empty())
                {
                    if (auto *binary = as<IRBinary, IR::Tag::BINARY>(assign->src()); binary != nullptr)
                    {
                        auto *lhs = binary->lhs;
                        auto *rhs = binary->rhs;
                        if (auto *lhs_var = as<IRVar, IR::Tag::VAR>(lhs); lhs_var != nullptr)
                        {
                            if (lhs_var->def != nullptr) lhs_var->def->killUse(lhs_var);
                        }
                        if (auto *rhs_var = as<IRVar, IR::Tag::VAR>(lhs); rhs_var != nullptr)
                        {
                            if (rhs_var->def != nullptr) rhs_var->def->killUse(rhs_var);
                        }
                        delete lhs;
                        delete rhs;
                        delete binary;
                    }
                    else if (auto *constant = as<IRConstant, IR::Tag::CONST>(assign->src()); constant != nullptr)
                    {
                        delete constant;
                    }
                    else if (auto *var = as<IRVar, IR::Tag::VAR>(assign->src()); var != nullptr)
                    {
                        if (var->def != nullptr) var->def->killUse(var);
                        delete assign->src();
                    }
                    else if (assign->src()->tag == IR::Tag::CALL)
                    {
                        inst_it++;
                        continue;
                    }
                    delete assign;
                    block->insts.erase(--inst_it.base());
                }
                else
                {
                    inst_it++;
                }
            }
        }
    }
}

void COMPILER::IRGenerator::fixBreakTarget()
{
    for (auto *inst : fix_break_wait_list)
    {
        auto *candidate_block = inst->target->loop_end;
        // if loop out block is empty block, it will be removed at CFG.simplifyCFG(), so we need find a not empty
        // succ block.
        while (candidate_block->insts.empty())
        {
            if (candidate_block->succs.empty()) break;

            candidate_block = *candidate_block->succs.begin();

            if (!candidate_block->insts.empty())
            {
                break;
            }
            else if (candidate_block->succs.size() > 1)
                LOGE("loop out block has two or more succs, it impossible!");
        }
        inst->target = candidate_block;
    }
}

void COMPILER::IRGenerator::fixContinueTarget()
{
    for (auto *inst : fix_continue_wait_list)
    {
        inst->target = *inst->block->succs.begin();
    }
}

void COMPILER::IRGenerator::destroyVar(IRVar *var)
{
    if (var == nullptr) return;
    // kill define
    if (var->def)
    {
        var->def->killUse(var);
    }
    if (!var->use.empty())
    {
        LOGE("this var is some vars' defined");
    }
    delete var;
}

void COMPILER::IRGenerator::visitArrayExpr(COMPILER::ArrayExpr *ptr)
{
    auto *array_expr = new IRArray;
    for (auto x : ptr->content)
    {
        x->visit(this);
        if (cur_value.hasValue())
        {
            auto *ir_const  = new IRConstant;
            ir_const->value = cur_value;
            array_expr->content.push_back(ir_const);
            cur_value.reset();
        }
        else
        {
            array_expr->content.push_back(consumeVariable());
        }
    }
    auto *var    = newVariable();
    auto *assign = new IRAssign;
    assign->setDest(var);
    assign->setSrc(array_expr);
    cur_basic_block->addInst(assign);
}

void COMPILER::IRGenerator::visitArrayIdExpr(COMPILER::ArrayIdExpr *ptr)
{
    auto *var     = new IRVar();
    var->is_array = true;
    var->name     = ptr->name;
    for (auto *idx : ptr->index)
    {
        idx->visit(this);
        if (cur_value.hasValue())
        {
            auto *ir_const  = new IRConstant;
            ir_const->value = cur_value;
            var->index.push_back(ir_const);
            cur_value.reset();
        }
        else
        {
            var->index.push_back(consumeVariable());
        }
    }
    tmp_vars.push(var);
}

void COMPILER::IRGenerator::fixEdges()
{
    // remove some edges that shouldn't be there
    for (auto *func : funcs)
    {
        // direct erase causes iterator to fail.
        // std::erase_if supported at c++2a
        std::vector<std::pair<BasicBlock *, BasicBlock *>> remove_list;
        for (auto *block : func->blocks)
        {
            for (auto *pre : block->pres)
            {
                if (pre->insts.empty()) continue;
                auto *jmp = as<IRJump, IR::Tag::JMP>(pre->insts.back());
                auto *ret = as<IRReturn, IR::Tag::RETURN>(pre->insts.back());
                if ((jmp == nullptr || jmp->target == block) && ret == nullptr) continue;
                // add to list
                remove_list.emplace_back(pre, block);
            }
        }
        for (const auto &x : remove_list)
        {
            // delete edge
            x.first->succs.erase(x.second);
            x.second->pres.erase(x.first);
            // add new edge
            auto *tmp = as<IRJump, IR::Tag::JMP>(x.first->insts.back());
            if (tmp != nullptr)
            {
                LINK(tmp->block, tmp->target);
            }
        }
    }
}

#undef LINK
#undef POS