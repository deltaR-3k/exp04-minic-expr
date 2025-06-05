﻿///
/// @file IRGenerator.cpp
/// @brief AST遍历产生线性IR的源文件
/// @author zenglj (zenglj@live.com)
/// @version 1.1
/// @date 2024-11-23
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-09-29 <td>1.0     <td>zenglj  <td>新建
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>表达式版增强
/// </table>
///
#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>

#include "AST.h"
#include "Common.h"
#include "Function.h"
#include "IRCode.h"
#include "IRGenerator.h"
#include "Module.h"
#include "EntryInstruction.h"
#include "LabelInstruction.h"
#include "ExitInstruction.h"
#include "FuncCallInstruction.h"
#include "BinaryInstruction.h"
#include "MoveInstruction.h"
#include "GotoInstruction.h"
#include "CondBrInstruction.h"

/// @brief 构造函数
/// @param _root AST的根
/// @param _module 符号表
IRGenerator::IRGenerator(ast_node * _root, Module * _module) : root(_root), module(_module)
{
    /* 叶子节点 */
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_LITERAL_UINT] = &IRGenerator::ir_leaf_node_uint;
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_VAR_ID] = &IRGenerator::ir_leaf_node_var_id;
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_TYPE] = &IRGenerator::ir_leaf_node_type;

    /* 表达式运算， 加减 */
    ast2ir_handlers[ast_operator_type::AST_OP_SUB] = &IRGenerator::ir_sub;
    ast2ir_handlers[ast_operator_type::AST_OP_ADD] = &IRGenerator::ir_add;
    ast2ir_handlers[ast_operator_type::AST_OP_MUL] = &IRGenerator::ir_mul;
    ast2ir_handlers[ast_operator_type::AST_OP_DIV] = &IRGenerator::ir_div;
    ast2ir_handlers[ast_operator_type::AST_OP_MOD] = &IRGenerator::ir_mod;
    ast2ir_handlers[ast_operator_type::AST_OP_NEG] = &IRGenerator::ir_neg;
    
    /* 关系运算符 */
    ast2ir_handlers[ast_operator_type::AST_OP_EQ] = &IRGenerator::ir_eq;
    ast2ir_handlers[ast_operator_type::AST_OP_NEQ] = &IRGenerator::ir_neq;
    ast2ir_handlers[ast_operator_type::AST_OP_LT] = &IRGenerator::ir_lt;
    ast2ir_handlers[ast_operator_type::AST_OP_LE] = &IRGenerator::ir_le;
    ast2ir_handlers[ast_operator_type::AST_OP_GT] = &IRGenerator::ir_gt;
    ast2ir_handlers[ast_operator_type::AST_OP_GE] = &IRGenerator::ir_ge;
    
    /* 逻辑运算符 */
    ast2ir_handlers[ast_operator_type::AST_OP_AND] = &IRGenerator::ir_and;
    ast2ir_handlers[ast_operator_type::AST_OP_OR] = &IRGenerator::ir_or;
    ast2ir_handlers[ast_operator_type::AST_OP_NOT] = &IRGenerator::ir_not;

    /* 语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_ASSIGN] = &IRGenerator::ir_assign;
    ast2ir_handlers[ast_operator_type::AST_OP_RETURN] = &IRGenerator::ir_return;
    ast2ir_handlers[ast_operator_type::AST_OP_IF] = &IRGenerator::ir_if;
    ast2ir_handlers[ast_operator_type::AST_OP_IF_ELSE] = &IRGenerator::ir_if_else;
    ast2ir_handlers[ast_operator_type::AST_OP_WHILE] = &IRGenerator::ir_while;
    ast2ir_handlers[ast_operator_type::AST_OP_BREAK] = &IRGenerator::ir_break;
    ast2ir_handlers[ast_operator_type::AST_OP_CONTINUE] = &IRGenerator::ir_continue;

    /* 函数调用 */
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_CALL] = &IRGenerator::ir_function_call;

    /* 函数定义 */
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_DEF] = &IRGenerator::ir_function_define;
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS] = &IRGenerator::ir_function_formal_params;

    /* 变量定义语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_DECL_STMT] = &IRGenerator::ir_declare_statment;
    ast2ir_handlers[ast_operator_type::AST_OP_VAR_DECL] = &IRGenerator::ir_variable_declare;

    /* 语句块 */
    ast2ir_handlers[ast_operator_type::AST_OP_BLOCK] = &IRGenerator::ir_block;

    /* 编译单元 */
    ast2ir_handlers[ast_operator_type::AST_OP_COMPILE_UNIT] = &IRGenerator::ir_compile_unit;
}

