# 实验四-MiniC强化报告

<center><div style='height:2mm;'></div><div style="font-family:华文楷体;font-size:14pt;">陈诗鹏，2022303330</div></center>

## MiniC基础版分析及评价

MiniC编译器作为一种精简版的C语言编译器，旨在提供C语言的核心功能支持。目前版本的编译器实现了基础的语言特性，包括对int类型全局变量的支持（尚未支持变量初始化），多函数定义功能（仅限于无参数且返回值为int类型的函数），局部变量声明（允许在任意语句块位置定义int类型变量），基本赋值语句（不支持连续赋值），嵌套语句块，加减运算和括号表达式，以及内置函数putint用于终端显示。此外，编译器还支持变量分层管理机制，使得不同作用域中可以存在同名变量。

### 词法分析

词法分析方面，MiniC编译器提供了三种实现技术。经典的Flex词法分析器通过正规式描述单词模式，在识别单词后返回相应的Token类型码，具有强大的表达能力和高效的分析代码生成能力，能与Bison语法分析器无缝配合，但需要学习特定语法且生成代码可读性较差。而现代的Antlr4词法分析器则采用更为一体化的设计思路，将词法和语法规则统一在同一文件中定义，不仅提供丰富的错误恢复机制，还支持多种目标语言，自带可视化工具便于调试，尽管生成代码体积较大且运行效率可能略低。手写的递归下降词法分析器则通过直接编码实现，给予开发者完全的分析过程控制权，便于针对特定语言特性进行优化和灵活的错误处理，但开发工作量大且容易引入bug，扩展维护也相对困难。

### 语法分析

语法分析技术同样采用了三种不同实现。Bison语法分析器基于LR(1)文法，通过产生式规则描述语言结构，能自动处理移进-归约冲突，与Flex配合良好，生成高效分析器，但要求开发者理解LR分析原理，文法冲突处理复杂且错误恢复机制有限。Antlr4语法分析器则采用Adaptive LL(*)分析技术，能自动处理左递归问题，通过访问者模式构建AST，支持直接左递归、强大的错误恢复机制和语法预测回溯功能，生成的分析器可读性好，不过在处理某些复杂文法时可能效率较低。手写递归下降语法分析器通过相互递归函数实现，每个函数对应文法中的一个非终结符，分析过程直观清晰，错误处理灵活，但只能处理LL(1)文法，需要手动消除左递归，难以处理复杂语法结构。

### AST与符号表

在抽象语法树和符号表设计方面，MiniC编译器使用`ast_node`类表示AST节点，每个节点包含节点类型、源码行号、值类型、字面量值、变量或函数名、子节点列表以及IR相关信息等属性。AST节点类型通过枚举类定义，包括叶子节点和内部节点两大类。这种设计结构清晰，直观表示程序层次结构，便于语义分析和代码生成，但内存占用较大，节点类型较多导致处理逻辑复杂。符号表则由`Module`类管理，包含函数映射表、全局变量映射表、常量表和作用域栈等组件，支持作用域进出、函数和变量的创建查找等基本操作，能有效支持变量分层管理和高效符号查找，但缺少对复杂类型的支持和严格的类型检查机制。

### Dragon IR生成

线性IR生成过程由`IRGenerator`类实现，通过遍历AST为每个节点生成对应IR指令，处理全局变量、函数定义、语句块、局部变量、表达式和函数调用等多种程序结构。IR指令类型包括二元运算指令、赋值指令、函数调用指令、参数指令、标签和跳转指令以及函数入口出口指令等。这一中间表示将AST转换为更接近目标代码的形式，便于代码优化和简化后端生成，不过当前的IR指令集较为简单，缺少对复杂操作和控制流的完整支持，类型系统也相对简单。

### ARM32汇编生成

ARM32代码生成由`CodeGeneratorArm32`类负责，主要包括指令选择、寄存器分配、栈帧管理和最终汇编代码输出等步骤。指令选择通过映射表将IR指令转换为ARM32指令序列，寄存器分配则采用简单策略将变量和临时值映射到寄存器。这种模块化设计便于扩展和维护，能支持基本ARM32指令集并生成可执行汇编代码，但寄存器分配策略简单且缺少针对ARM32特定优化，导致生成代码效率不高。

