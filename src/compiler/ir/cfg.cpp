#include "cfg.h"

void COMPILER::CFG::init(IRFunction *func)
{
    for (auto block : func->blocks)
    {
        sdom[block]             = block;
        disjoint_set[block]     = block;
        disjoint_set_val[block] = block;
    }
}

void COMPILER::CFG::clear()
{
    sdom.clear();
    dfn.clear();
    dfn_map.clear();
    father.clear();
    visited.clear();
    sdom.clear();
    idom.clear();
    tree.clear();
    dominance_frontier.clear();
    disjoint_set.clear();
    disjoint_set_val.clear();
    var_block_map.clear();
    counter.clear();
    stack.clear();
    ssa_def_map.clear();
}

void COMPILER::CFG::dfs(COMPILER::BasicBlock *cur_basic_block)
{
    if (cur_basic_block == nullptr) return;
    if (visited.find(cur_basic_block) != visited.end()) return;
    visited[cur_basic_block] = true;
    dfn.push_back(cur_basic_block);
    dfn_map[cur_basic_block] = dfn.size() - 1;
    for (auto *block : cur_basic_block->succs)
    {
        dfs(block);
        father[block] = cur_basic_block;
    }
}

COMPILER::BasicBlock *COMPILER::CFG::find(COMPILER::BasicBlock *block)
{
    if (block == disjoint_set[block]) return block;
    auto *tmp = find(disjoint_set[block]);
    if (dfn_map[sdom[disjoint_set_val[disjoint_set[block]]]] < dfn_map[sdom[disjoint_set_val[block]]])
        disjoint_set_val[block] = disjoint_set_val[disjoint_set[block]];
    disjoint_set[block] = tmp;
    return tmp;
}

void COMPILER::CFG::tarjan()
{
    for (unsigned long i = dfn.size() - 1; i >= 1; i--)
    {
        auto *cur_block = dfn[i];
        if (cur_block == nullptr) continue;
        for (auto *pre_block : cur_block->pres)
        {
            if (dfn_map.find(pre_block) != dfn_map.end())
            {
                find(cur_block);
                int a           = dfn_map[sdom[cur_block]];
                int b           = dfn_map[sdom[disjoint_set_val[pre_block]]];
                sdom[cur_block] = dfn[std::min(a, b)];
            }
        }
        tree[sdom[cur_block]].insert(cur_block);
        disjoint_set[cur_block] = father[cur_block];
        auto *tmp               = disjoint_set[cur_block];
        for (auto *block : tree[tmp])
        {
            find(block);
            if (dfn_map[sdom[disjoint_set_val[block]]] < dfn_map[father[cur_block]])
                idom[block] = disjoint_set_val[block];
            else
                idom[block] = father[cur_block];
        }
        tree[tmp].clear();
    }
    for (int i = 1; i < dfn.size(); i++)
    {
        auto *cur_block = dfn[i];
        if (idom[cur_block] != sdom[cur_block]) idom[cur_block] = idom[idom[cur_block]];
        tree[idom[cur_block]].insert(cur_block);
    }
}

void COMPILER::CFG::buildDominateTree(COMPILER::IRFunction *func)
{
    clear();
    init(func);
    dfs(func->blocks.front());
    tarjan();
    calcDominanceFrontier(func);
}

void COMPILER::CFG::calcDominanceFrontier(COMPILER::IRFunction *func)
{
    for (auto *block : func->blocks)
    {
        if (block->pres.size() < 2) continue;
        for (auto *pre : block->pres)
        {
            auto *runner = pre;
            while (runner != idom[block] && runner != nullptr)
            {
                dominance_frontier[runner].insert(block);
                runner = idom[runner];
            }
        }
    }
}