/// @brief 创建新的标签指令
/// @return 新创建的标签指令
LabelInstruction * IRGenerator::create_new_label() {
    Function* currentFunc = module->getCurrentFunction();
    
    // 生成唯一的标签名
    std::stringstream ss;
    ss << "label" << labelCounter++;
    
    // 创建新的标签指令
    LabelInstruction* label = new LabelInstruction(currentFunc);
    
    return label;
}

/// @brief 遍历抽象语法树产生线性IR，保存到IRCode中
/// @param root 抽象语法树
/// @param IRCode 线性IR
/// @return true: 成功 false: 失败
bool IRGenerator::run()
{
    ast_node * node;

    // 从根节点进行遍历
    node = ir_visit_ast_node(root);

    return node != nullptr;
}

/// @brief 根据AST的节点运算符查找对应的翻译函数并执行翻译动作
/// @param node AST节点
/// @return 成功返回node节点，否则返回nullptr
ast_node * IRGenerator::ir_visit_ast_node(ast_node * node)
{
    // 空节点
    if (nullptr == node) {
        return nullptr;
    }

    bool result;

    std::unordered_map<ast_operator_type, ast2ir_handler_t>::const_iterator pIter;
    pIter = ast2ir_handlers.find(node->node_type);
    if (pIter == ast2ir_handlers.end()) {
        // 没有找到，则说明当前不支持
        result = (this->ir_default)(node);
    } else {
        result = (this->*(pIter->second))(node);
    }

    if (!result) {
        // 语义解析错误，则出错返回
        node = nullptr;
    }

    return node;
}

/// @brief 未知节点类型的节点处理
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_default(ast_node * node)
{
    // 未知的节点
    printf("Unkown node(%d)\n", (int) node->node_type);
    return true;
}

/// @brief 编译单元AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_compile_unit(ast_node * node)
{
    module->setCurrentFunction(nullptr);

    for (auto son: node->sons) {

        // 遍历编译单元，要么是函数定义，要么是语句
        ast_node * son_node = ir_visit_ast_node(son);
        if (!son_node) {
            // TODO 自行追加语义错误处理
            return false;
        }
    }

    return true;
}

