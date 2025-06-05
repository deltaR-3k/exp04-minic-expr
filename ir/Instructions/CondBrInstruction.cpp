///
/// @file CondBrInstruction.cpp
/// @brief 条件跳转指令
///
/// @author chensp (imchensp@gmail.com)
/// @version 1.0
/// @date 2025-05-22
///
/// @copyright Copyright (c) 2025
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2025-05-22 <td>1.0     <td>chensp  <td>新建
/// </table>
///

#include "VoidType.h"
#include "CondBrInstruction.h"

///
/// @brief 条件跳转指令的构造函数
/// @param _func 所属函数
/// @param _cond 条件值
/// @param _trueTarget 条件为真时跳转目标
/// @param _falseTarget 条件为假时跳转目标
///
CondBrInstruction::CondBrInstruction(Function * _func, Value * _cond, LabelInstruction * _trueTarget, LabelInstruction * _falseTarget)
    : Instruction(_func, IRInstOperator::IRINST_OP_COND_BR, VoidType::getType())
{
    condition = _cond;
    trueTarget = _trueTarget;
    falseTarget = _falseTarget;
    
    // 添加操作数
    addOperand(_cond);
}

/// @brief 转换成IR指令文本
void CondBrInstruction::toString(std::string & str)
{
    str = "bc " + condition->getIRName() + ", label " + trueTarget->getIRName() + ", label " + falseTarget->getIRName();
}

///
/// @brief 获取条件值
/// @return 条件值
///
Value * CondBrInstruction::getCondition() const
{
    return condition;
}

///
/// @brief 获取条件为真时的跳转目标
/// @return 真分支目标
///
LabelInstruction * CondBrInstruction::getTrueTarget() const
{
    return trueTarget;
}

///
/// @brief 获取条件为假时的跳转目标
/// @return 假分支目标
///
LabelInstruction * CondBrInstruction::getFalseTarget() const
{
    return falseTarget;
} 