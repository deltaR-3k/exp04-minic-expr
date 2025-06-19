///
/// @file StoreInstruction.h
/// @brief Store指令，也就是DragonIR的内存存储指令 *%dst = %src
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
/// @brief 内存存储指令 *%dst = %src
///
class StoreInstruction : public Instruction {

public:
    ///
    /// @brief 构造函数
    /// @param _func 所属的函数
    /// @param dstAddr 目标地址操作数
    /// @param srcVal 源值操作数
    ///
    StoreInstruction(Function * _func, Value * dstAddr, Value * srcVal);

    /// @brief 转换成字符串
    void toString(std::string & str) override;
}; 