void COMPILER::CFG::simplifyCFG()
{
    for (auto *func : funcs)
    {
        for (auto it = func->blocks.begin(); it != func->blocks.end();)
        {
            auto *block = *it;
            if (block->insts.empty() && !block->pres.empty() && block->succs.size() == 1)
            {
                // remove it!
                auto *succ = *block->succs.begin();
                succ->pres.erase(block);
                for (auto *pre : block->pres)
                {
                    // remove block from pred's succs
                    pre->succs.erase(block);
                    pre->succs.insert(succ);
                    succ->pres.insert(pre);
                    // remove block from pred's insts
                    for (auto *inst : pre->insts)
                    {
                        if (inOr(inst->tag, IR::Tag::BRANCH, IR::Tag::JMP))
                        {
                            if (inst->tag == IR::Tag::BRANCH)
                            {
                                auto *tmp = static_cast<IRBranch *>(inst);
                                if (tmp->true_block == block) tmp->true_block = succ;
                                if (tmp->false_block == block) tmp->false_block = succ;
                            }
                            else
                            {
                                auto *tmp = static_cast<IRJump *>(inst);
                                if (tmp->target == block) tmp->target = succ;
                            }
                        }
                    }
                }
                it = func->blocks.erase(it);
                delete block;
            }
            else if (it != func->blocks.begin() && block->pres.empty())
            {
                // remove this block, it isn't the entry block and it has succ blocks
                // remove all instructions
                for (auto inst : block->insts)
                {
                    if (auto *tmp = as<IRAssign, IR::Tag::ASSIGN>(inst); tmp != nullptr)
                    {
                        COMPILER::forceRemoveVar(tmp->dest());
                        auto *binary = as<IRBinary, IR::Tag::BINARY>(tmp->src());
                        if (binary != nullptr)
                        {
                            // maybe lhs and rhs are not var but IRConstant.
                            if (!forceRemoveVar(as<IRVar, IR::Tag::VAR>(binary->lhs))) delete binary->lhs;
                            if (!forceRemoveVar(as<IRVar, IR::Tag::VAR>(binary->rhs))) delete binary->rhs;
                            delete binary;
                        }
                        delete tmp;
                    }
                    else if (auto *tmp = as<IRBranch, IR::Tag::BRANCH>(inst); tmp != nullptr)
                    {
                        forceRemoveVar(tmp->cond);
                        delete tmp;
                    }
                    else
                    {
                        delete inst;
                    }
                }
                // remove succs edges
                for (auto succ : block->succs)
                {
                    succ->pres.erase(block);
                }
                // remove it from block list
                it = func->blocks.erase(it);
                delete block;
            }
            else
            {
                it++;
            }
        }
        // append missing return inst
        auto *end_block = func->blocks.back();
        if (end_block->insts.empty() || end_block->insts.back()->tag != IR::Tag::RETURN)
        {
            end_block->addInst(new IRReturn);
        }
    }
}

void COMPILER::CFG::removeUnusedPhis(IRFunction *func)
{
    for (auto *block : func->blocks)
    {
        for (auto phi_it = block->phis.begin(); phi_it != block->phis.end();)
        {
            auto *phi_assign = *phi_it;
            if (phi_assign->dest()->use.empty())
                phi_it = block->phis.erase(phi_it);
            else
                phi_it++;
        }
    }
}

void COMPILER::CFG::transformToSSA()
{
    if (NO_SSA) return;
    for (auto *func : funcs)
    {
        if (func->blocks.empty()) continue;
        entry = func->blocks.front();
        buildDominateTree(func);

        var_block_map.clear();
        collectVarAssign(func);
        insertPhiNode();
        tryRename(func);
        removeUnusedPhis(func);
        removeTrivialPhi(func);
        if (CONSTANT_FOLDING)
        {
            constantFolding(func);
            if (CONSTANT_PROPAGATION) constantPropagation(func);
            removeUnusedPhis(func);
        }
        phiElimination(func);
        if (DEAD_CODE_ELIMINATION) deadCodeElimination(func);
    }
}

void COMPILER::CFG::collectVarAssign(COMPILER::IRFunction *func)
{
    for (auto *block : func->blocks)
    {
        for (auto *inst : block->insts)
        {
            if (inst->tag == IR::Tag::ASSIGN)
            {
                auto *assign = static_cast<IRAssign *>(inst);
                if (assign->dest()->def == nullptr && assign->dest()->is_ir_gen) continue;
                var_block_map[assign->dest()->name].insert(block);
            }
        }
    }
}

void COMPILER::CFG::insertPhiNode()
{
    std::queue<BasicBlock *> work_list;
    std::unordered_map<BasicBlock *, std::string> inserted;

    for (const auto &p : var_block_map)
    {
        const std::string var_name = p.first;
        for (auto *block : p.second)
        {
            work_list.push(block);
        }
        while (!work_list.empty())
        {
            auto *block = work_list.front();
            work_list.pop();

            for (auto *df_block : dominance_frontier[block])
            {
                if (inserted[df_block] != var_name)
                {
                    // add phi node
                    inserted[df_block] = var_name;
                    auto *assign       = new IRAssign;
                    //
                    auto *lhs = new IRVar;
                    lhs->name = var_name;
                    //
                    auto *phi = new IRPhi;
                    //
                    assign->setDest(lhs);
                    assign->setSrc(phi);
                    assign->block = df_block;
                    df_block->phis.push_back(assign);
                    // add block's dominance frontier to worklist
                    work_list.push(df_block);
                }
            }
        }
    }
}

