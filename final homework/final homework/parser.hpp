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
    string arg1;
    string arg2;
    string result;
} Code;

typedef struct {
    string name;
    string prop; // property
    string Register;
} var; // variable

// Lexer lexer("text.txt");

class Parser {
private:
    vector<Code> MidCode;
    var Vars[100];
    int init;
    int ptr;
    int temp_number;
    Lexer lexer;

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
    void ASNST(Token token);
    string EXP();
    void _EXP();// 消除左递归
    string T();
    string _T();// 消除左递归
    string F();
    string AOP();
    string MOP();
    void CONDST(bool firstCheck=false);
    void LOOP(bool firstCheck=false);
    void COND();
    string ROP();
    void NULLST(bool firstCheck=false);
    void M(){}

    string getTemp();// 获取T1, T2, ...
    int emit(string op, string arg1, string arg2, string result);// 负责写入一条中间代码，返回该代码索引
    int getPreCodePtr();
    int deletePreCode();
public:
    Parser(const string& filename); 
    vector<Code> ParserAndCodeGenerator(){
        P();
        return MidCode;
    }
};

Parser::Parser(const string& filename):lexer(filename) {
    init = 100;
    ptr = 0;
    temp_number = 0;
}

int Parser::getPreCodePtr()
{
    return MidCode.size()-1;
}

int Parser::emit(string op, string arg1, string arg2, string result){
    Code code;
    code.op=op;
    code.arg1=arg1;
    code.arg2=arg2;
    code.result=result;
    code.address=init;
    MidCode.push_back(code);
    ptr++;
    return init++;
}

int Parser::deletePreCode()
{
    if(MidCode.empty())
        return -1;
    MidCode.pop_back();
    init--;
    ptr--;
}

string Parser::getTemp()
{
    string temp;
    temp.append("T");
    temp.append(std::to_string(++temp_number));
    return temp;
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
        ASNST(token);
    }
}

// ASNST—>IDFS:=EXP
void Parser::ASNST(Token up_token){
    if(up_token.type!="ID"){
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

// <EXP>—[+|-]T EXP'
string Parser::EXP(){
    token = lexer.getNextToken();
    if(token.type != "+" || token.type == "-"){
        token = lexer.getNextToken();
    }
    T();
    _EXP();
    return "";
}

// EXP'->AOP T EXP' | <NULL>
void Parser::_EXP(){
    token = lexer.getPeekChar();
    if(";"==token.type){
        return;
    }
    AOP();
    T();
    _EXP();
}

// T—>FT'
string Parser::T(){
    string value = F();
    int index = emit("", value, "", "");// 第一个Factor的代码生成，后期会改
    string str = _T();
    if(str == "<null>"){
        return value; // 先前emit的代码被递归函数删除
    }
    else {
        return str; // 此表达式最终结果的临时存储（T形式）
    }
}

// T'->MOP F T' | <NULL>
string Parser::_T(){
    token = lexer.getPeekChar();
    int pre = getPreCodePtr();
    if(";" == token.type){
        string ret = MidCode[pre].arg1; // 取最后一个的结果
        deletePreCode();
        return ret;
    }
    string opt = MOP();
    string second_factor = F();
    string this_result = getTemp();

    MidCode[pre].op = opt;
    MidCode[pre].arg2 = second_factor;
    MidCode[pre].result = this_result;
    emit("",this_result,"","");
    return _T();
}

// F—>IDFS|UINT|(EXP)
string Parser::F(){
    token = lexer.getNextToken();
    if("ID" == token.type){
        return token.value;
    }
    else if("INT" == token.type){
        return token.value;
    }
    else if("(" == token.type){
        string str = EXP();
        token = lexer.getNextToken();
        if(")" != token.type){
            cerr<<"curves unmatched"<<endl;
            exit(1);
        }
        return str; //返回表达式的值（T的形式）
    }
    else{
        cerr<<"Wrong Factor"<<endl;
        exit(1);
    }
    return "";
}

// AOP—>+|-
string Parser::AOP(){
    token = lexer.getNextToken();
    if("+"!=token.type && "-"!=token.type){
        cerr<<"wrong add and substract operator"<<endl;
        exit(1);
    }
    return token.type;
}

// MOP->*|/
string Parser::MOP(){
    token = lexer.getNextToken();
    if("*"!=token.type && "/"!=token.type){
        cerr<<"wrong add and substract operator"<<endl;
        exit(1);
    }
    return token.type;
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
    return;// 返回待回填的索引
}
string Parser::ROP(){
    token = lexer.getNextToken();
    if("ROP"!=token.type){
        cerr<<"error condition operator"<<endl;
        exit(1);
    }
    return token.value;
}

// NULLST-> 空
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