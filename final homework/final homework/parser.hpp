#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include "lexical_analyzer.h"
using namespace std;

Token token;

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

typedef struct {
    string name;
    string prop;
    string Register;
} var;

Lexer lexer("text.txt");

class Parser {
private:
    Code MidCode[1000];
    var Vars[100];
    int init;
    int ptr;

    void P();
    void PHEAD();
    void SP();
    void C(bool firstCheck=false);// 如果看了FISRT，则第一个token已经被读取，此函数不再读取
    void CDF();
    void UINT();
    void V(bool firstCheck=false);// 同上
    void IDFS();
    void CPLXST(bool firstCheck=false);
    void ST();
    void ASNST();
    void EXP();
    void _EXP();// 消除左递归
    void T();
    void _T();// 消除左递归
    void F();
    void AOP();
    void MOP();
    void CONDST(bool firstCheck=false);
    void LOOP(bool firstCheck=false);
    void COND();
    void ROP();
    void NULLST(bool firstCheck=false);
public:
    Parser(); 
    void ParserAndCodeGenerator();
};

void Parser::ParserAndCodeGenerator(){}

Parser::Parser() {
    init = 100;
    ptr = 0;
}

// P->PHEAD SP
void Parser::P(){
    PHEAD();
    SP();
}

// PHEAD -> 'PROGRAM' IDFS
void Parser::PHEAD(){
    token = lexer.getNextToken();
    if("PROGRAM" != token.type){
        cerr<<"Program doesn't start with \'PROGRAM\'!"<<endl;
        exit(1);
    }
    IDFS();
}

// SP-> [C][V]ST
void Parser::SP(){
    token = lexer.getNextToken();
    if("CONST" == token.type){
        C(true);
        token = lexer.getNextToken();
        if("VAR" == token.type) {
            V(true);
        }
    }
    else if("VAR" == token.type){
        V(true);
    }
    // sentence
    ST();
}

// C-> 'CONST'CDF{,CDF};
void Parser::C(bool firstCheck=false) {
    if(firstCheck == false){
        token = lexer.getNextToken();
        if(token.type != "CONST"){
            cerr << "\'CONST\' symbol missing"<<endl;
            exit(1);
        }
    }

    while(true) {
        CDF();
        token=lexer.getNextToken();
        if(token.type == ",")
            continue;
        else if(token.type == ";")
            break;
        else{
            cerr<<"Unrecoginzed symbol when initilizing const values"<<endl;
        }
    }
}

// CDF->IDFS:=UINT
void Parser::CDF(){
    IDFS();
    token = lexer.getNextToken();
    if(token.type != ":="){
        cerr<<"Tempt to initilizing const values with error identifier"<<endl;
        exit(1);
    }
    UINT();
}

// UINT->N{N}
void Parser::UINT(){
    token = lexer.getNextToken();
    if(token.type!="INT"){
        cerr<<"Using error type"<<endl;
        exit(1);
    }
}

// V->'VAR'IDFS{,IDFS};
void Parser::V(bool firstCheck=false){
    if(firstCheck == false){
        token = lexer.getNextToken();
        if(token.type != "VAR"){
            cerr << "\'VAR\' symbol missing"<<endl;
            exit(1);
        }
    }

    while(true) {
        IDFS();
        token=lexer.getNextToken();
        if(token.type == ",")
            continue;
        else if(token.type == ";")
            break;
        else{
            cerr<<"Unrecoginzed symbol when initilizing vars"<<endl;
        }
    }
}

// IDFS —> L{L|D}
void Parser::IDFS(){
    token = lexer.getNextToken();
    if(token.type!="ID"){
        cerr<<"Identifier format wrong"<<endl;
        exit(1);
    }
}

// CPLXST—>'BEGIN' ST{;ST}'END'
void Parser::CPLXST(bool firstCheck=false){
    if(firstCheck == false){
        token=lexer.getNextToken();
        if(token.type!="BEGIN"){
            cerr<<"Missing \'BEGIN\' with complex sentence"<<endl;
            exit(1);
        }
    }
    while(true){
        ST();
        token=lexer.getNextToken();
        if(";" == token.type)
            continue;
        else if("END" == token.type)
            break;
        else{
            cerr<<"sentence ends with wrong codes"<<endl;
            exit(1);
        }
    }
}

