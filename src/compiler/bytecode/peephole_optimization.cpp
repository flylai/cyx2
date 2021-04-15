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
    std::deque<std::list<CVM::VMInstruction *>::iterator> window;
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
                if (window.size() >= 2) window.pop_front();
                window.push_back(it);
                pass(window, it, block->vm_insts);
            }
        }
    }
}

void COMPILER::PeepholeOptimization::pass(std::deque<std::list<CVM::VMInstruction *>::iterator> window,
                                          std::list<CVM::VMInstruction *>::iterator &cur_it,
                                          std::list<CVM::VMInstruction *> &vm_insts)
{
    // if removed some code, `cur_it` will do nothing at the end of this function
    // else `cur_it++`
    auto remove_code = false;
    auto checkTarget = [this](const std::string &target_name)
    {
        // check whether the front instruction of the target block is a jmp instruction
        auto tmp = (*block_list)[jump_map[target_name]]->vm_insts.front();
        if (tmp->opcode == CVM::Opcode::JMP) return static_cast<CVM::Jmp *>(tmp)->basic_block_name;
        return target_name;
    };
    auto loadStorePass = [&window, &cur_it, &vm_insts]()
    {
        // storex a %1
        // loadx %1 a
        // or
        // storex a %1
        // storex a %1
        if (window.size() <= 1) return false;
        auto storex  = dynamic_cast<CVM::StoreX *>(*window[0]);
        auto storex2 = dynamic_cast<CVM::StoreX *>(*window[1]);
        auto loadx   = dynamic_cast<CVM::LoadX *>(*window[1]);
        if (storex == nullptr || (loadx == nullptr && storex2 == nullptr)) return false;
        if ((loadx != nullptr && storex->name == loadx->name && storex->reg_idx == loadx->reg_idx &&
             storex->index.empty() && loadx->index.empty()) ||
            (storex2 != nullptr && storex->name == storex2->name && storex->reg_idx == storex2->reg_idx &&
             storex->index.empty() && storex2->index.empty()))
        {
            delete *window[1];
            *window[1] = nullptr;
            cur_it     = vm_insts.erase(window[1]);
            window.pop_back();
            return true;
        }
        return false;
    };
    auto jmpJmpPass = [&window, &cur_it, &vm_insts]()
    {
        // jmp 666
        // jmp 777 (no code jump to this line) or jmp 666
        if (window.size() <= 1) return false;
        auto jmp  = dynamic_cast<CVM::Jmp *>(*window[0]);
        auto jmp2 = dynamic_cast<CVM::Jmp *>(*window[1]);
        if (jmp == nullptr || jmp2 == nullptr) return false;
        if (jmp->target == jmp2->target)
        {
            delete *window[1];
            *window[1] = nullptr;
            cur_it     = vm_insts.erase(window[1]);
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
            verifyTarget(*x);
        }
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
            verifyTarget(*x);
        }
    };
    remove_code |= loadStorePass();
    remove_code |= jmpJmpPass();
    jmpToJmpPass();
    jifPass();
    changed |= remove_code;
    if (!remove_code) cur_it++;
}
