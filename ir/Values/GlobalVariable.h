///
/// @file GlobalVariable.h
/// @brief 全局变量描述类
///
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-09-29
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-09-29 <td>1.0     <td>zenglj  <td>新建
/// </table>
///
#pragma once

#include "GlobalValue.h"
#include "IRConstant.h"
#include "ArrayType.h"

///
/// @brief 全局变量，寻址时通过符号名或变量名来寻址
///
class GlobalVariable : public GlobalValue {

public:
    ///
    /// @brief 构建全局变量，默认对齐为4字节
    /// @param _type 类型
    /// @param _name 名字
    ///
    explicit GlobalVariable(Type * _type, std::string _name) : GlobalValue(_type, _name)
    {
        // 设置对齐大小
        setAlignment(4);
    }

    ///
    /// @brief  检查是否是函数
    /// @return true 是函数
    /// @return false 不是函数
    ///
    [[nodiscard]] bool isGlobalVarible() const override
    {
        return true;
    }

    ///
    /// @brief 是否属于BSS段的变量，即未初始化过的变量，或者初值都为0的变量
    /// @return true
    /// @return false
    ///
    [[nodiscard]] bool isInBSSSection() const
    {
        return this->inBSSSection;
    }

    ///
    /// @brief 取得变量所在的作用域层级
    /// @return int32_t 层级
    ///
    int32_t getScopeLevel() override
    {
        return 0;
    }

    ///
    /// @brief 对该Value进行Load用的寄存器编号
    /// @return int32_t 寄存器编号
    ///
    int32_t getLoadRegId() override
    {
        return this->loadRegNo;
    }

    ///
    /// @brief 对该Value进行Load用的寄存器编号
    /// @return int32_t 寄存器编号
    ///
    void setLoadRegId(int32_t regId) override
    {
        this->loadRegNo = regId;
    }

    ///
    /// @brief 设置全局变量的初始化值
    /// @param val 初始化值
    ///
    void setInitValue(ConstInt * val)
    {
        this->initValue = val;
        this->inBSSSection = false;  // 有初始化值，不在BSS段
    }
    
    ///
    /// @brief 获取全局变量的初始化值
    /// @return ConstInt* 初始化值，如果没有则返回nullptr
    ///
    ConstInt * getInitValue() const
    {
        return this->initValue;
    }

    ///
    /// @brief Declare指令IR显示
    /// @param str
    ///
    void toDeclareString(std::string & str)
    {
        // 如果有初始化值，使用赋值格式：@var = value
        if (initValue != nullptr) {
            str = "declare " + getType()->toString() + " " + getIRName() + " = " + std::to_string(initValue->getVal());
            return;
        }
        
        if (getType()->isArrayType()) {
            // 数组类型：declare i32 @a[5]
            ArrayType * arrayType = static_cast<ArrayType *>(getType());
            std::string elementTypeStr = arrayType->getElementType()->toString();
            std::string varNameWithDims = getIRName();
            for (int dim : arrayType->getDimensions()) {
                varNameWithDims += "[" + std::to_string(dim) + "]";
            }
            str = "declare " + elementTypeStr + " " + varNameWithDims;
        } else {
            // 普通变量：declare i32 @var
            str = "declare " + getType()->toString() + " " + getIRName();
        }
    }

private:
    ///
    /// @brief 变量加载到寄存器中时对应的寄存器编号
    ///
    int32_t loadRegNo = -1;

    ///
    /// @brief 默认全局变量在BSS段，没有初始化，或者即使初始化过，但都值都为0
    ///
    bool inBSSSection = true;
    
    ///
    /// @brief 全局变量的初始化值
    ///
    ConstInt * initValue = nullptr;
};
