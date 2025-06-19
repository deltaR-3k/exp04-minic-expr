///
/// @file StoreInstruction.cpp
/// @brief Store指令，也就是DragonIR的内存存储指令 *%dst = %src
///
/// @author AI Assistant
/// @version 1.0
/// @date 2024-12-20
///
/// @copyright Copyright (c) 2024
///

#include "VoidType.h"
#include "StoreInstruction.h"

///
/// @brief 构造函数
/// @param _func 所属的函数
/// @param dstAddr 目标地址操作数
/// @param srcVal 源值操作数
///
StoreInstruction::StoreInstruction(Function * _func, Value * dstAddr, Value * srcVal)
    : Instruction(_func, IRInstOperator::IRINST_OP_ASSIGN, VoidType::getType())
{
    addOperand(dstAddr);
    addOperand(srcVal);
}

/// @brief 转换成字符串显示
/// @param str 转换后的字符串
void StoreInstruction::toString(std::string & str)
{
    Value *dstAddr = getOperand(0), *srcVal = getOperand(1);
    str = "*" + dstAddr->getIRName() + " = " + srcVal->getIRName();
} 