// ST—>ASNST|CONDST|LOOP|CPLXST|NULLST
void Parser::ST(){
    token = lexer.getNextToken();
    if("IF" == token.type){
        CONDST(true);
    }
    else if("WHILE" == token.type){
        LOOP(true);
    }
    else if("BEGIN"== token.type){
        CPLXST(true);
    }
    else if(";" == token.type){
        NULLST(true);
    }
    else{
        ASNST();
    }
}

// ASNST—>IDFS:=EXP
void Parser::ASNST(){
    token = lexer.getNextToken();
    if(token.type!="ID"){
        cerr<<"Assignment sentence wrong: not using identifier"<<endl;
        exit(1);
    }
    token = lexer.getNextToken();
    if(":=" != token.type){
        cerr<<"Assignment sentence wrong: not using \':=\'"<<endl;
        exit(1);
    }
    EXP();
}

// <EXP>—[+|-]T|EXP' MOP T
void Parser::EXP(){
    token = lexer.getNextToken();
    if(token.type != "+" && token.type != "-"){
        cerr<<"Wrong expression: start without +/-"<<endl;
        exit(1);
    }
    T();
    _EXP();
}

// EXP'->MOP T EXP' | <NULL>
void Parser::_EXP(){
    token = lexer.getPeekChar();
    if(";"==token.type){
        return;
    }
    MOP();
    T();
    _EXP();
}

// T—>FT'
void Parser::T(){
    F();
    _T();
}

// T'->MOP F T' | <NULL>
void Parser::_T(){
    token = lexer.getPeekChar();
    if(";" == token.type){
        return;
    }
    MOP();
    F();
    _T();
}

// F—>IDFS|UINT|(EXP)
void Parser::F(){
    token = lexer.getNextToken();
    if("ID" == token.type){
        return;
    }
    else if("INT" == token.type){
        return;
    }
    else if("(" == token.type){
        EXP();
        token = lexer.getNextToken();
        if("(" != token.type){
            cerr<<"curves unmatched"<<endl;
        }
    }
    else{
        cerr<<"Wrong Factor"<<endl;
    }
}

// AOP—>+|-
void Parser::AOP(){
    token = lexer.getNextToken();
    if("+"!=token.type && "-"!=token.type){
        cerr<<"wrong add and substract operator"<<endl;
        exit(1);
    }
}

// MOP->*|/
void Parser::MOP(){
    token = lexer.getNextToken();
    if("*"!=token.type && "/"!=token.type){
        cerr<<"wrong add and substract operator"<<endl;
        exit(1);
    }
}

void Parser::CONDST(bool firstCheck=false){
    if(firstCheck==false){
        token = lexer.getNextToken();
        if("IF"!=token.type){
            cerr<<"error condition sentence"<<endl;
            exit(1);
        }
    }
    COND();
    token = lexer.getNextToken();
    if("THEN"!=token.type){
        cerr<<"error condition sentence"<<endl;
        exit(1);
    }
    ST();
    
}
void Parser::LOOP(bool firstCheck=false){
    if(firstCheck==false){
        token = lexer.getNextToken();
        if("WHILE"!=token.type){
            cerr<<"error loop sentence"<<endl;
            exit(1);
        }
    }
    COND();
    token = lexer.getNextToken();
    if("DO"!=token.type){
        cerr<<"error loop sentence"<<endl;
        exit(1);
    }
    ST();
}
void Parser::COND(){
    EXP();
    ROP();
    EXP();
}
void Parser::ROP(){
    token = lexer.getNextToken();
    if("ROP"!=token.type){
        cerr<<"error condition operator"<<endl;
        exit(1);
    }
}
void Parser::NULLST(bool firstCheck=false)
{
    if(firstCheck==false){
        token = lexer.getPeekChar();
        if(";"!=token.type){
            cerr<<"not a null sentence"<<endl;
            exit(1);
        }
    }
}