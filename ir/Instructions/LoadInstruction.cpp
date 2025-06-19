///
/// @file LoadInstruction.cpp
/// @brief Load指令，也就是DragonIR的内存加载指令 %dst = *%src
///
/// @author AI Assistant
/// @version 1.0
/// @date 2024-12-20
///
/// @copyright Copyright (c) 2024
///

#include "VoidType.h"
#include "LoadInstruction.h"

///
/// @brief 构造函数
/// @param _func 所属的函数
/// @param result 目标操作数
/// @param srcAddr 源地址操作数
///
LoadInstruction::LoadInstruction(Function * _func, Value * result, Value * srcAddr)
    : Instruction(_func, IRInstOperator::IRINST_OP_ASSIGN, VoidType::getType())
{
    addOperand(result);
    addOperand(srcAddr);
}

/// @brief 转换成字符串显示
/// @param str 转换后的字符串
void LoadInstruction::toString(std::string & str)
{
    Value *dstVal = getOperand(0), *srcAddr = getOperand(1);
    str = dstVal->getIRName() + " = *" + srcAddr->getIRName();
} 