void COMPILER::CFG::removeTrivialPhi(COMPILER::IRFunction *func)
{
    // remove phi node like x2 = phi(x1, x1)
    for (auto *block : func->blocks)
    {
        for (auto it = block->phis.begin(); it != block->phis.end();)
        {
            // check phi node
            auto *assign       = as<IRAssign, IR::Tag::ASSIGN>(*it);
            auto *phi          = as<IRPhi, IR::Tag::PHI>(assign->src());
            int pre_idx        = -1;
            int same_idx_count = 1;
            //
            IRVar *def{ nullptr };
            int def_ssa_index = -1;
            for (auto *tmp : phi->args)
            {
                auto *var = as<IRVar, IR::Tag::VAR>(tmp);
                if (pre_idx == -1)
                {
                    pre_idx = var->ssa_index;
                    continue;
                }
                if (var->ssa_index == pre_idx)
                {
                    same_idx_count++;
                    def           = var->def;
                    def_ssa_index = var->ssa_index;
                }
                else
                    break;
            }
            // if found phi(x1, x1, x1, x1....), remove it~
            if (same_idx_count == phi->args.size())
            {
                // replace uses to x1
                for (auto use_it = assign->dest()->use.begin(); use_it != assign->dest()->use.end();)
                {
                    auto *use      = *use_it;
                    use->def       = def;
                    use->name      = def->name;
                    use->ssa_index = def_ssa_index;
                    def->addUse(use);
                    use_it = assign->dest()->use.erase(use_it);
                }
                // remove
                it = block->phis.erase(it);
                destroyPhiNode(assign);
            }
            else
                it++;
        }
    }
}

void COMPILER::CFG::constantPropagation(COMPILER::IRFunction *func)
{
    // constant propagation will be executed after constant folding.
    // find some variables like following
    // a1 = 1
    // a3 = phi(a1, a2)
    // -> a3 = phi(1, a2)
    for (auto block : func->blocks)
    {
        for (auto *assign : block->phis)
        {
            auto *phi = as<IRPhi, IR::Tag::PHI>(assign->src());
            for (auto &arg : phi->args)
            {
                auto *var        = as<IRVar, IR::Tag::VAR>(arg);
                auto *arg_assign = as<IRAssign, IR::Tag::ASSIGN>(var->def->belong_inst);
                if (arg_assign == nullptr) continue;
                auto *constant = as<IRConstant, IR::Tag::CONST>(arg_assign->src());
                if (constant == nullptr) continue;
                // this is constant! propagate it!
                var->def->killUse(var);
                delete var;
                // make a copy
                auto *new_const        = new IRConstant;
                new_const->value       = constant->value;
                new_const->belong_inst = arg_assign;
                //
                arg = new_const;
            }
        }
    }
}

void COMPILER::CFG::constantFolding(COMPILER::IRFunction *func)
{
    for (auto block : func->blocks)
    {
        for (auto *inst : block->insts)
        {
            auto *assign = as<IRAssign, IR::Tag::ASSIGN>(inst);
            if (assign == nullptr) continue;
            if (assign->src()->tag == IR::Tag::CONST) continue;
            //
            auto constant = tryFindConstant(assign->dest());
            if (constant.has_value())
            {
                auto *result  = new IRConstant;
                result->value = constant.value();
                // free memory
                auto *var = as<IRVar, IR::Tag ::VAR>(assign->src());
                if (var != nullptr)
                {
                    var->def->killUse(var);
                    delete var;
                }
                //
                auto *binary = as<IRBinary, IR::Tag ::BINARY>(assign->src());
                if (binary != nullptr)
                {
                    auto *lhs_const = as<IRConstant, IR::Tag::CONST>(binary->lhs);
                    auto *lhs_var   = as<IRVar, IR::Tag::VAR>(binary->lhs);
                    //
                    auto *rhs_const = as<IRConstant, IR::Tag::CONST>(binary->rhs);
                    auto *rhs_var   = as<IRVar, IR::Tag::VAR>(binary->rhs);
                    // update ud-chains
                    delete lhs_const;
                    delete rhs_const;
                    if (lhs_var != nullptr)
                    {
                        lhs_var->def->killUse(lhs_var);
                        delete lhs_var;
                    }
                    if (rhs_var != nullptr)
                    {
                        rhs_var->def->killUse(rhs_var);
                        delete rhs_var;
                    }
                }
                // set new constant
                assign->setSrc(result);
            }
        }
    }
}

