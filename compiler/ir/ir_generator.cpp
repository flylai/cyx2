#include "ir_generator.h"
void COMPILER::IRGenerator::visitUnaryExpr(COMPILER::UnaryExpr *ptr)
{
}

void COMPILER::IRGenerator::visitBinaryExpr(COMPILER::BinaryExpr *ptr)
{
}

void COMPILER::IRGenerator::visitIntExpr(COMPILER::IntExpr *ptr)
{
}

void COMPILER::IRGenerator::visitDoubleExpr(COMPILER::DoubleExpr *ptr)
{
}

void COMPILER::IRGenerator::visitStringExpr(COMPILER::StringExpr *ptr)
{
}

void COMPILER::IRGenerator::visitAssignExpr(COMPILER::AssignExpr *ptr)
{
}

void COMPILER::IRGenerator::visitIdentifierExpr(COMPILER::IdentifierExpr *ptr)
{
}

void COMPILER::IRGenerator::visitExprStmt(COMPILER::ExprStmt *ptr)
{
}

void COMPILER::IRGenerator::visitIfStmt(COMPILER::IfStmt *ptr)
{
}

void COMPILER::IRGenerator::visitForStmt(COMPILER::ForStmt *ptr)
{
}

void COMPILER::IRGenerator::visitWhileStmt(COMPILER::WhileStmt *ptr)
{
}

void COMPILER::IRGenerator::visitSwitchStmt(COMPILER::SwitchStmt *ptr)
{
}

void COMPILER::IRGenerator::visitMatchStmt(COMPILER::MatchStmt *ptr)
{
}

void COMPILER::IRGenerator::visitFuncDeclStmt(COMPILER::FuncDeclStmt *ptr)
{
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
    return "t" + std::to_string(var_cnt++);
}

std::string COMPILER::IRGenerator::newLabel()
{
    return "@l" + std::to_string(label_cnt++);
}

void COMPILER::IRGenerator::genExpr(COMPILER::Expr *ptr, const std::string &var_name)
{
    if (typeid(*ptr) == typeid(AssignExpr))
    {
        auto *tmp    = dynamic_cast<AssignExpr *>(ptr);
        auto *id_ptr = dynamic_cast<IdentifierExpr *>(tmp->lhs);
        current_symbol->upsert(id_ptr->value, Symbol(id_ptr->value, ""));

        std::string var1 = newVariable();
        genExpr(tmp->rhs, var1);

        //        IRInstruction instruction;
        //        instruction.dest   = var_name;
        //        instruction.opcode = IROpcode::IR_ASSIGN;
        //        instruction.operand1_type = IROperandType::VARIABLE;
        //        instruction.operand1      = var1;
        //        instructions.push_back(instruction);

        IRInstruction instruction2;
        instruction2.dest          = id_ptr->value;
        instruction2.opcode        = IROpcode::IR_ASSIGN;
        instruction2.operand1_type = IROperandType::VARIABLE;
        instruction2.operand1      = var1;
        instructions.push_back(instruction2);
    }
    else if (typeid(*ptr) == typeid(BinaryExpr))
    {
        auto *tmp        = dynamic_cast<BinaryExpr *>(ptr);
        std::string var1 = newVariable();
        std::string var2 = newVariable();

        genExpr(tmp->lhs, var1);
        genExpr(tmp->rhs, var2);

        IRInstruction instruction;
        instruction.dest          = var_name;
        instruction.operand1      = var1;
        instruction.operand1_type = IROperandType::VARIABLE;
        instruction.operand2      = var2;
        instruction.operand2_type = IROperandType::VARIABLE;
        instruction.opcode        = COMPILER::IRInstruction::token2IROp(tmp->op.keyword);

        instructions.push_back(instruction);
    }
    else if (typeid(*ptr) == typeid(IdentifierExpr))
    {
        auto *tmp            = dynamic_cast<IdentifierExpr *>(ptr);
        auto [depth, symbol] = current_symbol->query(tmp->value);

        IRInstruction instruction;
        instruction.dest          = var_name;
        instruction.operand1_type = IROperandType::STRING;
        instruction.operand1      = symbol.value().value<std::string>();
        instruction.opcode        = IROpcode::IR_ASSIGN;
        instructions.push_back(instruction);
    }
    else if (typeid(*ptr) == typeid(IntExpr))
    {
        IRInstruction instruction;
        instruction.dest          = var_name;
        instruction.operand1_type = IROperandType::INTEGER;
        instruction.operand1      = dynamic_cast<IntExpr *>(ptr)->value;
        instruction.opcode        = IROpcode::IR_ASSIGN;
        instructions.push_back(instruction);
    }
}

