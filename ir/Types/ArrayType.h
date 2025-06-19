///
/// @file ArrayType.h
/// @brief 数组类型描述类
///
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-12-05
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-12-05 <td>1.0     <td>zenglj  <td>新建
/// </table>
///

#pragma once

#include "Type.h"
#include <vector>

///
/// @brief 数组类型，用于描述一维和多维数组
///
class ArrayType : public Type {

public:
    ///
    /// @brief 构造函数
    /// @param _elementType 数组元素类型
    /// @param _dimensions 数组各维度大小
    ///
    ArrayType(Type * _elementType, const std::vector<int> & _dimensions) 
        : Type(ArrayTyID), elementType(_elementType), dimensions(_dimensions)
    {
        // 计算数组总大小
        totalElements = 1;
        for (int dim : dimensions) {
            totalElements *= dim;
        }
    }

    ///
    /// @brief 获取元素类型
    /// @return Type* 元素类型
    ///
    [[nodiscard]] Type * getElementType() const
    {
        return elementType;
    }

    ///
    /// @brief 获取数组维度信息
    /// @return const std::vector<int>& 维度列表
    ///
    [[nodiscard]] const std::vector<int> & getDimensions() const
    {
        return dimensions;
    }

    ///
    /// @brief 获取数组总元素个数
    /// @return int 总元素个数
    ///
    [[nodiscard]] int getTotalElements() const
    {
        return totalElements;
    }

    ///
    /// @brief 获得类型所占内存空间大小
    /// @return int32_t 字节数
    ///
    [[nodiscard]] int32_t getSize() const override
    {
        return totalElements * elementType->getSize();
    }

    ///
    /// @brief 转换字符串
    /// @return 字符串表示
    ///
    [[nodiscard]] std::string toString() const override
    {
        std::string result = elementType->toString();
        for (int dim : dimensions) {
            result += "[" + std::to_string(dim) + "]";
        }
        return result;
    }

private:
    ///
    /// @brief 数组元素类型
    ///
    Type * elementType;

    ///
    /// @brief 数组各维度大小
    ///
    std::vector<int> dimensions;

    ///
    /// @brief 数组总元素个数
    ///
    int totalElements;
}; 