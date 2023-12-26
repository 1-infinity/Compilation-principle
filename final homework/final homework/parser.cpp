#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include "lexical_analyzer.h"
using namespace std;

enum Elem {
    ERROR,      //错误
    P=1,        //程序
    SP,         //子程序
    PHEAD,      //程序首部
    IDFS,       //标识符
    C,          //常量说明
    V,          //变量说明
    ST,         //语句
    CDF,        //常量定义
    UINT,       //无符号整数
    N,          //数字
    VDF,        //变量说明
    L,          //字母
    CPLXST,     //复合语句
    ASNST,      //赋值语句
    CONDST,     //条件语句
    LOOP,       //循环语句
    NULLST,     //空语句
    EXP,        //表达式
    T,          //术语
    AOP,        //加法运算符
    F,          //因子
    MOP,        //乘法运算符
    COND,       //条件
    ROP         //关系运算符
};

typedef struct {
    int address;
    string op;
    string op1;
    string op2;
    string op3;
} Code;

class parser {
private:
    Code MidCode[1000];
    int init = 100;
    Lexer lexer;
public:
    
};

string getToken()
{
    // To do
    return "Hello, world";
}

// N->a|b|...|z
bool N()
{
    string token = getToken();
    if(token.length()==1 && token[0]>='a' && token[0]<='z'){

    }
    return true;
}