### 改进建议

1. 可以扩展语言特性：支持更多基本类型（如float、char等），添加数组、结构体、指针等复杂类型支持，实现更完整的表达式系统，包括逻辑运算、位运算等，支持if-else、while、for等控制流语句
2. 增强符号表功能：实现更严格的类型检查和类型转换，增加对函数重载的支持
3. 改进代码优化：实现基本的优化技术，如常量折叠、公共子表达式消除，实现循环优化和内联优化




## MiniC编译器功能扩展实现

在前一部分中，对MiniC编译器的基本架构和功能进行了全面分析。本部分将详细介绍本实验为编译器新增的功能及其实现方法，包括对词法分析、语法分析、中间代码生成以及后端代码生成的扩展。

### 功能扩展概述

为了增强MiniC编译器的表达能力，实验四实现了以下新功能：

- 支持多种进制的无符号整数字面量，包括十进制、八进制(0开头)和十六进制(0x或0X开头)
- 添加单目运算符求负运算(-)
- 支持乘法(*)、除法(/)和求余(%)运算
- 扩展IR指令集以支持新增运算
- 增强ARM32后端以支持新增指令的代码生成

下面将详细介绍各部分的设计与实现：

从前端的词法分析和语法分析，到中间代码生成，再到后端的目标代码生成。



### 前端扩展实现

#### 词法分析扩展

为支持多种进制的整数字面量，使用三种词法分析技术进行扩展：

**Flex词法分析器扩展**

在Flex词法分析器中，添加对八进制和十六进制数字的识别规则：

```lex
0[0-7]+{
          // 八进制数字识别
          yylval.integer_num.val = (uint32_t)strtol(yytext, (char **)NULL, 8);
          yylval.integer_num.lineno = yylineno;
          return T_DIGIT;
      }

0[xX][0-9a-fA-F]+ {
         // 十六进制数字识别
         yylval.integer_num.val = (uint32_t)strtol(yytext, (char **)NULL, 16);
         yylval.integer_num.lineno = yylineno;
         return T_DIGIT;
       }
```

同时，保留原有的十进制数字识别规则，并添加了对符号的识别：

```lex
"-"         { return T_SUB; }
"*"         { return T_MUL; }
"/"         { return T_DIV; }
"%"         { return T_MOD; }
```

**Antlr4词法分析器扩展**

在Antlr4的语法文件中，修改了T_DIGIT的定义，使其支持多种进制：

```antlr
T_DIGIT: '0' [0-7]* | '0' [xX] [0-9a-fA-F]+ | [1-9][0-9]*;
```

同时在访问者模式中，添加对不同进制数字的解析逻辑：

```cpp
        std::string digitText = ctx->T_DIGIT()->getText();
        uint32_t val;
        
        // 解析八进制数字（以0开头，但不是0x或0X开头）
        if (digitText.size() > 1 && digitText[0] == '0' && 
            (digitText.size() < 2 || (digitText[1] != 'x' && digitText[1] != 'X'))) {
            val = (uint32_t) stoull(digitText, nullptr, 8);
        }
        // 解析十六进制数字（以0x或0X开头）
        else if (digitText.size() > 2 && digitText[0] == '0' && 
                (digitText[1] == 'x' || digitText[1] == 'X')) {
            val = (uint32_t) stoull(digitText, nullptr, 16);
        }
        // 解析十进制数字
        else {
            val = (uint32_t) stoull(digitText, nullptr, 10);
        }
    }
    // 其他终结符处理...
}
```

**手写递归下降词法分析器扩展**

在手写的递归下降词法分析器中，实现一个更复杂的数字识别函数：

