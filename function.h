//
//  function.h
//  编译技术实验一_3
//
//  Created by 抠鼻小新 on 2018/10/16.
//  Copyright © 2018年 抠鼻小新. All rights reserved.
//


/*****助记符的定义*****/
#define T_SYMBOL 1
#define T_CONSTANT 2
#define T_INT 3
#define T_IF 4
#define T_ELSE 5
#define T_WHILE 6
#define T_FOR 7
#define T_READ 8
#define T_WRITE 9
#define T_ADD 10
#define T_SUB 11
#define T_MUL 12
#define T_DIV 13
#define T_L 14
#define T_LE 15
#define T_G 16
#define T_GE 17
#define T_NE 18
#define T_E 19
#define T_ASSIGN 20
#define T_LPAR 21
#define T_RPAR 22
#define T_COM 23
#define T_SEM 24
#define T_ANNOTATOR 27

#define T_EOLN        25
#define T_EOF        26

#define MAX_TAB_SIZE     50        //表的最大容量
#define MAX_BIT      128        //常量的最大位数
#define MAX_ID_LENGTH     128        //标识符的最长长度

// 符号表存储类型定义
typedef struct symTableItem{
    char name[MAX_ID_LENGTH];
    int  type;
    int  ln;
} symTableItem;

// 常量表存储类型定义
typedef struct numTableItem {
    int *binary;    //二进制（数组）
    int bit;        //二进制位数
} numTableItem;

// 保留字表
extern char *reserve_table[9];
extern int reserve_size;

// 符号表，不规则数组
extern symTableItem symbol_table[MAX_TAB_SIZE];
extern int symbol_size;

// 常量表
extern numTableItem constant_table[MAX_TAB_SIZE];
extern int constant_size;

extern int line;    //记录行号

// 词法分析所用到的函数声明
char getnbc(FILE *sfp);
void concat(char character, char token[], int *n);
int digit(char character);
int retract(char character, char *back);
int reserve(char token[]);
int symbol(char token[]);
int constant(char token[], int n);
void error1(FILE *fp4, char character, int line);
int analyze(FILE *fp1, FILE *fp2, FILE * fp3, FILE *fp4, int *back_flag, char *back);


