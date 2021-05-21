#include "peephole_optimization.h"

void COMPILER::PeepholeOptimization::doPeepholeOptimization()
{
    collectJumpTarget();
    traversal();
}

void COMPILER::PeepholeOptimization::collectJumpTarget()
{
    for (int i = 0; i < block_list->size(); i++)
    {
        jump_map[(*block_list)[i]->name] = i;
    }
}

void COMPILER::PeepholeOptimization::traversal()
{
    // remove some instructions like following code
    // store? c %1
    // load? %1 c  (remove this line)
    // store %1 c  (remove this line too)
    std::deque<std::pair<std::list<CVM::VMInstruction *> *, std::list<CVM::VMInstruction *>::iterator>> window;
    while (changed)
    {
        changed = false;
        for (auto &block : *block_list)
        {
            for (auto it = block->vm_insts.begin(); it != block->vm_insts.end();)
            {
                auto inst = *it;
                if (inst == nullptr ||
                    !inOr(inst->opcode, CVM::Opcode::STOREA, CVM::Opcode::STORED, CVM::Opcode::STOREI,
                          CVM::Opcode::STORES, CVM::Opcode::STOREX, //
                          CVM::Opcode::LOADA, CVM::Opcode::LOADD, CVM::Opcode::LOADI, CVM::Opcode::LOADS,
                          CVM::Opcode::LOADX, //
                          CVM::Opcode::JMP, CVM::Opcode::JIF))
                {
                    if (inst == nullptr) window.clear();
                    it++;
                    continue;
                }
                window.emplace_back(&block->vm_insts, it);
                pass(window, it);
            }
        }
    }
}

void COMPILER::PeepholeOptimization::pass(
    std::deque<std::pair<std::list<CVM::VMInstruction *> *, std::list<CVM::VMInstruction *>::iterator>> &window,
    std::list<CVM::VMInstruction *>::iterator &cur_it)
{
    // if removed some code, `cur_it` will do nothing at the end of this function
    // else `cur_it++`
    auto remove_code = false;
    const auto len   = window.size();
    auto checkTarget = [this](const std::string &target_name)
    {
        // check whether the front instruction of the target block is a jmp instruction
        auto tmp = (*block_list)[jump_map[target_name]]->vm_insts.front();
        if (tmp->opcode == CVM::Opcode::JMP) return static_cast<CVM::Jmp *>(tmp)->basic_block_name;
        return target_name;
    };
    auto loadStorePass = [&window, &cur_it, &len]()
    {
        // storex a %1
        // loadx %1 a
        // or
        // storex a %1
        // storex a %1
        if (window.size() <= 1) return false;

        auto storex  = dynamic_cast<CVM::StoreX *>(*window[len - 2].second);
        auto storex2 = dynamic_cast<CVM::StoreX *>(*window[len - 1].second);
        auto loadx   = dynamic_cast<CVM::LoadX *>(*window[len - 1].second);
        if (storex == nullptr || (loadx == nullptr && storex2 == nullptr)) return false;
        if ((loadx != nullptr && storex->name == loadx->name && storex->reg_idx == loadx->reg_idx &&
             storex->index.empty() && loadx->index.empty()) ||
            (storex2 != nullptr && storex->name == storex2->name && storex->reg_idx == storex2->reg_idx &&
             storex->index.empty() && storex2->index.empty()))
        {
            delete *window[len - 1].second;
            *window[len - 1].second = nullptr;
            if (cur_it == window[len - 1].second)
            {
                cur_it = window[len - 1].first->erase(cur_it);
            }
            window.pop_back();
            return true;
        }
        return false;
    };
    auto jmpJmpPass = [&window, &cur_it, &len]()
    {
        // jmp 666
        // jmp 777 (no code jump to this line) or jmp 666
        if (window.size() <= 1) return false;
        auto jmp  = dynamic_cast<CVM::Jmp *>(*window[len - 2].second);
        auto jmp2 = dynamic_cast<CVM::Jmp *>(*window[len - 1].second);
        if (jmp == nullptr || jmp2 == nullptr) return false;
        if (jmp->target == jmp2->target)
        {
            delete *window[len - 1].second;
            *window[len - 1].second = nullptr;
            if (cur_it == window[len - 1].second)
            {
                cur_it = window[len - 1].first->erase(cur_it);
            }
            window.pop_back();
            return true;
        }
        return false;
    };
    auto jmpToJmpPass = [&window, checkTarget]()
    {
        // 1 jmp 6
        // 6 jmp 666
        // 1 jmp `6` will replace with `666`
        if (window.empty()) return;
        auto verifyTarget = [checkTarget](CVM::VMInstruction *inst)
        {
            if (auto jmp = dynamic_cast<CVM::Jmp *>(inst); jmp != nullptr)
            {
                const auto new_target = checkTarget(jmp->basic_block_name);
                jmp->basic_block_name = new_target;
            }
        };
        for (auto x : window)
        {
            verifyTarget(*x.second);
        }
    };
    auto jifSamePass = [&window, &cur_it]()
    {
        // jif 666 666
        // replace it with jmp 666
        if (window.empty()) return false;
        bool retval = false;
        for (auto w_it = window.begin(); w_it != window.end();)
        {
            auto &[vm_inst, it] = *w_it;
            auto *jif           = dynamic_cast<CVM::Jif *>(*it);
            if (jif == nullptr || jif->basic_block_name1 != jif->basic_block_name2)
                return false;
            else
                w_it++;
            // otherwise replace it.
            retval |= true;
            auto *jmp             = new CVM::Jmp;
            jmp->basic_block_name = jif->basic_block_name1;
            if (it == cur_it)
            {
                cur_it = vm_inst->erase(cur_it);
                cur_it = vm_inst->insert(cur_it, jmp);
            }
            else
            {
                it = vm_inst->erase(it);
                it = vm_inst->insert(it, jmp);
            }
            w_it = window.erase(w_it);
        }
        return retval;
    };
    auto jifPass = [&window, checkTarget]()
    {
        // 111 jif 666 777
        // 666 jmp ....
        // 777 jmp ....
        if (window.empty()) return;
        auto verifyTarget = [checkTarget](CVM::VMInstruction *inst)
        {
            if (auto jif = dynamic_cast<CVM::Jif *>(inst); jif != nullptr)
            {
                const auto new_target1 = checkTarget(jif->basic_block_name1);
                const auto new_target2 = checkTarget(jif->basic_block_name2);
                jif->basic_block_name1 = new_target1;
                jif->basic_block_name2 = new_target2;
            }
        };
        for (auto x : window)
        {
            verifyTarget(*x.second);
        }
    };

    remove_code |= loadStorePass();
    remove_code |= jmpJmpPass();
    remove_code |= jifSamePass();
    jmpToJmpPass();
    jifPass();
    changed |= remove_code;
    if (!remove_code) cur_it++;
}