```cpp
// 处理八进制和十六进制数字
if (c == '0') {
rd_lval.integer_num.val = 0;
c = fgetc(rd_filein);

// 处理十六进制数字 (0x 或 0X)
if (c == 'x' || c == 'X') {
    c = fgetc(rd_filein);
    if (isxdigit(c)) {
        // 解析十六进制数字
        do {
            if (isdigit(c)) {
                rd_lval.integer_num.val = rd_lval.integer_num.val * 16 + (c - '0');
            } else if (c >= 'a' && c <= 'f') {
                rd_lval.integer_num.val = rd_lval.integer_num.val * 16 + (c - 'a' + 10);
            } else if (c >= 'A' && c <= 'F') {
                rd_lval.integer_num.val = rd_lval.integer_num.val * 16 + (c - 'A' + 10);
            }
            c = fgetc(rd_filein);
        } while (isxdigit(c));
    } else {
        // 错误：0x 后面没有有效的十六进制数字
        printf("Line(%lld): Invalid hexadecimal digit\n", (long long) rd_line_no);
        tokenKind = RDTokenType::T_ERR;
        ungetc(c, rd_filein);
        return tokenKind;
    }
} 
// 处理八进制数字 (0开头)
else if (c >= '0' && c <= '7') {
    do {
        rd_lval.integer_num.val = rd_lval.integer_num.val * 8 + (c - '0');
        c = fgetc(rd_filein);
    } while (c >= '0' && c <= '7');
}
// 单个0
else {
    // 已经读取了一个0，不需要做其他处理
}
} 
```

#### 语法分析扩展

为支持新增的运算符，对三种语法分析技术进行了扩展：

**Bison语法分析器扩展**

在Bison语法文件中，添加新的运算符和相应的产生式：

```bison
// 新增运算符
%token T_ASSIGN T_SUB T_ADD T_MUL T_DIV T_MOD

// 定义运算符优先级和结合性
%left T_ADD T_SUB
%left T_MUL T_DIV T_MOD
%right UMINUS  // 单目求负运算符的优先级

%type <node> AddExp MulExp UnaryExp PrimaryExp
%type <op_class> AddOp MulOp
```

同时，为新增的语法规则添加了相应的语义动作，构建AST节点：

```bison
// 乘除求余运算符
MulOp: T_MUL {
		$$ = (int)ast_operator_type::AST_OP_MUL;
	}
	| T_DIV {
		$$ = (int)ast_operator_type::AST_OP_DIV;
	}
	| T_MOD {
		$$ = (int)ast_operator_type::AST_OP_MOD;
	}
	;

UnaryExp : PrimaryExp 
	| T_SUB UnaryExp %prec UMINUS {
		// 单目求负运算

		// 创建单目求负运算节点，其孩子为UnaryExp($2)
		$$ = create_contain_node(ast_operator_type::AST_OP_NEG, $2);
	}
```

**Antlr4语法分析器扩展**

在Antlr4的语法文件中，修改增加表达式相关的规则：

```antlr
// 表达式文法扩展
expr: addExp;

// 加减表达式
addExp: mulExp (addOp mulExp)*;

// 乘除模表达式
mulExp: unaryExp (mulOp unaryExp)*;

// 一元表达式
unaryExp: primaryExp | T_SUB unaryExp | T_ID T_L_PAREN realParamList? T_R_PAREN;

// 乘除模运算符
mulOp: T_MUL | T_DIV | T_MOD;
```

在访问者模式中，我们实现了对应的访问方法：