std::optional<CYX::Value> COMPILER::CFG::tryFindConstant(COMPILER::IRVar *var)
{
    // recursive exit
    if (var == nullptr) return {};
    auto *assign = as<IRAssign, IR::Tag::ASSIGN>(var->belong_inst);
    if (assign == nullptr) return {};
    // c = 1
    // a = c
    auto *src_var = as<IRVar, IR::Tag::VAR>(assign->src());
    if (src_var != nullptr)
    {
        return tryFindConstant(src_var->def);
    }
    // a = 1
    auto *src_constant = as<IRConstant, IR::Tag::CONST>(assign->src());
    if (src_constant)
    {
        return src_constant->value;
    }
    // c = 1
    // b = 1
    // a = b + c
    auto *src_binary = as<IRBinary, IR::Tag::BINARY>(assign->src());
    if (src_binary && inOr(src_binary->opcode, IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_BOR, IR_BXOR, IR_MOD, IR_SHR, IR_SHL))
    {
        auto *lhs_const = as<IRConstant, IR::Tag::CONST>(src_binary->lhs);
        auto *lhs_var   = as<IRVar, IR::Tag::VAR>(src_binary->lhs);
        //
        auto *rhs_const = as<IRConstant, IR::Tag::CONST>(src_binary->rhs);
        auto *rhs_var   = as<IRVar, IR::Tag::VAR>(src_binary->rhs);
        //
        // recursive to find definitions. return null if found it not a constant.
        //
        CYX::Value lhs, rhs;
        if (lhs_const != nullptr)
            lhs = lhs_const->value;
        else if (lhs_var != nullptr)
        {
            auto tmp = tryFindConstant(lhs_var->def);
            if (tmp.has_value())
                lhs = tmp.value();
            else
                return {};
        }
        //
        if (rhs_const != nullptr)
            rhs = rhs_const->value;
        else if (rhs_var != nullptr)
        {
            auto tmp = tryFindConstant(rhs_var->def);
            if (tmp.has_value())
                rhs = tmp.value();
            else
                return {};
        }
        switch (src_binary->opcode)
        {
            case IR_ADD: return lhs + rhs;
            case IR_SUB: return lhs - rhs;
            case IR_MUL: return lhs * rhs;
            case IR_DIV: return lhs / rhs;
            case IR_BOR: return lhs | rhs;
            case IR_BXOR: return lhs ^ rhs;
            case IR_MOD: return lhs % rhs;
            case IR_SHR: return lhs >> rhs;
            case IR_SHL: return lhs << rhs;
            default: UNREACHABLE();
        }
    }
    return {};
}

void COMPILER::CFG::destroyPhiNode(COMPILER::IRAssign *assign)
{
    delete assign->dest();
    auto *phi = as<IRPhi, IR::Tag::PHI>(assign->src());
    for (auto *tmp : phi->args)
    {
        auto *var = as<IRVar, IR::Tag::VAR>(tmp);
        if (var->def) var->def->killUse(var);
        delete var;
    }
    delete phi;
    delete assign;
}

