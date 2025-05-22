# MiniC编译器实验四 - 表达式支持

## 实验内容

本实验在MiniC编译器基础上实现了算术表达式支持，新增功能包括：
- 整数乘法运算 (`a * b`)
- 整数除法运算 (`a / b`)
- 整数求余运算 (`a % b`)
- 混合算术表达式计算与优先级处理

## 实验环境

- 操作系统：Ubuntu 22.04
- 编译工具：GCC/G++ 11.0+、CMake 3.10+
- 辅助工具：Flex、Bison或ANTLR4

## 构建与运行

```bash
cd exp04-minic-expr && mkdir -p build && cd build && cmake .. && make -j4
```

## 生成与验证

### 生成与验证IR（以test_expr.c为例）

```bash
# antlr4 生成IR
./minic -S -A -I -o ../tests/test-expr-antlr4.ir ../tests/test-expr.c

# flex+bison 生成IR
./minic -S -I -o ../tests/test-expr-flexbison.ir ../tests/test-expr.c

# 递归下降生成IR
./minic -S -D -I -o ../tests/test-expr-recursive.ir ../tests/test-expr.c

# 检查IR是否正确
cat ../tests/test-expr-antlr4.ir
cat ../tests/test-expr-flexbison.ir
cat ../tests/test-expr-recursive.ir
# 检查是否包含乘法(mul)、除法(div)和求余(mod)指令
# 检查指令序列是否符合表达式计算顺序


# 直接运行IR
# x86_64架构系统：对于生成的IR文件，可以使用IRCompiler直接运行
./tools/IRCompiler/Linux-x86_64/Ubuntu-22.04/IRCompiler -R ../tests/test-expr-antlr4.ir
# 由于本机为ARM64架构，可使用QEMU模拟器运行
qemu-x86_64 ./tools/IRCompiler/Linux-x86_64/Ubuntu-22.04/IRCompiler -R ../tests/test-expr-antlr4.ir
```


### 生成与验证ARM32汇编

```bash
# 生成ARM32汇编
./minic -S -A -o ../tests/test-expr-antlr4.s ../tests/test-expr.c
# 使用flex+bison生成
./minic -S -o ../tests/test-expr-flexbison.s ../tests/test-expr.c
# 使用递归下降生成
./minic -S -D -o ../tests/test-expr-recursive.s ../tests/test-expr.c

# 使用ARM交叉编译器验证汇编代码
arm-linux-gnueabihf-gcc -S -o ../tests/test-expr.s ../tests/test-expr.c

# 编译可执行程序
arm-linux-gnueabihf-gcc -static -g -o ../tests/test-expr-antlr4 ../tests/test-expr-antlr4.s
arm-linux-gnueabihf-gcc -static -g -o ../tests/test-expr-flexbison ../tests/test-expr-flexbison.s
arm-linux-gnueabihf-gcc -static -g -o ../tests/test-expr-recursive ../tests/test-expr-recursive.s
arm-linux-gnueabihf-gcc -static -g -o ../tests/test-expr ../tests/test-expr.s

# 运行验证
../tests/test-expr
../tests/test-expr-antlr4
../tests/test-expr-flexbison
../tests/test-expr-recursive
```

### 生成与查看AST

```bash
# 生成AST树状图 (test-expr.c生成的图片太复杂，这里以test1-1.c为例)
# antlr4
./build/minic -S -T -A -o ./tests/test1-1-antlr4.png ./tests/test1-1.c
# flex+bison
./build/minic -S -T -o ./tests/test1-1-flexbison.png ./tests/test1-1.c
# 递归下降
./build/minic -S -T -D -o ./tests/test1-1-recursive.png ./tests/test1-1.c
```

## 主要修改的文件

- `frontend/AST.h`：增加了乘法、除法和求余相关的AST节点类型定义
- `frontend/AST.cpp`：实现了新增AST节点的处理逻辑
- Antlr4:
  - `miniC.g4`：新增乘法、除法和求余运算符
  - `Antlr4CSTVisitor.cpp`：新增乘法、除法和求余运算符的AST节点处理
  - `Antlr4CSTVisitor.h`：新增乘法、除法和求余运算符的AST节点处理
- Flex+Bison:
  - `minic.y`：更新语法规则，支持新增的表达式运算符
  - `minic.l`：添加新的词法单元识别
- 递归下降：
  - `RecursiveDescentFlex.cpp`：新增乘法、除法和求余运算符的词法分析
  - `RecursiveDescentParser.cpp`：新增乘法、除法和求余运算符的语法分析
  - `RecursiveDescentParser.h`：新增乘法、除法和求余运算符的语法分析
- IR翻译：
  - `ir/Generator/IRGenerator.cpp`：实现了乘法(ir_mul)、除法(ir_div)和求余(ir_mod)函数
  - `ir/Generator/IRGenerator.h`：声明了新增的IR生成函数
  - `ir/Instructions/BinaryInstruction.*`：支持新增二元运算指令
  - `ir/Instruction.h`：添加了IRINST_OP_MUL_I、IRINST_OP_DIV_I和IRINST_OP_MOD_I操作码
- 翻译为ARM32汇编：
  - `InstSelectorArm32.cpp`：新增了乘法、除法和求余运算符的指令选择
  - `InstSelectorArm32.h`：新增了乘法、除法和求余运算符的指令选择