```cpp
std::any MiniCCSTVisitor::visitMulExp(MiniCParser::MulExpContext * ctx)
{
    // 识别的文法产生式：mulExp : unaryExp (mulOp unaryExp)*;

    if (ctx->mulOp().empty()) {

        // 没有mulOp运算符，则说明闭包识别为0，只识别了第一个非终结符unaryExp
        return visitUnaryExp(ctx->unaryExp()[0]);
    }

    ast_node *left, *right;

    // 存在mulOp运算符
    auto opsCtxVec = ctx->mulOp();

    // 有操作符，肯定会进循环，使得right设置正确的值
    for (int k = 0; k < (int) opsCtxVec.size(); k++) {

        // 获取运算符
        ast_operator_type op = std::any_cast<ast_operator_type>(visitMulOp(opsCtxVec[k]));

        if (k == 0) {

            // 左操作数
            left = std::any_cast<ast_node *>(visitUnaryExp(ctx->unaryExp()[k]));
        }

        // 右操作数
        right = std::any_cast<ast_node *>(visitUnaryExp(ctx->unaryExp()[k + 1]));

        // 新建结点作为下一个运算符的右操作符
        left = ast_node::New(op, left, right, nullptr);
    }

    return left;
}

/// @brief 非终结运算符mulOp的遍历
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitMulOp(MiniCParser::MulOpContext * ctx)
{
    // 识别的文法产生式：mulOp : T_MUL | T_DIV | T_MOD

    if (ctx->T_MUL()) {
        return ast_operator_type::AST_OP_MUL;
    } else if (ctx->T_DIV()) {
        return ast_operator_type::AST_OP_DIV;
    } else {
        return ast_operator_type::AST_OP_MOD;
    }
}

antlrcpp::Any MiniCAntlrVisitor::visitUnaryExp(MiniCParser::UnaryExpContext *ctx) {
    } else if (ctx->T_SUB()) {
        // 单目求负运算
        auto operand = std::any_cast<ast_node *>(visitUnaryExp(ctx->unaryExp()));
        return ast_node::New(ast_operator_type::AST_OP_NEG, operand, nullptr);
    // 其他情况处理...
}
```

**手写递归下降语法分析器扩展**

在手写的递归下降语法分析器中，添加新的分析函数：

```cpp
ast_operator_type mulOp()
{
    ast_operator_type type = ast_operator_type::AST_OP_MAX;

    if (F(T_MUL)) {
        type = ast_operator_type::AST_OP_MUL;
        // 跳过当前的记号，指向下一个记号
        advance();
    } else if (F(T_DIV)) {
        type = ast_operator_type::AST_OP_DIV;
        // 跳过当前的记号，指向下一个记号
        advance();
    } else if (F(T_MOD)) {
        type = ast_operator_type::AST_OP_MOD;
        // 跳过当前的记号，指向下一个记号
        advance();
    }

    return type;
}

static ast_node * mulExp()
{
    // 识别第一个unaryExp
    ast_node * left_node = unaryExp();
    if (!left_node) {
        // 非法的一元表达式
        return nullptr;
    }

    // 识别闭包(mulOp unaryExp)*，循环
    // 循环退出条件，1) 不是二元乘除模运算符， 2) 语法错误
    for (;;) {
        // 获取乘除模运算符
        ast_operator_type op = mulOp();
        if (ast_operator_type::AST_OP_MAX == op) {
            // 不是乘除模运算符则正常结束
            break;
        }

        // 获取右侧表达式
        ast_node * right_node = unaryExp();
        if (!right_node) {
            // 二元乘除模运算没有合法的右侧表达式
            break;
        }

        // 创建二元运算符节点
        left_node = create_contain_node(op, left_node, right_node);
    }

    return left_node;
}
```

**额外的，完成了识别单行注释与多行注释识别的TODO：**

```cpp
    // 处理注释，包括单行注释和多行注释
    if (c == '/') {
        // 获取下一个字符
        int next_c = fgetc(rd_filein);
        
        // 处理单行注释 //
        if (next_c == '/') {
            // 读取直到行尾或文件结束
            while ((c = fgetc(rd_filein)) != EOF && c != '\n' && c != '\r') {
                // 继续读取，直到行尾
            }
            
            // 如果遇到了换行符，增加行号
            if (c == '\r') {
                c = fgetc(rd_filein);
                rd_line_no++;
                if (c != '\n') {
                    // 不是\n，则回退
                    ungetc(c, rd_filein);
                }
            } else if (c == '\n') {
                rd_line_no++;
            } else if (c == EOF) {
                // 文件结束
                return RDTokenType::T_EOF;
            }
            
            // 递归调用以获取下一个有效的token
            return rd_flex();
        }
        // 处理多行注释 /* */
        else if (next_c == '*') {
            bool comment_ended = false;
            
            // 读取直到遇到 */
            while (!comment_ended && (c = fgetc(rd_filein)) != EOF) {
                if (c == '*') {
                    c = fgetc(rd_filein);
                    if (c == '/') {
                        comment_ended = true;
                    } else if (c != EOF) {
                        // 如果不是'/'，回退以便继续检查
                        ungetc(c, rd_filein);
                    }
                }
                // 处理注释中的换行
                else if (c == '\r') {
                    c = fgetc(rd_filein);
                    rd_line_no++;
                    if (c != '\n') {
                        // 不是\n，则回退
                        ungetc(c, rd_filein);
                    }
                } else if (c == '\n') {
                    rd_line_no++;
                }
            }
            
            if (!comment_ended && c == EOF) {
                // 如果文件结束但注释没有结束，报错
                printf("Line(%lld): Unterminated comment\n", (long long) rd_line_no);
                return RDTokenType::T_ERR;
            }
            
            // 递归调用以获取下一个有效的token
            return rd_flex();
        }
        // 如果不是注释，而是除法运算符
        else {
            // 回退下一个字符，因为它不是注释的一部分
            ungetc(next_c, rd_filein);
            
            // 识别字符/
            tokenKind = RDTokenType::T_DIV;
            // 存储字符/
            tokenValue = "/";
            return tokenKind;
        }
    }

```