/// @brief 函数定义AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_define(ast_node * node)
{
    bool result;

    // 创建一个函数，用于当前函数处理
    if (module->getCurrentFunction()) {
        // 函数中嵌套定义函数，这是不允许的，错误退出
        // TODO 自行追加语义错误处理
        return false;
    }

    // 函数定义的AST包含四个孩子
    // 第一个孩子：函数返回类型
    // 第二个孩子：函数名字
    // 第三个孩子：形参列表
    // 第四个孩子：函数体即block
    ast_node * type_node = node->sons[0];
    ast_node * name_node = node->sons[1];
    ast_node * param_node = node->sons[2];
    ast_node * block_node = node->sons[3];

    // 创建一个新的函数定义
    Function * newFunc = module->newFunction(name_node->name, type_node->type);
    if (!newFunc) {
        // 新定义的函数已经存在，则失败返回。
        // TODO 自行追加语义错误处理
        return false;
    }

    // 当前函数设置有效，变更为当前的函数
    module->setCurrentFunction(newFunc);

    // 进入函数的作用域
    module->enterScope();

    // 获取函数的IR代码列表，用于后面追加指令用，注意这里用的是引用传值
    InterCode & irCode = newFunc->getInterCode();

    // 这里也可增加一个函数入口Label指令，便于后续基本块划分

    // 创建并加入Entry入口指令
    irCode.addInst(new EntryInstruction(newFunc));

    // 创建出口指令并不加入出口指令，等函数内的指令处理完毕后加入出口指令
    LabelInstruction * exitLabelInst = new LabelInstruction(newFunc);

    // 函数出口指令保存到函数信息中，因为在语义分析函数体时return语句需要跳转到函数尾部，需要这个label指令
    newFunc->setExitLabel(exitLabelInst);

    // 遍历形参，没有IR指令，不需要追加
    result = ir_function_formal_params(param_node);
    if (!result) {
        // 形参解析失败
        // TODO 自行追加语义错误处理
        return false;
    }
    node->blockInsts.addInst(param_node->blockInsts);

    // 新建一个Value，用于保存函数的返回值，如果没有返回值可不用申请
    LocalVariable * retValue = nullptr;
    if (!type_node->type->isVoidType()) {

        // 保存函数返回值变量到函数信息中，在return语句翻译时需要设置值到这个变量中
        retValue = static_cast<LocalVariable *>(module->newVarValue(type_node->type));
    }
    newFunc->setReturnValue(retValue);

    // 这里最好设置返回值变量的初值为0，以便在没有返回值时能够返回0

    // 函数内已经进入作用域，内部不再需要做变量的作用域管理
    block_node->needScope = false;

    // 遍历block
    result = ir_block(block_node);
    if (!result) {
        // block解析失败
        // TODO 自行追加语义错误处理
        return false;
    }

    // IR指令追加到当前的节点中
    node->blockInsts.addInst(block_node->blockInsts);

    // 此时，所有指令都加入到当前函数中，也就是node->blockInsts

    // node节点的指令移动到函数的IR指令列表中
    irCode.addInst(node->blockInsts);

    // 添加函数出口Label指令，主要用于return语句跳转到这里进行函数的退出
    irCode.addInst(exitLabelInst);

    // 函数出口指令
    irCode.addInst(new ExitInstruction(newFunc, retValue));

    // 恢复成外部函数
    module->setCurrentFunction(nullptr);

    // 退出函数的作用域
    module->leaveScope();

    return true;
}

/// @brief 形式参数AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_formal_params(ast_node * node)
{
    // TODO 目前形参还不支持，直接返回true

    // 每个形参变量都创建对应的临时变量，用于表达实参转递的值
    // 而真实的形参则创建函数内的局部变量。
    // 然后产生赋值指令，用于把表达实参值的临时变量拷贝到形参局部变量上。
    // 请注意这些指令要放在Entry指令后面，因此处理的先后上要注意。

    return true;
}

/// @brief 函数调用AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_call(ast_node * node)
{
    std::vector<Value *> realParams;

    // 获取当前正在处理的函数
    Function * currentFunc = module->getCurrentFunction();

    // 函数调用的节点包含两个节点：
    // 第一个节点：函数名节点
    // 第二个节点：实参列表节点

    std::string funcName = node->sons[0]->name;
    int64_t lineno = node->sons[0]->line_no;

    ast_node * paramsNode = node->sons[1];

    // 根据函数名查找函数，看是否存在。若不存在则出错
    // 这里约定函数必须先定义后使用
    auto calledFunction = module->findFunction(funcName);
    if (nullptr == calledFunction) {
        minic_log(LOG_ERROR, "函数(%s)未定义或声明", funcName.c_str());
        return false;
    }

    // 当前函数存在函数调用
    currentFunc->setExistFuncCall(true);

    // 如果没有孩子，也认为是没有参数
    if (!paramsNode->sons.empty()) {

        int32_t argsCount = (int32_t) paramsNode->sons.size();

        // 当前函数中调用函数实参个数最大值统计，实际上是统计实参传参需在栈中分配的大小
        // 因为目前的语言支持的int和float都是四字节的，只统计个数即可
        if (argsCount > currentFunc->getMaxFuncCallArgCnt()) {
            currentFunc->setMaxFuncCallArgCnt(argsCount);
        }

        // 遍历参数列表，孩子是表达式
        // 这里自左往右计算表达式
        for (auto son: paramsNode->sons) {

            // 遍历Block的每个语句，进行显示或者运算
            ast_node * temp = ir_visit_ast_node(son);
            if (!temp) {
                return false;
            }

            realParams.push_back(temp->val);
            node->blockInsts.addInst(temp->blockInsts);
        }
    }

    // TODO 这里请追加函数调用的语义错误检查，这里只进行了函数参数的个数检查等，其它请自行追加。
    if (realParams.size() != calledFunction->getParams().size()) {
        // 函数参数的个数不一致，语义错误
        minic_log(LOG_ERROR, "第%lld行的被调用函数(%s)未定义或声明", (long long) lineno, funcName.c_str());
        return false;
    }

    // 返回调用有返回值，则需要分配临时变量，用于保存函数调用的返回值
    Type * type = calledFunction->getReturnType();

    FuncCallInstruction * funcCallInst = new FuncCallInstruction(currentFunc, calledFunction, realParams, type);

    // 创建函数调用指令
    node->blockInsts.addInst(funcCallInst);

    // 函数调用结果Value保存到node中，可能为空，上层节点可利用这个值
    node->val = funcCallInst;

    return true;
}

