///
/// @file RecursiveDescentFlex.cpp
/// @brief 词法分析的手动实现源文件
/// @author zenglj (zenglj@live.com)
/// @version 1.1
/// @date 2024-11-23
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-21 <td>1.0     <td>zenglj  <td>新做
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>表达式版增强
/// </table>
///
#include <cctype>
#include <cstdio>
#include <cstring>
#include <string>

#include "RecursiveDescentParser.h"
#include "Common.h"

/// @brief 词法分析的行号信息
int64_t rd_line_no = 1;

/// @brief 词法分析的token对应的字符识别
std::string tokenValue;

/// @brief 输入源文件指针
FILE * rd_filein;

/// @brief 关键字与Token类别的数据结构
struct KeywordToken {
    std::string name;
    enum RDTokenType type;
};

/// @brief  关键字与Token对应表
static KeywordToken allKeywords[] = {
    {"int", RDTokenType::T_INT},
    {"return", RDTokenType::T_RETURN},
};

/// @brief 在标识符中检查是否时关键字，若是关键字则返回对应关键字的Token，否则返回T_ID
/// @param id 标识符
/// @return Token
static RDTokenType getKeywordToken(std::string id)
{
    //如果在allkeywords中找到，则说明为关键字
    for (auto & keyword: allKeywords) {
        if (keyword.name == id) {
            return keyword.type;
        }
    }
    // 如果不再allkeywords中，说明是标识符
    return RDTokenType::T_ID;
}

/// @brief 词法文法，获取下一个Token
/// @return  Token，值保存在rd_lval中
int rd_flex()
{
    int c;              // 扫描的字符
    int tokenKind = -1; // Token的值

    // 忽略空白符号，主要有空格，TAB键和换行符
    while ((c = fgetc(rd_filein)) == ' ' || c == '\t' || c == '\n' || c == '\r') {

        // 支持Linux/Windows/Mac系统的行号分析
        // Windows：\r\n
        // Mac: \n
        // Unix(Linux): \r
        if (c == '\r') {
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

    // 文件结束符
    if (c == EOF) {
        // 返回文件结束符
        return RDTokenType::T_EOF;
    }

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

    // 处理数字
    if (isdigit(c)) {

        // 识别无符号数
        rd_lval.integer_num.lineno = rd_line_no;
        
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
        // 处理十进制数字 (1-9开头)
        else {
            rd_lval.integer_num.val = c - '0';
            
            // 最长匹配，直到非数字结束
            while (isdigit(c = fgetc(rd_filein))) {
                rd_lval.integer_num.val = rd_lval.integer_num.val * 10 + c - '0';
            }
        }

        // 存储数字的token值
        tokenValue = std::to_string(rd_lval.integer_num.val);

        // 多读的字符回退
        ungetc(c, rd_filein);

        tokenKind = RDTokenType::T_DIGIT;
    } else if (c == '(') {
        // 识别字符(
        tokenKind = RDTokenType::T_L_PAREN;
        // 存储字符(
        tokenValue = "(";
    } else if (c == ')') {
        // 识别字符)
        tokenKind = RDTokenType::T_R_PAREN;
        // 存储字符)
        tokenValue = ")";
    } else if (c == '{') {
        // 识别字符{
        tokenKind = RDTokenType::T_L_BRACE;
        // 存储字符{
        tokenValue = "{";
    } else if (c == '}') {
        // 识别字符}
        tokenKind = RDTokenType::T_R_BRACE;
        // 存储字符}
        tokenValue = "}";
    } else if (c == ';') {
        // 识别字符;
        tokenKind = RDTokenType::T_SEMICOLON;
        // 存储字符;
        tokenValue = ";";
    } else if (c == '+') {
        // 识别字符+
        tokenKind = RDTokenType::T_ADD;
		// 存储字符+
        tokenValue = "+";
    } else if (c == '-') {
        // 识别字符-
        tokenKind = RDTokenType::T_SUB;
		// 存储字符-
        tokenValue = "-";
    } else if (c == '*') {
        // 识别字符*
        tokenKind = RDTokenType::T_MUL;
		// 存储字符*
        tokenValue = "*";
    } else if (c == '%') {
        // 识别字符%
        tokenKind = RDTokenType::T_MOD;
		// 存储字符%
        tokenValue = "%";
    } else if (c == '=') {
        // 识别字符=
        tokenKind = RDTokenType::T_ASSIGN;
    }  else if (c == ',') {
        // 识别字符;
        tokenKind = RDTokenType::T_COMMA;
		// 存储字符,
        tokenValue = ",";
    } else if (isLetterUnderLine(c)) {
        // 识别标识符，包含关键字/保留字或自定义标识符

        // 最长匹配标识符
        std::string name;

        do {
            // 记录字符
            name.push_back(c);
            c = fgetc(rd_filein);
        } while (isLetterDigitalUnderLine(c));

        // 存储标识符
        tokenValue = name;

        // 多读的字符恢复，下次可继续读到该字符
        ungetc(c, rd_filein);

        // 检查是否是关键字，若是则返回对应的Token，否则返回T_ID
        tokenKind = getKeywordToken(name);
        if (tokenKind == RDTokenType::T_ID) {
            // 自定义标识符

            // 设置ID的值
            rd_lval.var_id.id = strdup(name.c_str());

            // 设置行号
            rd_lval.var_id.lineno = rd_line_no;
        } else if (tokenKind == RDTokenType::T_INT) {
            // int关键字

            // 设置类型与行号
            rd_lval.type.type = BasicType::TYPE_INT;
            rd_lval.type.lineno = rd_line_no;
        }
    } else {
        printf("Line(%lld): Invalid char %s\n", (long long) rd_line_no, tokenValue.c_str());
        tokenKind = RDTokenType::T_ERR;
    }

    // Token的类别
    return tokenKind;
}