### 中间代码生成扩展

为了支持新增的运算符，扩展IR指令集和代码生成逻辑：

#### IR指令集扩展

首先，在`IRInstOperator`枚举中添加新的指令类型：

```cpp
enum class IRInstOperator : int {
    // 原有指令...
    IRINST_OP_ADD_I,
    IRINST_OP_SUB_I,
    // 新增指令
    IRINST_OP_MUL_I,
    IRINST_OP_DIV_I,
    IRINST_OP_MOD_I,
    IRINST_OP_NEG_I,
    // 其他指令...
};
```

然后，扩展`BinaryInstruction`类，使其支持新增的二元运算：

```cpp
void BinaryInstruction::toString(std::string & str)
{
    Value *src1 = getOperand(0), *src2 = getOperand(1);

    switch (op) {
        case IRInstOperator::IRINST_OP_ADD_I:
            str = getIRName() + " = add " + src1->getIRName() + "," + src2->getIRName();
            break;
        case IRInstOperator::IRINST_OP_SUB_I:
            str = getIRName() + " = sub " + src1->getIRName() + "," + src2->getIRName();
            break;
        // 新增指令的字符串表示
        case IRInstOperator::IRINST_OP_MUL_I:
            // 乘法指令，二元运算
            str = getIRName() + " = mul " + src1->getIRName() + "," + src2->getIRName();
            break;
        case IRInstOperator::IRINST_OP_DIV_I:
            // 除法指令，二元运算
            str = getIRName() + " = div " + src1->getIRName() + "," + src2->getIRName();
            break;
        case IRInstOperator::IRINST_OP_MOD_I:
            // 求余指令，二元运算
            str = getIRName() + " = mod " + src1->getIRName() + "," + src2->getIRName();
            break;
    }
}
```



#### AST到IR的转换扩展

扩展`IRGenerator`类，使其能够处理新增的AST节点类型：

```cpp
bool IRGenerator::visit(ast_node* node) {
    switch (node->node_type) {
        // 原有节点类型处理...
        case ast_operator_type::AST_OP_ADD:
            return visitAddNode(node);
        case ast_operator_type::AST_OP_SUB:
            return visitSubNode(node);
        // 新增节点类型处理
        case ast_operator_type::AST_OP_MUL:
            return visitMulNode(node);
        case ast_operator_type::AST_OP_DIV:
            return visitDivNode(node);
        case ast_operator_type::AST_OP_MOD:
            return visitModNode(node);
        case ast_operator_type::AST_OP_NEG:
            return visitNegNode(node);
        // 其他节点类型...
    }
    return false;
}

bool IRGenerator::visitMulNode(ast_node* node) {
    // 访问左右子节点，生成操作数
    if (!visit(node->sons[0]) || !visit(node->sons[1])) {
        return false;
    }
    
    // 获取操作数
    Value* left = node->sons[0]->val;
    Value* right = node->sons[1]->val;
    
    // 创建乘法指令
    Function* currentFunc = module->getCurrentFunction();
    BinaryInstruction* mulInst = new BinaryInstruction(
        currentFunc, IRInstOperator::IRINST_OP_MUL_I, left, right, IntegerType::getType()
    );
    
    // 将指令添加到当前函数
    currentFunc->addInstruction(mulInst);
    
    // 设置节点的值为指令结果
    node->val = mulInst;
    
    return true;
}

// 类似地实现visitDivNode和visitModNode

bool IRGenerator::visitNegNode(ast_node* node) {
    // 访问子节点，生成操作数
    if (!visit(node->sons[0])) {
        return false;
    }
    
    // 获取操作数
    Value* operand = node->sons[0]->val;
    
    // 创建求负指令
    Function* currentFunc = module->getCurrentFunction();
    UnaryInstruction* negInst = new UnaryInstruction(
        currentFunc, IRInstOperator::IRINST_OP_NEG_I, operand, IntegerType::getType()
    );
    
    // 将指令添加到当前函数
    currentFunc->addInstruction(negInst);
    
    // 设置节点的值为指令结果
    node->val = negInst;
    
    return true;
}
```