/// @brief 语句块（含函数体）AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_block(ast_node * node)
{
    // 进入作用域
    if (node->needScope) {
        module->enterScope();
    }

    std::vector<ast_node *>::iterator pIter;
    for (pIter = node->sons.begin(); pIter != node->sons.end(); ++pIter) {

        // 遍历Block的每个语句，进行显示或者运算
        ast_node * temp = ir_visit_ast_node(*pIter);
        if (!temp) {
            return false;
        }

        node->blockInsts.addInst(temp->blockInsts);
    }

    // 离开作用域
    if (node->needScope) {
        module->leaveScope();
    }

    return true;
}

/// @brief 整数加法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_add(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 加法节点，左结合，先计算左节点，后计算右节点

    // 加法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 加法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * addInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_ADD_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(addInst);

    node->val = addInst;

    return true;
}

/// @brief 整数减法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_sub(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 加法节点，左结合，先计算左节点，后计算右节点

    // 加法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 加法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * subInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_SUB_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(subInst);

    node->val = subInst;

    return true;
}

/// @brief 赋值AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_assign(ast_node * node)
{
    ast_node * son1_node = node->sons[0];
    ast_node * son2_node = node->sons[1];

    // 赋值节点，自右往左运算

    // 赋值运算符的左侧操作数
    ast_node * left = ir_visit_ast_node(son1_node);
    if (!left) {
        // 某个变量没有定值
        // 这里缺省设置变量不存在则创建，因此这里不会错误
        return false;
    }

    // 赋值运算符的右侧操作数
    ast_node * right = ir_visit_ast_node(son2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    MoveInstruction * movInst = new MoveInstruction(module->getCurrentFunction(), left->val, right->val);

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(movInst);

    // 这里假定赋值的类型是一致的
    node->val = movInst;

    return true;
}

/// @brief return节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_return(ast_node * node)
{
    ast_node * right = nullptr;

    // return语句可能没有没有表达式，也可能有，因此这里必须进行区分判断
    if (!node->sons.empty()) {

        ast_node * son_node = node->sons[0];

        // 返回的表达式的指令保存在right节点中
        right = ir_visit_ast_node(son_node);
        if (!right) {

            // 某个变量没有定值
            return false;
        }
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理
    Function * currentFunc = module->getCurrentFunction();

    // 返回值存在时则移动指令到node中
    if (right) {

        // 创建临时变量保存IR的值，以及线性IR指令
        node->blockInsts.addInst(right->blockInsts);

        // 返回值赋值到函数返回值变量上，然后跳转到函数的尾部
        node->blockInsts.addInst(new MoveInstruction(currentFunc, currentFunc->getReturnValue(), right->val));

        node->val = right->val;
    } else {
        // 没有返回值
        node->val = nullptr;
    }

    // 跳转到函数的尾部出口指令上
    node->blockInsts.addInst(new GotoInstruction(currentFunc, currentFunc->getExitLabel()));

    return true;
}

/// @brief 类型叶子节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_type(ast_node * node)
{
    // 不需要做什么，直接从节点中获取即可。

    return true;
}

/// @brief 标识符叶子节点翻译成线性中间IR，变量声明的不走这个语句
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_var_id(ast_node * node)
{
    Value * val;

    // 查找ID型Value
    // 变量，则需要在符号表中查找对应的值

    val = module->findVarValue(node->name);

    node->val = val;

    return true;
}

/// @brief 无符号整数字面量叶子节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_uint(ast_node * node)
{
    ConstInt * val;

    // 新建一个整数常量Value
    val = module->newConstInt((int32_t) node->integer_val);

    node->val = val;

    return true;
}

/// @brief 变量声明语句节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_declare_statment(ast_node * node)
{
    bool result = false;

    for (auto & child: node->sons) {

        // 遍历每个变量声明
        result = ir_variable_declare(child);
        if (!result) {
            break;
        }
    }

    return result;
}

/// @brief 变量定声明节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_variable_declare(ast_node * node)
{
    // 共有两个孩子，第一个类型，第二个变量名

    // TODO 这里可强化类型等检查

    node->val = module->newVarValue(node->sons[0]->type, node->sons[1]->name);

    return true;
}

/// @brief 整数乘法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_mul(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 乘法节点，左结合，先计算左节点，后计算右节点

    // 乘法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 乘法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * mulInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_MUL_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(mulInst);

    node->val = mulInst;

    return true;
}

/// @brief 整数除法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_div(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 除法节点，左结合，先计算左节点，后计算右节点

    // 除法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 除法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * divInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_DIV_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(divInst);

    node->val = divInst;

    return true;
}

