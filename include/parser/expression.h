//
// Created by Sam on 2018/1/17.
//

#ifndef DONGMENDB_EXPRESSION_H
#define DONGMENDB_EXPRESSION_H

#include <dongmensql/literal.h>
#include <utils/arraylist.h>
#include <dongmensql/column.h>
#include "tokenizer.h"

/**
 * 支持条件表达式和算术表达式
 *
 */
typedef enum {
    TERM_UNKNOWN = 0,
    TERM_LITERAL,
    TERM_ID,
    TERM_NULL,
    TERM_COLREF,
    TERM_FUNC
} TermType;

typedef enum {
    FUNC_MAX,
    FUNC_MIN,
    FUNC_COUNT,
    FUNC_AVG,
    FUNC_SUM
} FuncType;

typedef enum {
    left2right,
    right2left
} associativity;

typedef struct {
    int numbers;        // 操作数
    int icp;            // 优先级
    int isp;            // 优先级
    associativity ass;  // 结合性
    TokenType oper; // 操作符
} OPERATOR;

// [操作数，入栈优先级，出栈优先级，结合性，运算符编号]
static const OPERATOR operators[] = {
        /* 算数运算 */
        {2, 18, 1,  left2right, TOKEN_OPEN_PAREN},     // 左括号
        {2, 18, 18, left2right, TOKEN_CLOSE_PAREN},    // 右括号
        {2, 12, 12, left2right, TOKEN_PLUS},      // 加
        {2, 12, 12, left2right, TOKEN_MINUS},     // 减
        {2, 13, 13, left2right, TOKEN_MULTIPLY},  // 乘
        {2, 13, 13, left2right, TOKEN_DIVIDE},    // 除
        /* 关系运算 */
        {2, 10, 10, left2right, TOKEN_LT},        // 小于
        {2, 10, 10, left2right, TOKEN_GT},        // 大于
        {2, 10, 9,  left2right, TOKEN_EQ},          // 等于
        {2, 10, 9,  left2right, TOKEN_NOT_EQUAL},          // 不等于
        {2, 10, 10, left2right, TOKEN_LE},        // 不大于
        {2, 10, 10, left2right, TOKEN_GE},        // 不小于
        {2, 10, 9,  left2right, TOKEN_IN},
        {2, 10, 9,  left2right, TOKEN_LIKE},
        /* 逻辑运算 */
        {2, 5,  5,  left2right, TOKEN_AND},         // 且
        {2, 5,  4,  left2right, TOKEN_OR},          // 或
        {1, 15, 15, right2left, TOKEN_NOT},       // 非
        /* 赋值 */
        {2, 2,  2,  right2left, TOKEN_ASSIGNMENT},  // 赋值
        /* 最小优先级 */
        {2, 17, 17, right2left, TOKEN_FUN},        //把函数解析为一个操作符
        {2, 1,  1,  right2left, TOKEN_COMMA}
};

typedef struct Expression_ Expression;
typedef struct Func {
    FuncType t;
    Expression *expr;
} Func;

typedef struct ColumnReference_s ColumnReference_t;
/*终结符：标识符，常量*/
typedef struct TermExpr_ {
    TermType t;
    union {
        char *id;
        Literal_t *val;
        ColumnReference_t *ref;
        Func f;
    };
} TermExpr;

typedef struct Expression_ {
    /*当term不为空时，表示是term*/
    TokenType opType;
    TermExpr *term;
    /*如果出现在select子句中，则可以有别名*/
    char *alias;
    /*按链表顺序存操作数*/
    Expression *nextexpr;

} Expression;

int printTermExpression(char *exprs, TermExpr *term);

const char *getOperatorDesc(TokenType type);

Expression *newExpression(TokenType type, Expression *nextexpr);

TermExpr *newTermExpr();

int expression_free(Expression *expr);

int expression_free_list(arraylist *exprlist);

Expression *expression_print(Expression *expr, char *desc);

int expression_print_list(arraylist *exprlist);

#endif //DONGMENDB_EXPRESSION_H