### 后端代码生成扩展

最后，扩展ARM32后端，使其能够处理新增的IR指令.

在`InstSelectorArm32`类中，添加新指令的处理函数：

```cpp
// 新增指令映射
    translator_handlers[IRInstOperator::IRINST_OP_MUL_I] = &InstSelectorArm32::translate_mul_int32;
    translator_handlers[IRInstOperator::IRINST_OP_DIV_I] = &InstSelectorArm32::translate_div_int32;
    translator_handlers[IRInstOperator::IRINST_OP_MOD_I] = &InstSelectorArm32::translate_mod_int32;
    translator_handlers[IRInstOperator::IRINST_OP_NEG_I] = &InstSelectorArm32::translate_neg_int32;
}
```

然后，实现以下处理函数：

```cpp
/// @brief 整数乘法指令翻译成ARM32汇编，直接使用mul
/// @param inst IR指令
void InstSelectorArm32::translate_mul_int32(Instruction * inst)
{
    translate_two_operator(inst, "mul");
}

/// @brief 整数除法指令翻译成ARM32汇编，直接使用sdiv
/// @param inst IR指令
void InstSelectorArm32::translate_div_int32(Instruction * inst)
{
    translate_two_operator(inst, "sdiv");
}

/// @brief 整数求余指令翻译成ARM32汇编，使用mul结合sdiv
/// @param inst IR指令
void InstSelectorArm32::translate_mod_int32(Instruction * inst)
{
    Value * result = inst;
    Value * arg1 = inst->getOperand(0);
    Value * arg2 = inst->getOperand(1);

    int32_t arg1_reg_no = arg1->getRegId();
    int32_t arg2_reg_no = arg2->getRegId();
    int32_t result_reg_no = inst->getRegId();
    int32_t load_result_reg_no, load_arg1_reg_no, load_arg2_reg_no;
    int32_t temp_reg_no;

    // 看arg1是否是寄存器，若是则寄存器寻址，否则要load变量到寄存器中
    if (arg1_reg_no == -1) {
        // 分配一个寄存器r8
        load_arg1_reg_no = simpleRegisterAllocator.Allocate(arg1);
        // arg1 -> r8
        iloc.load_var(load_arg1_reg_no, arg1);
    } else {
        load_arg1_reg_no = arg1_reg_no;
    }

    // 看arg2是否是寄存器，若是则寄存器寻址，否则要load变量到寄存器中
    if (arg2_reg_no == -1) {
        // 分配一个寄存器r9
        load_arg2_reg_no = simpleRegisterAllocator.Allocate(arg2);
        // arg2 -> r9
        iloc.load_var(load_arg2_reg_no, arg2);
    } else {
        load_arg2_reg_no = arg2_reg_no;
    }

    // 看结果变量是否是寄存器，若不是则需要分配一个新的寄存器来保存运算的结果
    if (result_reg_no == -1) {
        // 分配一个寄存器r10，用于暂存结果
        load_result_reg_no = simpleRegisterAllocator.Allocate(result);
    } else {
        load_result_reg_no = result_reg_no;
    }
    
    // 分配一个临时寄存器用于存储除法结果
    temp_reg_no = simpleRegisterAllocator.Allocate();
    
    // 先进行除法运算，结果存储在临时寄存器中
    iloc.inst("sdiv", 
              PlatformArm32::regName[temp_reg_no], 
              PlatformArm32::regName[load_arg1_reg_no], 
              PlatformArm32::regName[load_arg2_reg_no]);
    
    // 计算余数：余数 = 被除数 - (商 * 除数)
    // 先计算商 * 除数
    iloc.inst("mul", 
              PlatformArm32::regName[temp_reg_no], 
              PlatformArm32::regName[temp_reg_no], 
              PlatformArm32::regName[load_arg2_reg_no]);
    
    // 然后用被除数减去(商 * 除数)得到余数
    iloc.inst("sub", 
              PlatformArm32::regName[load_result_reg_no], 
              PlatformArm32::regName[load_arg1_reg_no], 
              PlatformArm32::regName[temp_reg_no]);

    // 结果不是寄存器，则需要把rs_reg_name保存到结果变量中
    if (result_reg_no == -1) {
        // r10 -> result
        iloc.store_var(load_result_reg_no, result, ARM32_TMP_REG_NO);
    }

    // 释放寄存器
    simpleRegisterAllocator.free(temp_reg_no);
    simpleRegisterAllocator.free(arg1);
    simpleRegisterAllocator.free(arg2);
    simpleRegisterAllocator.free(result);
}

/// @brief 整数求负指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_neg_int32(Instruction * inst)
{
    Value * result = inst;
    Value * arg = inst->getOperand(1);  // 操作数在第二个位置，第一个是常量0

    int32_t arg_reg_no = arg->getRegId();
    int32_t result_reg_no = inst->getRegId();
    int32_t load_arg_reg_no, load_result_reg_no;

    // 看arg是否是寄存器，若是则寄存器寻址，否则要load变量到寄存器中
    if (arg_reg_no == -1) {
        // 分配一个寄存器r8
        load_arg_reg_no = simpleRegisterAllocator.Allocate(arg);
        // arg -> r8
        iloc.load_var(load_arg_reg_no, arg);
    } else {
        load_arg_reg_no = arg_reg_no;
    }

    // 看结果变量是否是寄存器，若不是则需要分配一个新的寄存器来保存运算的结果
    if (result_reg_no == -1) {
        // 分配一个寄存器r9，用于暂存结果
        load_result_reg_no = simpleRegisterAllocator.Allocate(result);
    } else {
        load_result_reg_no = result_reg_no;
    }

    // 使用rsb指令实现求负：rsb rd, rn, #0 等价于 rd = 0 - rn
    iloc.inst("rsb", 
              PlatformArm32::regName[load_result_reg_no], 
              PlatformArm32::regName[load_arg_reg_no], 
              "#0");

    // 结果不是寄存器，则需要把结果保存到结果变量中
    if (result_reg_no == -1) {
        // r9 -> result
        iloc.store_var(load_result_reg_no, result, ARM32_TMP_REG_NO);
    }

    // 释放寄存器
    simpleRegisterAllocator.free(arg);
    simpleRegisterAllocator.free(result);
}
```