/// @brief 整数求余AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_mod(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 求余节点，左结合，先计算左节点，后计算右节点

    // 求余的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 求余的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * modInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_MOD_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(modInst);

    node->val = modInst;

    return true;
}

/// @brief 整数求负AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_neg(ast_node * node)
{
    ast_node * src_node = node->sons[0];

    // 求负的操作数
    ast_node * operand = ir_visit_ast_node(src_node);
    if (!operand) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    // 创建一个常量0
    ConstInt * zero = module->newConstInt(0);
    
    // 使用0减去操作数实现求负操作
    BinaryInstruction * negInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_SUB_I,
                                                        zero,
                                                        operand->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(operand->blockInsts);
    node->blockInsts.addInst(negInst);

    node->val = negInst;

    return true;
}

/// @brief 整数等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_eq(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 等于比较节点，左结合，先计算左节点，后计算右节点

    // 左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * eqInst = new BinaryInstruction(module->getCurrentFunction(),
                                                       IRInstOperator::IRINST_OP_EQ_I,
                                                       left->val,
                                                       right->val,
                                                       IntegerType::getTypeBool());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(eqInst);

    node->val = eqInst;

    return true;
}

/// @brief 整数不等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_neq(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 不等于比较节点，左结合，先计算左节点，后计算右节点

    // 左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * neqInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_NEQ_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeBool());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(neqInst);

    node->val = neqInst;

    return true;
}

/// @brief 整数小于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_lt(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 小于比较节点，左结合，先计算左节点，后计算右节点

    // 左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * ltInst = new BinaryInstruction(module->getCurrentFunction(),
                                                       IRInstOperator::IRINST_OP_LT_I,
                                                       left->val,
                                                       right->val,
                                                       IntegerType::getTypeBool());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(ltInst);

    node->val = ltInst;

    return true;
}

/// @brief 整数小于等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_le(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 小于等于比较节点，左结合，先计算左节点，后计算右节点

    // 左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * leInst = new BinaryInstruction(module->getCurrentFunction(),
                                                       IRInstOperator::IRINST_OP_LE_I,
                                                       left->val,
                                                       right->val,
                                                       IntegerType::getTypeBool());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(leInst);

    node->val = leInst;

    return true;
}

/// @brief 整数大于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_gt(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 大于比较节点，左结合，先计算左节点，后计算右节点

    // 左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * gtInst = new BinaryInstruction(module->getCurrentFunction(),
                                                       IRInstOperator::IRINST_OP_GT_I,
                                                       left->val,
                                                       right->val,
                                                       IntegerType::getTypeBool());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(gtInst);

    node->val = gtInst;

    return true;
}

/// @brief 整数大于等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_ge(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 大于等于比较节点，左结合，先计算左节点，后计算右节点

    // 左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * geInst = new BinaryInstruction(module->getCurrentFunction(),
                                                       IRInstOperator::IRINST_OP_GE_I,
                                                       left->val,
                                                       right->val,
                                                       IntegerType::getTypeBool());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(geInst);

    node->val = geInst;

    return true;
}

