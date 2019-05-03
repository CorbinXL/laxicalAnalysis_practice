//
//  main.c
//  编译技术实验一_3
//
//  Created by 抠鼻小新 on 2018/10/16.
//  Copyright © 2018年 抠鼻小新. All rights reserved.
//



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "function.h"

numTableItem constant_table[MAX_TAB_SIZE];
int constant_size = 0;

symTableItem symbol_table[MAX_TAB_SIZE];
int symbol_size = 0;

bool new_sym = false;
int line = 1;
/*****************************************************************************
 * 函数名称： getnbc
 * 功能描述： 从文件中读入字符，检查变量是否为空白字符（包括TAB)，若是，则读入下一个字符，直到读到
 *            非空白字符为止
 ******************************************************************************/
char getnbc(FILE *fp1)
{
    char ch = fgetc(fp1);
    while (ch == ' ' || ch == '\t' || ch == '\r') ch = fgetc(fp1);    //略过空格和制表符
    return ch;
}

/*****************************************************************************
 * 函数名称： concat
 * 功能描述： 将character连接到token数组的末尾，然后再插入'\0'，并修改token的长度
 ******************************************************************************/
void concat(char character, char token[], int *n)
{
    token[*n] = character;
    (*n)++;
    token[*n] = '\0';
}

/*****************************************************************************
 * 函数名称： digit
 * 功能描述： 判断是否是数字，若是则返回1，否则返回0
 ******************************************************************************/
int digit(char character)
{
    return (character >= '0' && character <= '9');
}

/*****************************************************************************
 * 函数名： letter
 * 功能描述： 判断是否是数字，若是返回1，否则返回0
 ******************************************************************************/
int letter(char character)
{
    return ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'));
}

/*****************************************************************************
 * 函数名称： retract
 * 功能描述： 回退字符，若是非空白字符，将字符保存在back中；若是空白字符，不回退
 ******************************************************************************/
int retract(char character, char *back)
{
    if (character != ' ') {
        *back = character;
        return 1;
    }
    return 0;
}

/*****************************************************************************
 * 函数名称： constant
 * 功能描述： 对token中的字符串查常数表，若查到，返回该常数的序号，否则将新的常数插入
 表中，再返回序号
 ******************************************************************************/
int constant(char token[], int n)
{
    int i, j, b;
    int dec, rem, tmp;
    int stack[MAX_BIT], top = 0;
    
    // 转换成十进制数
    for (i = 0, dec = 0; i < n; i++) {
        dec = dec * 10 + (token[i] - '0');
    }
    
    // 转换成二进制数，倒序放在栈中
    rem = dec;
    if (dec == 0) stack[top++] = 0;    //“0”单独处理
    while (rem > 0) {
        stack[top++] = rem % 2;
        rem = rem / 2;
    }
    
    tmp = top;    //暂存栈顶指针
    for (i = 0; i < constant_size; i++) {
        b = constant_table[i].bit;    //取二进制数的位数
        if (top != b) continue;        //位数不相等，直接跳过该数
        
        for (j = 0; j < b; j++) {    //逐位比较
            if (stack[--top] != constant_table[i].binary[j]) break;
        }
        if (j >= b) return i;    //找到相同的数，直接返回该数在表中的位置
        top = tmp;    //还原top
    }
    
    // 如果没有找到相同的数，就将该数录入表中
    i = constant_size++;
    constant_table[i].binary = (int *)malloc((top + 1)* sizeof(int));
    constant_table[i].bit = top;    //首位存储该二进制数的位数
    for (j = 0; top > 0; j++) {
        constant_table[i].binary[j] = stack[--top];
    }
    return i;
}

int reserve(char token[])
{
    switch(token[0])
    {
        case 'i':
            if(token[1] == 'n' && token[2] == 't')
                return T_INT;
            else if(token[1] == 'f')
                return T_IF;
            else
                return 0;
            break;
            
        case 'e':
            if(token[1] == 'l' && token[2] == 's' && token[3] == 'e')
                return T_ELSE;
            else
                return 0;
            break;
            
        case 'w':
            if(token[1] == 'h' && token[2] == 'i' && token[3] == 'l' && token[4] == 'e')
                return T_WHILE;
            else if(token[1] == 'r' && token[2] == 'i' && token[3] == 't' && token[4] == 'e')
                return T_WRITE;
            else
                return 0;
            break;
            
        case 'f':
            if(token[1] == 'o' && token[2] == 'r')
                return T_FOR;
            else
                return 0;
            break;
            
        case 'r':
            if(token[1] == 'e' && token[2] == 'a' && token[3] == 'd')
                return T_READ;
            else
                return 0;
            break;
            
        default: return 0;
            
    }
    return 0;
}

