// Deanne Pamela Antonio
// ciLisp Header
//

#ifndef __cilisp_h_
#define __cilisp_h_
#define BUFFER_DOUBLE 0.000001
#define CHAR_BUFFER 128

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

typedef enum oper {
    NEG_OPER, // 0
    ABS_OPER,
    EXP_OPER,
    SQRT_OPER,
    ADD_OPER,
    SUB_OPER,
    MULT_OPER,
    DIV_OPER,
    REMAINDER_OPER,
    LOG_OPER,
    POW_OPER,
    MAX_OPER,
    MIN_OPER,
    EXP2_OPER,
    CBRT_OPER,
    HYPOT_OPER,
    READ_OPER,
    RAND_OPER,
    PRINT_OPER,
    EQUAL_OPER,
    LESS_OPER,
    GREATER_OPER,
    CUSTOM_OPER =255
} OPER_TYPE;

OPER_TYPE resolveFunc(char *);


typedef enum {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    SYMBOL_NODE_TYPE,
    COND_NODE_TYPE
} AST_NODE_TYPE;


typedef enum {
    INT_TYPE,
    DOUBLE_TYPE,
    NO_TYPE
} NUM_TYPE;

typedef enum {
    VARIABLE_TYPE,
    LAMBDA_TYPE
} SYMBOL_TYPE;

NUM_TYPE resolveNum(char *);

typedef struct {
    NUM_TYPE type;
    union{
        double dval;
        long ival;
    } value;
} NUM_AST_NODE;

typedef NUM_AST_NODE RET_VAL;

typedef struct {
    OPER_TYPE oper;
    char* ident; 
    struct ast_node *opList;
} FUNC_AST_NODE;

typedef struct symbol_table_node {
    SYMBOL_TYPE sym_type;
    NUM_TYPE val_type;
    char *ident;
    struct ast_node *val;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

typedef struct symbol_ast_node {
    char *ident;
} SYMBOL_AST_NODE;

typedef struct {
    struct ast_node *condNode;
    struct ast_node *nonZero; 
    struct ast_node *zero; 
} COND_AST_NODE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE *symbolTable;
    struct arg_table_node *argTable;
    struct ast_node *parent;
    union {
        NUM_AST_NODE number;
        FUNC_AST_NODE function;
        COND_AST_NODE condition;
        SYMBOL_AST_NODE symbol;
    } data;
    struct ast_node *next;
} AST_NODE;

AST_NODE *newNode(AST_NODE_TYPE type);

typedef struct stack_node {
    RET_VAL val;
    struct stack_node *next;
} STACK_NODE;

typedef struct arg_table_node {
    char *ident;
    RET_VAL argVal;
    struct arg_table_node *next;
} ARG_TABLE_NODE;

AST_NODE *createNumberNode(double value, NUM_TYPE type);

AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1);

AST_NODE *createSymbolNode(char *ident);

AST_NODE *sExprLink(AST_NODE *newNode, AST_NODE *head);

AST_NODE *astLink(SYMBOL_TABLE_NODE *letList, AST_NODE *op);

SYMBOL_TABLE_NODE *createSymbolTableNode(AST_NODE *val, char *type, char *ident);

SYMBOL_TABLE_NODE *letLink(SYMBOL_TABLE_NODE *head, SYMBOL_TABLE_NODE *newVal);

AST_NODE *createCondition(AST_NODE *condExpr, AST_NODE *nonZero, AST_NODE *zero);

ARG_TABLE_NODE *createArgTable(char *headName, ARG_TABLE_NODE *list);

SYMBOL_TABLE_NODE *createLambdaSymbolTableNode(AST_NODE *val, char *type, char *ident, ARG_TABLE_NODE *argList);

void freeNode(AST_NODE *node);

RET_VAL eval(AST_NODE *node);
RET_VAL evalNumNode(NUM_AST_NODE *numNode);
RET_VAL evalFuncNode(AST_NODE *node);
RET_VAL evalSymbolNode(AST_NODE *symbolNode);
RET_VAL evalSymbolNodeHelper(SYMBOL_TABLE_NODE *symbol);
RET_VAL evalCondNode(COND_AST_NODE *condAstNode);
void printRetVal(RET_VAL val);
STACK_NODE *createStackNodes(AST_NODE *lambdaFunc, AST_NODE *paramList);
void attachStackNodes(ARG_TABLE_NODE *lambdaArgs, STACK_NODE *paramVal);

#endif