/// @brief 逻辑与AST节点翻译成线性中间IR（短路求值）
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_and(ast_node * node)
{
    ast_node * left_node = node->sons[0];
    ast_node * right_node = node->sons[1];

    // 左操作数
    ast_node * left = ir_visit_ast_node(left_node);
    if (!left) {
        return false;
    }

    // 创建标签用于短路求值
    LabelInstruction * rightLabel = create_new_label();
    LabelInstruction * falseLabel = create_new_label();
    LabelInstruction * endLabel = create_new_label();
    
    // 创建临时变量存储结果
    Value * resultVal = module->newVarValue(IntegerType::getTypeInt(), "");
    
    // 根据左操作数的值进行条件跳转：
    // 如果左操作数为假(0)，直接跳转到falseLabel，结果为假
    // 如果左操作数为真(非0)，跳转到rightLabel，计算右操作数
    CondBrInstruction * condBr = new CondBrInstruction(module->getCurrentFunction(), left->val, rightLabel, falseLabel);

    // 添加左侧计算指令和条件跳转
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(condBr);
    
    // 左操作数为假时，结果直接为假(0)
    node->blockInsts.addInst(falseLabel);
    ConstInt * zero = module->newConstInt(0);
    MoveInstruction * setFalse = new MoveInstruction(module->getCurrentFunction(), resultVal, zero);
    node->blockInsts.addInst(setFalse);
    GotoInstruction * gotoEnd = new GotoInstruction(module->getCurrentFunction(), endLabel);
    node->blockInsts.addInst(gotoEnd);
    
    // 左操作数为真时，计算右操作数
    node->blockInsts.addInst(rightLabel);
    
    // 右操作数
    ast_node * right = ir_visit_ast_node(right_node);
    if (!right) {
        return false;
    }
    
    // 结果为右操作数的值，表示逻辑与的结果
    MoveInstruction * moveRight = new MoveInstruction(module->getCurrentFunction(), resultVal, right->val);
    
    // 添加右侧计算指令和结果设置
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(moveRight);
    
    // 添加结束标签
    node->blockInsts.addInst(endLabel);
    
    // 设置节点的值为结果
    node->val = resultVal;
    
    return true;
}

/// @brief 逻辑或AST节点翻译成线性中间IR（短路求值）
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_or(ast_node * node)
{
    ast_node * left_node = node->sons[0];
    ast_node * right_node = node->sons[1];

    // 左操作数
    ast_node * left = ir_visit_ast_node(left_node);
    if (!left) {
        return false;
    }

    // 创建标签用于短路求值
    LabelInstruction * rightLabel = create_new_label();
    LabelInstruction * trueLabel = create_new_label();
    LabelInstruction * endLabel = create_new_label();
    
    // 创建临时变量存储结果
    Value * resultVal = module->newVarValue(IntegerType::getTypeInt(), "");
    
    // 根据左操作数的值进行条件跳转：
    // 如果左操作数为真(非0)，直接跳转到trueLabel，结果为真
    // 如果左操作数为假(0)，跳转到rightLabel，计算右操作数
    CondBrInstruction * condBr = new CondBrInstruction(module->getCurrentFunction(), left->val, trueLabel, rightLabel);

    // 添加左侧计算指令和条件跳转
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(condBr);
    
    // 左操作数为真时，结果直接为真(1)
    node->blockInsts.addInst(trueLabel);
    ConstInt * one = module->newConstInt(1);
    MoveInstruction * setTrue = new MoveInstruction(module->getCurrentFunction(), resultVal, one);
    node->blockInsts.addInst(setTrue);
    GotoInstruction * gotoEnd = new GotoInstruction(module->getCurrentFunction(), endLabel);
    node->blockInsts.addInst(gotoEnd);
    
    // 左操作数为假时，计算右操作数
    node->blockInsts.addInst(rightLabel);
    
    // 右操作数
    ast_node * right = ir_visit_ast_node(right_node);
    if (!right) {
        return false;
    }
    
    // 结果为右操作数的值，表示逻辑或的结果
    MoveInstruction * moveRight = new MoveInstruction(module->getCurrentFunction(), resultVal, right->val);
    
    // 添加右侧计算指令和结果设置
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(moveRight);
    
    // 添加结束标签
    node->blockInsts.addInst(endLabel);
    
    // 设置节点的值为结果
    node->val = resultVal;
    
    return true;
}