void COMPILER::CFG::phiElimination(COMPILER::IRFunction *func)
{
    for (auto *block : func->blocks)
    {
        for (auto *assign : block->phis)
        {
            auto args = as<IRPhi, IR::Tag::PHI>(assign->src())->args;

            const auto dest_name    = assign->dest()->name;
            const auto dest_ssa_idx = assign->dest()->ssa_index;
            auto *dest              = assign->dest();
            //
            for (int i = 0; i < args.size(); i++)
            {
                auto *new_assign = new IRAssign;
                if (i != 0)
                {
                    auto *new_var      = new IRVar;
                    new_var->name      = dest_name;
                    new_var->ssa_index = dest_ssa_idx;
                    new_assign->setDest(new_var);
                    new_var->def = dest;
                    dest->addUse(new_var);
                }
                else
                {
                    new_assign->setDest(dest);
                }
                //

                BasicBlock *insert_block;
                if (auto *tmp = as<IRVar, IR::Tag::VAR>(args[i]); tmp != nullptr)
                {
                    insert_block = tmp->def->belong_inst->block;
                }
                else if (auto *tmp = as<IRConstant, IR::Tag::CONST>(args[i]))
                {
                    insert_block = tmp->belong_inst->block;
                }
                else
                {
                    UNREACHABLE();
                }
                new_assign->setSrc(args[i]);
                new_assign->block = insert_block;
                // avoid insert as following code
                // jmp L1
                // x1 = x2
                if (insert_block->insts.back()->tag == IR::Tag::JMP)
                {
                    insert_block->addInstBefore(new_assign, insert_block->insts.back());
                }
                else
                {
                    insert_block->addInst(new_assign);
                }
            }
            delete assign;
        }
        // remove all phi functions.
        block->phis.clear();
    }
}

void COMPILER::CFG::deadCodeElimination(COMPILER::IRFunction *func)
{
    // MAGIC
    for (auto block_it = func->blocks.crbegin(); block_it != func->blocks.crend(); block_it++)
    {
        auto *block = *block_it;
        for (auto inst_it = block->insts.rbegin(); inst_it != block->insts.rend();)
        {
            auto *tmp    = *inst_it;
            auto *assign = as<IRAssign, IR::Tag::ASSIGN>(tmp);
            // a = b + c
            // no user! remove this instruction!
            if (assign != nullptr && assign->dest()->use.empty())
            {
                if (assign->dest()->def != nullptr && assign->dest()->def->ssaName() == assign->dest()->ssaName())
                {
                    inst_it++;
                    continue;
                }
                // call
                auto *call = as<IRCall, IR::Tag::CALL>(assign->src());
                if (call != nullptr)
                {
                    // non-retval. remove temp var.
                    delete tmp;
                    *inst_it = call;
                    inst_it++;
                    continue;
                }
                auto *var = as<IRVar, IR::Tag::VAR>(assign->src());
                if (var != nullptr)
                {
                    var->def->killUse(var);
                    delete var;
                }
                //
                auto *binary = as<IRBinary, IR::Tag::BINARY>(assign->src());
                if (binary != nullptr)
                {
                    auto *lhs_var   = as<IRVar, IR::Tag::VAR>(binary->lhs);
                    auto *lhs_const = as<IRConstant, IR::Tag::CONST>(binary->lhs);
                    auto *rhs_var   = as<IRVar, IR::Tag::VAR>(binary->rhs);
                    auto *rhs_const = as<IRConstant, IR::Tag::CONST>(binary->rhs);
                    if (lhs_var != nullptr) lhs_var->def->killUse(lhs_var);
                    if (rhs_var != nullptr) rhs_var->def->killUse(rhs_var);
                    //
                    delete lhs_var;
                    delete rhs_var;
                    delete lhs_const;
                    delete rhs_const;
                    delete binary;
                }
                delete assign;
                // remove it from list
                block->insts.erase(--inst_it.base());
            }
            else
            {
                inst_it++;
            }
        }
    }
}

void COMPILER::CFG::tryRename(COMPILER::IRFunction *func)
{
    for (const auto &p : var_block_map)
    {
        counter[p.first] = 0;
    }
    rename(func->blocks.front());
}

void COMPILER::CFG::rename(COMPILER::BasicBlock *block)
{
    for (auto *phi : block->phis)
    {
        auto *lhs                   = phi->dest();
        lhs->ssa_index              = newId(lhs->name, lhs);
        ssa_def_map[lhs->ssaName()] = lhs;
    }
    for (auto *inst : block->insts)
    {
        renameIrArgs(inst);
    }
    for (auto *succ : block->succs)
    {
        for (auto *phi : succ->phis)
        {
            // add phi args
            auto *dest       = phi->dest();
            auto *src        = as<IRPhi, IR::Tag::PHI>(phi->src()); // real phi function
            auto [id, def]   = getId(dest->name);
            auto *arg        = new IRVar;
            arg->ssa_index   = id;
            arg->name        = dest->name;
            arg->belong_inst = phi;
            // def-use chain
            arg->def = dynamic_cast<IRVar *>(def);
            if (def != nullptr) def->addUse(arg);
            src->args.push_back(arg);
        }
    }
    for (auto *succ : tree[block])
    {
        rename(succ);
    }
    for (auto *inst : block->insts)
    {
        if (inst->tag == IR::Tag::ASSIGN)
        {
            auto *assign = as<IRAssign, IR::Tag::ASSIGN>(inst);
            if (!stack[assign->dest()->name].empty()) stack[assign->dest()->name].pop();
        }
    }
    for (auto *phi : block->phis)
    {
        if (!stack[phi->dest()->name].empty()) stack[phi->dest()->name].pop();
    }
}

