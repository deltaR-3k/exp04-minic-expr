///
/// @file LoadInstruction.h
/// @brief Load指令，也就是DragonIR的内存加载指令 %dst = *%src
///
/// @author AI Assistant
/// @version 1.0
/// @date 2024-12-20
///
/// @copyright Copyright (c) 2024
///
#pragma once

#include <string>

#include "Value.h"
#include "Instruction.h"

class Function;

///
/// @brief 内存加载指令 %dst = *%src
///
class LoadInstruction : public Instruction {

public:
    ///
    /// @brief 构造函数
    /// @param _func 所属的函数
    /// @param result 目标操作数
    /// @param srcAddr 源地址操作数
    ///
    LoadInstruction(Function * _func, Value * result, Value * srcAddr);

    /// @brief 转换成字符串
    void toString(std::string & str) override;
}; 