/// @brief 逻辑非AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_not(ast_node * node)
{
    ast_node * operand_node = node->sons[0];

    // 操作数
    ast_node * operand = ir_visit_ast_node(operand_node);
    if (!operand) {
        return false;
    }

    // 创建临时变量存储结果，这里使用整数类型，0表示false，非0表示true
    Value * resultVal = module->newVarValue(IntegerType::getTypeInt(), "");

    // 创建标签用于短路求值
    LabelInstruction * trueLabel = create_new_label();
    LabelInstruction * falseLabel = create_new_label();
    LabelInstruction * endLabel = create_new_label();
    
    // 条件判断：如果操作数为0（假），则结果为1（真）；否则结果为0（假）
    CondBrInstruction * condBr = new CondBrInstruction(module->getCurrentFunction(), operand->val, falseLabel, trueLabel);

    // 添加操作数计算指令和条件跳转
    node->blockInsts.addInst(operand->blockInsts);
    node->blockInsts.addInst(condBr);
    
    // 操作数为真时，结果为假(0)
    node->blockInsts.addInst(falseLabel);
    ConstInt * zero = module->newConstInt(0);
    MoveInstruction * setFalse = new MoveInstruction(module->getCurrentFunction(), resultVal, zero);
    node->blockInsts.addInst(setFalse);
    GotoInstruction * gotoEnd = new GotoInstruction(module->getCurrentFunction(), endLabel);
    node->blockInsts.addInst(gotoEnd);
    
    // 操作数为假时，结果为真(1)
    node->blockInsts.addInst(trueLabel);
    ConstInt * one = module->newConstInt(1);
    MoveInstruction * setTrue = new MoveInstruction(module->getCurrentFunction(), resultVal, one);
    node->blockInsts.addInst(setTrue);
    
    // 添加结束标签
    node->blockInsts.addInst(endLabel);
    
    // 设置节点的值为结果
    node->val = resultVal;
    
    return true;
}

/// @brief if语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_if(ast_node * node)
{
    // if语句有两个孩子：条件表达式和真分支语句
    ast_node * cond_node = node->sons[0];
    ast_node * then_node = node->sons[1];
    
    // 生成条件表达式
    ast_node * cond = ir_visit_ast_node(cond_node);
    if (!cond) {
        return false;
    }
    
    // 创建标签
    LabelInstruction * thenLabel = create_new_label();
    LabelInstruction * endLabel = create_new_label();
    
    // 生成条件跳转指令
    CondBrInstruction * condBr = new CondBrInstruction(module->getCurrentFunction(), cond->val, thenLabel, endLabel);
    
    // 添加条件计算和分支指令
    node->blockInsts.addInst(cond->blockInsts);
    node->blockInsts.addInst(condBr);
    
    // 添加真分支标签
    node->blockInsts.addInst(thenLabel);
    
    // 生成真分支语句
    ast_node * then_stmt = ir_visit_ast_node(then_node);
    if (!then_stmt) {
        return false;
    }
    
    // 添加真分支语句
    node->blockInsts.addInst(then_stmt->blockInsts);
    
    // 添加结束标签
    node->blockInsts.addInst(endLabel);
    
    return true;
}

/// @brief if-else语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_if_else(ast_node * node)
{
    // if-else语句有三个孩子：条件表达式、真分支语句和假分支语句
    ast_node * cond_node = node->sons[0];
    ast_node * then_node = node->sons[1];
    ast_node * else_node = node->sons[2];
    
    // 生成条件表达式
    ast_node * cond = ir_visit_ast_node(cond_node);
    if (!cond) {
        return false;
    }
    
    // 创建标签
    LabelInstruction * thenLabel = create_new_label();
    LabelInstruction * elseLabel = create_new_label();
    LabelInstruction * endLabel = create_new_label();
    
    // 生成条件跳转指令
    CondBrInstruction * condBr = new CondBrInstruction(module->getCurrentFunction(), cond->val, thenLabel, elseLabel);
    
    // 添加条件计算和分支指令
    node->blockInsts.addInst(cond->blockInsts);
    node->blockInsts.addInst(condBr);
    
    // 添加真分支标签
    node->blockInsts.addInst(thenLabel);
    
    // 生成真分支语句
    ast_node * then_stmt = ir_visit_ast_node(then_node);
    if (!then_stmt) {
        return false;
    }
    
    // 添加真分支语句和跳转到结束指令
    node->blockInsts.addInst(then_stmt->blockInsts);
    GotoInstruction * gotoEnd = new GotoInstruction(module->getCurrentFunction(), endLabel);
    node->blockInsts.addInst(gotoEnd);
    
    // 添加假分支标签
    node->blockInsts.addInst(elseLabel);
    
    // 生成假分支语句
    ast_node * else_stmt = ir_visit_ast_node(else_node);
    if (!else_stmt) {
        return false;
    }
    
    // 添加假分支语句
    node->blockInsts.addInst(else_stmt->blockInsts);
    
    // 添加结束标签
    node->blockInsts.addInst(endLabel);
    
    return true;
}