void COMPILER::CFG::renameIrArgs(COMPILER::IR *inst)
{
    auto *func_call = as<IRCall, IR::Tag::CALL>(inst);
    if (func_call != nullptr)
    {
        renameFuncCall(func_call);
        return;
    }
    //
    auto *assign = as<IRAssign, IR::Tag::ASSIGN>(inst);
    auto *var    = as<IRVar, IR::Tag::VAR>(inst);
    if (var != nullptr)
    {
        renameVar(var);
        return;
    }
    if (assign == nullptr) return;
    // try type cast
    auto *binary = as<IRBinary, IR::Tag::BINARY>(assign->src());
    var          = as<IRVar, IR::Tag::VAR>(assign->src());
    func_call    = as<IRCall, IR::Tag ::CALL>(assign->src());
    // rename src!
    if (binary != nullptr)
    {
        auto *src_lhs = as<IRVar, IR::Tag::VAR>(binary->lhs);
        auto *src_rhs = as<IRVar, IR::Tag::VAR>(binary->rhs);
        renameVar(src_lhs);
        renameVar(src_rhs);
    }
    if (var != nullptr)
    {
        renameVar(var);
    }
    if (func_call != nullptr)
    {
        renameFuncCall(func_call);
    }
    // rename dest
    if (assign->dest()->def == nullptr && assign->dest()->is_ir_gen) return;
    assign->dest()->ssa_index = newId(assign->dest()->name, assign->dest());
    auto dest_name            = assign->dest()->ssaName();
    ssa_def_map[dest_name]    = assign->dest();
    assign->dest()->use.clear();
    assign->dest()->def = nullptr;
}

void COMPILER::CFG::renameFuncCall(COMPILER::IRCall *inst)
{
    for (auto *arg : inst->args)
    {
        renameIrArgs(arg);
    }
}

void COMPILER::CFG::renameVar(COMPILER::IRVar *var)
{
    if (var == nullptr || var->is_ir_gen) return;
    auto [id, def] = getId(var->name);
    var->ssa_index = id;
    // def-use chains update
    auto def_name = var->ssaName();
    if (ssa_def_map.find(def_name) != ssa_def_map.end())
    {
        var->def = ssa_def_map[def_name];
        var->def->addUse(var);
    }
    else
        UNREACHABLE();
}

int COMPILER::CFG::newId(const std::string &name, IRVar *def)
{
    int i = counter[name]++;
    stack[name].push({ i, def });
    return i;
}

std::pair<int, COMPILER::IRVar *> COMPILER::CFG::getId(const std::string &name)
{
    if (stack[name].empty()) return { 0, nullptr };
    return stack[name].top();
}

/////////////////////////dump as str//////////////////////

std::string COMPILER::CFG::iDomDetailStr() const
{
    std::string str;
    for (const auto &x : idom)
    {
        str += (x.first != nullptr ? x.first->name : "null") + " dominated by " +
               (x.second != nullptr ? x.second->name : "null") + "\n";
    }
    return str;
}

std::string COMPILER::CFG::dominanceFrontierStr() const
{
    std::string str;
    for (const auto &p : dominance_frontier)
    {
        str += p.first->name + " : {";
        for (auto *block : p.second)
        {
            str += block->name + " ";
        }
        str += "}\n";
    }
    return str;
}

std::string COMPILER::CFG::cfgStr() const
{
    std::string str = "digraph G{\n";
    for (auto *func : funcs)
    {
        for (auto *block : func->blocks)
        {
            addSpace(str, 4);
            str += block->name + " [shape=record, label=\"{" + block->name;
            for (auto *phi : block->phis)
            {
                str += "|" + phi->toString();
            }
            for (auto *inst : block->insts)
            {
                str += "|" + inst->toString();
            }
            str += "}\"]\n";

            for (auto *succ : block->succs)
            {
                addSpace(str, 4);
                str += block->name + " -> " + succ->name + "\n";
            }
        }
    }
    return str + "\n}";
}