COMPILER::IRGenerator::IRGenerator()
{
    current_symbol = new SymbolTable();
}

void COMPILER::IRGenerator::genStmt(COMPILER::Stmt *ptr)
{
    if (typeid(*ptr) == typeid(FuncDeclStmt))
    {
        auto *tmp = dynamic_cast<FuncDeclStmt *>(ptr);
        current_symbol->upsert(tmp->func_name->value, Symbol(tmp->func_name->value, "__FUNC__"));

        std::vector<std::string> params;
        for (auto &x : tmp->params)
        {
            params.push_back(x);
            current_symbol->upsert(x, Symbol(x, "__PARAM__"));
        }

        IRInstruction instruction;
        instruction.operand1_type = IROperandType::LABEL;
        instruction.operand1      = tmp->func_name->value;
        instructions.push_back(instruction);

        IRInstruction instruction2;
        instruction2.operand1_type = IROperandType::PARAMS;
        instruction2.operand1      = params;
        instructions.push_back(instruction2);

        genStmt(tmp->block);
    }
    else if (typeid(*ptr) == typeid(BlockStmt))
    {
        auto *tmp = dynamic_cast<BlockStmt *>(ptr);
        for (auto x : tmp->stmts)
        {
            if (auto *stmt = dynamic_cast<Stmt *>(x))
            {
                genStmt(stmt);
            }
        }
    }
    else if (typeid(*ptr) == typeid(WhileStmt))
    {
        auto *tmp          = dynamic_cast<WhileStmt *>(ptr);
        std::string label1 = newLabel();
        std::string label2 = newLabel();
        std::string label3 = newLabel();

        IRInstruction instruction1;
        instruction1.opcode        = IROpcode::IR_LABEL;
        instruction1.operand1_type = IROperandType::STRING;
        instruction1.operand1      = label1;
        instructions.push_back(instruction1);
        genCond(tmp->cond, label2, label3);

        IRInstruction instruction2;
        instruction2.opcode        = IROpcode::IR_LABEL;
        instruction2.operand1_type = IROperandType::STRING;
        instruction2.operand1      = label2;
        instructions.push_back(instruction2);
        genStmt(tmp->block);

        IRInstruction instruction3;
        instruction3.opcode        = IROpcode::IR_GOTO;
        instruction3.operand1_type = IROperandType::STRING;
        instruction3.operand1      = label1;
        instructions.push_back(instruction3);

        IRInstruction instruction4;
        instruction4.opcode        = IROpcode::IR_LABEL;
        instruction4.operand1_type = IROperandType::STRING;
        instruction4.operand1      = label3;
        instructions.push_back(instruction4);
    }
    else if (typeid(*ptr) == typeid(ExprStmt))
    {
        return genExpr(dynamic_cast<ExprStmt *>(ptr)->expr, "");
    }
}

void COMPILER::IRGenerator::genCond(COMPILER::Expr *ptr, const std::string &true_cond, const std::string &false_cond)
{
    if (typeid(*ptr) == typeid(BinaryExpr))
    {
        auto *tmp = dynamic_cast<BinaryExpr *>(ptr);
        if (tmp->op.keyword == Keyword::LAND)
        {
            std::string label1 = newLabel();
            genCond(ptr, label1, false_cond);
            genCond(ptr, true_cond, false_cond);
        }
        else if (tmp->op.keyword == Keyword::LOR)
        {
            std::string label1 = newLabel();
            genCond(ptr, true_cond, label1);
            genCond(ptr, true_cond, false_cond);
        }
        else if (tmp->op.keyword == Keyword::EQ || tmp->op.keyword == Keyword::NE) // TODO some RELOPs need to add
        {
            std::string var1 = newVariable();
            std::string var2 = newVariable();

            genExpr(tmp->lhs, var1);
            genExpr(tmp->rhs, var2);

            IRInstruction instruction1;
            instruction1.opcode        = IROpcode::IR_IF;
            instruction1.operand1_type = IROperandType::STRING;
            instruction1.operand1      = var1 + tmp->op.value + var2;
            instructions.push_back(instruction1);

            IRInstruction instruction2;
            instruction2.opcode        = IROpcode::IR_GOTO;
            instruction2.operand1_type = IROperandType::LABEL;
            instruction2.operand1      = true_cond;
            instructions.push_back(instruction2);

            IRInstruction instruction3;
            instruction3.opcode        = IROpcode::IR_GOTO;
            instruction3.operand1_type = IROperandType::LABEL;
            instruction3.operand1      = false_cond;
            instructions.push_back(instruction3);
        }
    }
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
}