## 测试

使用本地编写的test-expr.c与test-comment.c以及平台测例进行测试

test-expr.c:

```c
int main()
{
    int a, b, c, d, e, f, g, result;
    
    a = 10;
    b = 010;
    c = 0x10;
    
    d = a * b;
    putint(d);
    putint(10);
    
    e = c / b;
    putint(e);
    putint(10);
    
    f = a % b;
    putint(f);
    putint(10);
    
    g = -a;
    putint(g);
    putint(10);
    
    result = a * b + c / b - f * e + -g;
    putint(result);
    putint(10);
    
    return result;
} 
```

test-comment.c

```c
// 测试注释处理与新增运算符
// 这是一个单行注释

/* 
 * 这是一个多行注释
 * 包含多行内容
 */

int main() {
    // 测试八进制和十六进制数字
    int a, b, c, d, e, f, result;
    a = 0123;    // 八进制数 = 83
    b = 0x4F;    // 十六进制数 = 79
    
    /* 测试新增运算符 */
    c = a * b;   // 乘法运算
    d = c / 10;  // 除法运算
    e = c % 10;  // 求余运算
    f = -e;      // 单目求负
    
    // 复杂表达式测试
    result = a + b * c - d / e % f + (-3) * (a + b);
    
    return result;
} 
```