int symbol(char token[])
{
    int i;
    if(symbol_size == 0)
    {
        new_sym = true;
        i = symbol_size++;
        symbol_table[0].type = 0;
        symbol_table[0].ln = line;
        for(int k = 0 ; token[k] != '\0' ; ++k)
        {
            symbol_table[0].name[k] = token[k];
        }
        return i;
    }
    
    for(int k = 0 ; k < symbol_size ; ++k)
    {
        if(strcmp(token, symbol_table[k].name) == 0)
        {
            i = symbol_table[k].type;
            return i;
        }
    }
    
    i = symbol_size++;
    new_sym = true;
    for(int k = 0 ; token[k] != '\0' ; ++k)
    {
        symbol_table[i].name[k] = token[k];
    }
    symbol_table[i].type = i;
    symbol_table[i].ln = line;
    return i;
}

/*****************************************************************************
 * 函数名称： error1
 * 功能描述： 打印出错的信息
 ******************************************************************************/
void error1(FILE *fp4, char character, int line)
{
    printf("~error: line %.2d: 含有非法字符:\t%c\n\n", line, character);
    fprintf(fp4, "~error: line %.2d: 含有非法字符:\t%c \n", line, character);
}

/*****************************************************************************
 * 函数名称： analyze
 * 功能描述： 主词法分析函数，每调用一次获取一个单词，将其打印到屏幕并输出到文本
 ******************************************************************************/