/// @brief while语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_while(ast_node * node)
{
    // while语句有两个孩子：条件表达式和循环体语句
    ast_node * cond_node = node->sons[0];
    ast_node * body_node = node->sons[1];
    
    // 创建标签
    LabelInstruction * condLabel = create_new_label();
    LabelInstruction * bodyLabel = create_new_label();
    LabelInstruction * endLabel = create_new_label();
    
    // 保存循环的入口和出口标签，用于break和continue语句
    loopEntryLabels.push(condLabel);
    loopExitLabels.push(endLabel);
    
    // 首先跳转到条件标签
    GotoInstruction * gotoCondition = new GotoInstruction(module->getCurrentFunction(), condLabel);
    node->blockInsts.addInst(gotoCondition);
    
    // 添加条件标签
    node->blockInsts.addInst(condLabel);
    
    // 生成条件表达式
    ast_node * cond = ir_visit_ast_node(cond_node);
    if (!cond) {
        // 记得弹出标签栈
        loopEntryLabels.pop();
        loopExitLabels.pop();
        return false;
    }
    
    // 生成条件跳转指令
    CondBrInstruction * condBr = new CondBrInstruction(module->getCurrentFunction(), cond->val, bodyLabel, endLabel);
    
    // 添加条件计算和分支指令
    node->blockInsts.addInst(cond->blockInsts);
    node->blockInsts.addInst(condBr);
    
    // 添加循环体标签
    node->blockInsts.addInst(bodyLabel);
    
    // 生成循环体语句
    ast_node * body_stmt = ir_visit_ast_node(body_node);
    if (!body_stmt) {
        // 记得弹出标签栈
        loopEntryLabels.pop();
        loopExitLabels.pop();
        return false;
    }
    
    // 添加循环体语句和跳转回条件判断
    node->blockInsts.addInst(body_stmt->blockInsts);
    GotoInstruction * gotoCondition2 = new GotoInstruction(module->getCurrentFunction(), condLabel);
    node->blockInsts.addInst(gotoCondition2);
    
    // 添加结束标签
    node->blockInsts.addInst(endLabel);
    
    // 处理完循环后，弹出标签栈
    loopEntryLabels.pop();
    loopExitLabels.pop();
    
    return true;
}

/// @brief break语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_break(ast_node * node)
{
    // 检查是否在循环内部
    if (loopExitLabels.empty()) {
        minic_log(LOG_ERROR, "break语句不在循环内部");
        return false;
    }
    
    // 获取当前循环的出口标签
    LabelInstruction * exitLabel = loopExitLabels.top();
    
    // 生成跳转指令
    GotoInstruction * gotoExit = new GotoInstruction(module->getCurrentFunction(), exitLabel);
    node->blockInsts.addInst(gotoExit);
    
    return true;
}

/// @brief continue语句AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_continue(ast_node * node)
{
    // 检查是否在循环内部
    if (loopEntryLabels.empty()) {
        minic_log(LOG_ERROR, "continue语句不在循环内部");
        return false;
    }
    
    // 获取当前循环的入口标签
    LabelInstruction * entryLabel = loopEntryLabels.top();
    
    // 生成跳转指令
    GotoInstruction * gotoEntry = new GotoInstruction(module->getCurrentFunction(), entryLabel);
    node->blockInsts.addInst(gotoEntry);

    return true;
}