### 对输出的线性IR(DragonIR格式)通过IR运行器进行功能测试验证

平台评测结果如图

![image-20250510002011606](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510002011606.png)

因为本机为Arm64架构，无法使用IRcompiler直接运行，因而在本地通过直接查看IR来观察是否有明显错误：

对于我本地编写的测例test-expr.c，第一次输出IR时新增的运算符全为unknown:

<img src="https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510232214772.png" alt="image-20250510232214772" style="zoom:50%;" />

这是因为虽然修改了前端词法和语法分析器，忘记在但IRgenerator里处理这些新增的指令。

增加以下case，重新make后解决此问题：

```cpp
        case IRInstOperator::IRINST_OP_MUL_I:

            // 乘法指令，二元运算
            str = getIRName() + " = mul " + src1->getIRName() + "," + src2->getIRName();
            break;
        case IRInstOperator::IRINST_OP_DIV_I:

            // 除法指令，二元运算
            str = getIRName() + " = div " + src1->getIRName() + "," + src2->getIRName();
            break;
        case IRInstOperator::IRINST_OP_MOD_I:

            // 求余指令，二元运算
            str = getIRName() + " = mod " + src1->getIRName() + "," + src2->getIRName();
            break;
```

IR里成功翻译出新运算：

```
%t9 = mul %l1,%l2
%t10 = div %l3,%l2
%t11 = mod %l1,%l2
%t12 = sub 0,%l1
```



针对递归下降识别注释的TODO的测试：

以下为生成的IR：

<img src="https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510232741496.png" alt="image-20250510232741496" style="zoom:50%;" />

如图，注释内容被忽略，且与另外两种方法所得的IR完全一致



### 对输出的ARM32汇编通过交叉编译与qemu运行进行功能测试验证

平台评测结果如图

![image-20250510002011606](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510002011606.png)

本地针对test-expr.c，因为其内包括所有新增的运算，故不再使用其他测例：

分别用gcc和minic编译的arm32生成可执行程序

![image-20250510233245128](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510233245128.png)

![image-20250510233208821](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510233208821.png)

分别运行并比较结果：

![image-20250510233309833](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510233309833.png)

查看返回值：

![image-20250510233328298](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510233328298.png)

- 两个程序的输出完全一致：8010210210-10108810

- 两个程序的返回值也一致：88

因此，后端生成汇编代码的功能已成功实现。



以下是三种前端技术分别进行IR和arm32评测的综合图：

![image-20250510113121424](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510113121424.png)



### 对输出的AST语法树进行测试验证

因为test-expr.c较为复杂，生成的AST语法树图太大不方便展示

![image-20250510233702942](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/image-20250510233702942.png)

使用test-comment.c，因为只在递归下降法里实现了对注释的识别，因此这里指展示递归下降法的AST图：

![test-comment-rd](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/test-comment-rd.png)

图中新增的运算符显示为unknown，检查后发现忘在Graph.cpp里添加新增运算符的处理，添加以下代码：

```cpp
        case ast_operator_type::AST_OP_MUL:
            nodeName = "*";
            break;
        case ast_operator_type::AST_OP_DIV:
            nodeName = "/";
            break;
        case ast_operator_type::AST_OP_MOD:
            nodeName = "%";
            break;
        case ast_operator_type::AST_OP_NEG:
            nodeName = "neg";
            break;
```

解决此问题：

![test-comment-rd2](https://ob-and-pic.oss-cn-beijing.aliyuncs.com/img/test-comment-rd2.png)

观察语法树，可以发现核心计算`result = a + b * c - d / e % f + (-3) * (a + b);`完全与test-comment.c里的逻辑相对应，说明已成功实现