int analyze(FILE *fp1, FILE *fp2, FILE * fp3, FILE *fp4, int *back_flag, char *back)
{
    char Nuse[1001];
    char character;
    char token[MAX_ID_LENGTH];
    int  n = 0;        //n记录token中的字符串长度
    int  type;        //单词类型
    int  val = 0;    //单词属性，只有标识符和常数不是0
    
    if (*back_flag) {    //上一次有回退
        character = *back;
        *back_flag = 0;
    }
    else character = getnbc(fp1);
    
    if (digit(character)) {    //是常数
        while (digit(character)) {
            concat(character, token, &n);
            character = fgetc(fp1);
        }
        val = constant(token, n);
        type = T_CONSTANT;
        *back_flag = retract(character, back);
    }
    else if(letter(character))
    {
        while(letter(character) || digit(character))
        {
            concat(character, token, &n);
            character = fgetc(fp1);
        }
        * back_flag = retract(character, back);
        
        type = reserve(token);
        if(type != 0)
        val = 0;
        else
        {
            val = symbol(token);
            type =T_SYMBOL;
        }
    }
    else {
        switch (character) {
            case '+':    type = T_ADD;    concat(character, token, &n);    break;
            case '-':    type = T_SUB;    concat(character, token, &n);    break;
            case '*':    type = T_MUL;    concat(character, token, &n);    break;
            case '/':
                concat(character, token, &n);
                character = fgetc(fp1);
                if(character == '/')
                {
                    concat(character, token, &n);
                    type = T_ANNOTATOR;
                    val = 0;
                    break;
                }
                else
                {
                    *back = retract(character, back);
                    type = T_DIV;
                    val = 0;
                    break;
                }
            case '<':
                concat(character, token, &n);
                character = fgetc(fp1);
                if(character == '=')
                {
                    concat(character, token, &n);
                    type = T_LE;
                    val = 0;
                    break;
                }
                else
                {
                    *back = retract(character, back);
                    type = T_L;
                    val = 0;
                    break;
                }
            case '>':
                concat(character, token, &n);
                character = fgetc(fp1);
                if(character == '=')
                {
                    concat(character, token, &n);
                    type = T_GE;
                    val = 0;
                    break;
                }
                else
                {
                    *back = retract(character, back);
                    type = T_G;
                    val = 0;
                    break;
                }
            case '=':
                concat(character, token, &n);
                character = fgetc(fp1);
                if(character == '=')
                {
                    type = T_E;
                    val = 0;
                    concat(character, token, &n);
                    break;
                }
                else
                {
                    *back = retract(character, back);
                    type = T_ASSIGN;
                    val = 0;
                    break;
                }
            case '!':
                concat(character, token, &n);
                character = getnbc(fp1);
                if(character == '=')
                {
                    concat(character, token, &n);
                    type = T_NE;
                    val = 0;
                    break;
                }
                else
                {
                    *back = retract(character, back);
                    error1(fp4, character, line);
                    type = -1;
                    break;
                }
            case '{':
                concat(character, token, &n);
                type = T_LPAR;
                val = 0;
                break;
            case '}':
                concat(character, token, &n);
                type = T_RPAR;
                val = 0;
                break;
            case '[':
                concat(character, token, &n);
                type = T_LPAR;
                val = 0;
                break;
            case ']':
                concat(character, token, &n);
                type = T_RPAR;
                val = 0;
                break;
            case '(':
                concat(character, token, &n);
                type = T_LPAR;
                val = 0;
                break;
            case ')':
                concat(character, token, &n);
                type = T_RPAR;
                val = 0;
                break;
            case ',':
                concat(character, token, &n);
                type = T_COM;
                val = 0;
                break;
            case ';':
                concat(character, token, &n);
                type = T_SEM;
                val = 0;
                break;
            case EOF:    type = T_EOF;    concat('#', token, &n);            break;
            case '\n':    type = T_EOLN;    concat(character, token, &n);    break;
            default:    type = -1; error1(fp4, character, line);
                //return -1;
        }
    }
    
    // 在屏幕上打印并输出到文本
    if(type == T_INT || type == T_IF || type == T_ELSE || type == T_WHILE || type == T_FOR || type == T_READ || type == T_WRITE)
    {
        printf("line %.2d: (%.2d, %d) 保留字：%s\n", line, type, val, token);
        fprintf(fp2, "line %.2d: (%.2d, %d) RESERVE：%s\n", line, type, val, token);
    }
    if(type == T_SYMBOL)
    {
        printf("line %.2d: (%.2d, %d) 标识符: %s\n", line, type, val, token);
        if(new_sym)
        {
            fprintf(fp3, "line %.2d: (%.2d, %d) SYMBOL: %s\n", line, type, val, token);
            new_sym = false;
        }
    }
    if (type == T_CONSTANT) {
        printf("line %.2d: (%.2d, %d) 常  数：%s\n", line, type, val, token);
        fprintf(fp2, "line %.2d: (%.2d, %d) 常  数：%s\n", line, type, val, token);
    }
    else if ( type >= 10 && type <=20){
        printf("line %.2d: (%.2d, %d) 运算符：%s\n", line, type, val, token);
        fprintf(fp2, "line %.2d: (%.2d, %d) 运算符：%s\n", line, type, val, token);
    }
    else if ( type == T_EOLN){
        printf("line %.2d: (%.2d, %d) 行尾符：\\n\n", line, type, val);
        fprintf(fp2, "line %.2d: (%.2d, %d) 行尾符 \\n\n", line, type, val);
    }
    else if ( type >= 21 && type <=24){
        printf("line %.2d: (%.2d, %d) 界符：%s\n", line, type, val, token);
        fprintf(fp2, "line %.2d: (%.2d, %d) 界符：%s\n", line, type, val, token);
    }
    else if ( type == T_EOF){
        printf("line %.2d: (%.2d, %d) 结尾符：%s\n", line, type, val, token);
        fprintf(fp2, "line %.2d: (%.2d, %d) 结尾符：%s\n", line, type, val, token);
    }
    else if(type == T_ANNOTATOR)
    {
        printf("line %.2d: (%.2d, %d) 界符：%s\n", line, type, val, token);
        fprintf(fp2, "line %.2d: (%.2d, %d) 界符：%s\n", line, type, val, token);
        fgets(Nuse, 1000, fp1);
    }
    
    if (type == T_EOLN || type == T_ANNOTATOR) line++; //换行
    
    return type;
}

int main(void)
{
    char *soure = "sorting.txt";
    char *output = "output.txt";
    char *symbol = "symbol.txt";
    char *error = "error.txt";
    
    FILE *fp_src = fopen(soure, "r");
    if (fp_src == NULL) {
        printf("打开源文件失败！请确定源文件的位置！\n");
        return 0;
    }
    
    FILE *fp_output = fopen(output, "w");
    FILE *fp_err = fopen(error, "w");
    FILE *fp_sym = fopen(symbol, "w");
    
    int back_flag = 0;    //回退的标记，1代表上一次有回退(空格不算)，0代表没有
    char back;            //存放上一次的回退值
    
    printf("郑殿新-2017221103005\n\n");
    fprintf(fp_output, "郑殿新-2017221103005\n\n");
    fprintf(fp_sym, "郑殿新-2017221103005\n\n");
    fprintf(fp_err, "郑殿新-2017221103005\n\n");
 
    
    while (analyze(fp_src, fp_output, fp_sym, fp_err, &back_flag, &back) != T_EOF);
    
    fclose(fp_src);
    fclose(fp_output);
    fclose(fp_err);
    return 0;
}
