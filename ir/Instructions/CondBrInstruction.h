///
/// @file CondBrInstruction.h
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
#pragma once

#include <string>

#include "Instruction.h"
#include "LabelInstruction.h"
#include "Function.h"

///
/// @brief 条件跳转指令
///
class CondBrInstruction final : public Instruction {

public:
    ///
    /// @brief 条件跳转指令的构造函数
    /// @param _func 所属函数
    /// @param _cond 条件值
    /// @param _trueTarget 条件为真时跳转目标
    /// @param _falseTarget 条件为假时跳转目标
    ///
    CondBrInstruction(Function * _func, Value * _cond, LabelInstruction * _trueTarget, LabelInstruction * _falseTarget);

    /// @brief 转换成字符串
    void toString(std::string & str) override;

    ///
    /// @brief 获取条件值
    /// @return 条件值
    ///
    [[nodiscard]] Value * getCondition() const;

    ///
    /// @brief 获取条件为真时的跳转目标
    /// @return 真分支目标
    ///
    [[nodiscard]] LabelInstruction * getTrueTarget() const;

    ///
    /// @brief 获取条件为假时的跳转目标
    /// @return 假分支目标
    ///
    [[nodiscard]] LabelInstruction * getFalseTarget() const;

private:
    ///
    /// @brief 条件值
    ///
    Value * condition;

    ///
    /// @brief 条件为真时跳转目标
    ///
    LabelInstruction * trueTarget;

    ///
    /// @brief 条件为假时跳转目标
    ///
    LabelInstruction * falseTarget;
}; 