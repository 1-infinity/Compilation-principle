
#pragma once
#include <iostream>
#include <map>
#include <queue>
#include <iomanip>
#include "lexical_analyzer.h"
using namespace std;

Token token;

enum Elem {
    ERROR,      //错误
    P = 1,        //程序
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

enum class VarType{Var, Const};

typedef struct {
    string name;
    VarType type;
} var;

class Parser {
private:
    vector<Code> MidCode;
    vector<var> VarList;
    // 起始地址
    int init;
    // MidCode指针
    int ptr;
    int temp_number;
    Lexer lexer;

    void P();
    void PHEAD();
    void SP();
    void C();// 如果看了FISRT，则第一个token已经被读取，此函数不再读取
    void CDF();
    string UINT();
    void V();// 同上
    string IDFS();
    void CPLXST();
    void ST();
    void ASNST();
    string EXP();
    string _EXP(const string& arg1);// 消除左递归
    string T();
    string _T(const string& arg1);// 消除左递归
    string F();
    string AOP();
    string MOP();
    void CONDST();
    void LOOP();
    int COND();
    string ROP();
    void NULLST();
    void M(); // 空，打断

    string getTemp();// 寄存器值T1, T2, ...
    int emit(string op, string arg1, string arg2, string result);// 璐熻矗鍐欏叆涓�鏉′腑闂翠唬鐮侊紝杩斿洖璇ヤ唬鐮佺储寮�
    int getPreCodePtr();
    int deletePreCode();
    bool existedIDFS(string name);
    var findIDFS(string name);
public:
    Parser(const string& filename);
    vector<Code> ParserAndCodeGenerator() {
        P();
        return MidCode;
    }
};

ostream& operator<<(ostream& out, const Code& code)
{
    out << setw(10) << code.address << setw(10) << code.op << setw(10) << code.arg1 << setw(10) << code.arg2 << setw(10) << code.result;
    return out;
}

Parser::Parser(const string& filename) :lexer(filename) {
    init = 100;
    ptr = 0;
    temp_number = 0;
}

int Parser::getPreCodePtr()
{
    return MidCode.size() - 1;
}

int Parser::emit(string op, string arg1, string arg2, string result) {
    Code code;
    code.op = op;
    code.arg1 = arg1;
    code.arg2 = arg2;
    code.result = result;
    code.address = init;
    MidCode.push_back(code);
    ptr++;
    return init++;
}

int Parser::deletePreCode()
{
    if (MidCode.empty())
        return -1;
    MidCode.pop_back();
    init--;
    ptr--;
    return ptr;
}

string Parser::getTemp()
{
    string temp;
    temp.append("T");
    temp.append(std::to_string(++temp_number));
    return temp;
}

bool Parser::existedIDFS(string name)
{
    for(int i=0;i<VarList.size();i++){
        if(name == VarList[i].name){
            return true;
        }
    }
    return false;
}

var Parser::findIDFS(string name)
{
    for(unsigned int index = 0; index < VarList.size(); index++) {
        if(VarList[index].name == name) {
            return VarList[index];
        }
    }
    var nullobject;
    nullobject.name = "<null>";
    return nullobject;
}

// P->PHEAD SP
void Parser::P() {
    PHEAD();
    SP();
}

// PHEAD -> 'PROGRAM' IDFS
void Parser::PHEAD() {
    token = lexer.getNextToken();
    if (token.type == "PROGRAM") {
        token = lexer.getNextToken();
    }
    else {
        cerr << "line " << token.line << " " << "Program doesn't start with \'PROGRAM\'!" << endl;
        exit(1);
    }
    IDFS();
}

// SP-> [C][V]ST
void Parser::SP() {
    if (token.type == "CONST") {
        C();
    }
    if (token.type == "VAR") {
        V();
    }
    // sentence
    ST();
    if (token.type != "$") {
        cout << "redundant information at the end of the program" << endl;
        exit(1);
    }
}

// C-> 'CONST'CDF{,CDF};
void Parser::C() {
    if (token.type == "CONST") {
        token = lexer.getNextToken();
        do {
            CDF();
            if (token.type == ",") {
                token = lexer.getNextToken();
                continue;
            }
            else if (token.type == ";") {
                token = lexer.getNextToken();
                break;
            }
            else {
                cerr << "line " << token.line << " " << "C: Unrecoginzed symbol when initilizing const values" << endl;
                exit(1);
            }
        } while (true);
    }
}

// CDF->IDFS:=UINT
void Parser::CDF() {
    string idfs = IDFS();
    if(existedIDFS(idfs)){
        cerr<<"CDF: Depulicated Const Statement"<<endl;
        exit(1);
    }
    if (token.type == ":=") {
        token = lexer.getNextToken();
    }
    else {
        cerr << "line " << token.line << " " << "Tempt to initilizing const values with error identifier" << endl;
        exit(1);
    }
    string number = UINT();
    emit(":=", number, "-", idfs);
    var new_var;
    new_var.name = idfs;
    new_var.type = VarType::Const;
    VarList.push_back(new_var);
}

// UINT->N{N}
string Parser::UINT() {
    if (token.type == "INT") {
        string result = token.value;
        token = lexer.getNextToken();
        return result;
    }
    else {
        cerr <<"line "<<token.line<<" " << "Using error type" << endl;
        exit(1);
    }
    return "";
}

// V->'VAR'IDFS{,IDFS};
void Parser::V() {
    if (token.type == "VAR") {
        token = lexer.getNextToken();
    }
    else {
        cerr << "line " << token.line << " " << "\'VAR\' symbol missing" << endl;
        exit(1);
    }

    while (true) {
        string idfs = IDFS();
        if(existedIDFS(idfs)){
            cerr << "line " << token.line << " " << "CDF: Depulicated Variable Statement"<<endl;
            exit(1);
        }
        var new_var;
        new_var.name = idfs;
        new_var.type = VarType::Var;
        VarList.push_back(new_var);
        if (token.type == ",") {
            token = lexer.getNextToken();
            continue;
        }
        else if (token.type == ";") {
            token = lexer.getNextToken();
            break;
        }
        else {
            cerr << "line " << token.line << " " << "V: Unrecoginzed symbol when initilizing vars, check whether \',\' or \';\' is missing" << endl;
            exit(1);
        }
    }
}

// IDFS —> L{L|D}
string Parser::IDFS() {
    if (token.type == "ID") {
        string result = token.value;
        token = lexer.getNextToken();
        return result;
    }
    else {
        cerr << "line " << token.line << " " << "Identifier format wrong" << endl;
        exit(1);
    }
    return "";
}


// ST—>ASNST|CONDST|LOOP|CPLXST|NULLST
void Parser::ST() {
    // 根据首符集判断
    if ("IF" == token.type) {
        CONDST();
    }
    else if ("WHILE" == token.type) {
        LOOP();
    }
    else if ("BEGIN" == token.type) {
        CPLXST();
    }
    else if ("END" == token.type || token.type == "$"||token.type == ";") {
        NULLST();
    }
    else if (token.type == "ID") {
        ASNST();
    }
    else {
        cerr << "line " << token.line << " " << "ST error" << endl;
    }
}

// CPLXST—>'BEGIN' ST{;ST}'END'
void Parser::CPLXST() {
    if (token.type == "BEGIN") {
        token = lexer.getNextToken();
    }
    else {
        cerr << "line " << token.line << " " << "Missing \'BEGIN\' with complex sentence" << endl;
        exit(1);
    }
    while (true) {
        ST();
        if (";" == token.type) {
            //if (lexer.nextToken().type == "END") {
            //    cerr << "CPLXST: excessive \';\' before END" << endl;
            //    exit(1);
            //}
            token = lexer.getNextToken();
            continue;
        }
        else if ("END" == token.type) {
            token = lexer.getNextToken();
            break;
        }
        else {
            cerr << "line " << token.line << " " << "sentence ends with wrong codes" << endl;
            exit(1);
        }
    }   
}

// ASNST—>IDFS:=EXP
void Parser::ASNST(){
    string result = IDFS();
    var existed = findIDFS(result);
    if(existed.name == "<null>"){
        cerr << "line " << token.line << " " << "ASNST: unstated identifier " << "\'" << result << "\'" << endl;
        exit(1);
    }
    else if(existed.type == VarType::Const) {
        cerr << "line " << token.line << " " << "ASNST: const identifier cannot be assigned."<<endl;
        exit(1);
    }
    string opt = "";
    if (token.type == ":=") {
        opt = token.type;
        token = lexer.getNextToken();
    }
    else {
        cerr << "line " << token.line << " " << "Assignment sentence wrong: not using \':=\'" << endl;
        exit(1);
    }
    string arg1 = EXP();
    emit(opt,arg1,"-",result);
    return;
}


// <EXP>—[+|-]T EXP'
string Parser::EXP() {
    string T_part;
    if (token.type == "AOP" && token.value == "+") {
        token = lexer.getNextToken();
        T_part = T();
    }
    else if(token.type == "AOP" && token.value == "-") {
        token = lexer.getNextToken();
        if (token.type == "ID") {
            T_part = getTemp();
            emit("uminus", T(), "-", T_part); // return register name
        }
        else {
            cerr << "line " << token.line << " " << "The system only support unsigned integer." << endl;
            exit(1);
        }
    }
    else if (token.type == "ID" || token.type == "INT" || token.type == "(") {
        T_part = T();
    }
    else {
        cerr << "line " << token.line << " " << "Wrong expression" << endl;
        exit(1);
    }
    return _EXP(T_part);
}

// EXP'->AOP T EXP' | <NULL>
string Parser::_EXP(const string& arg1) {
    if (token.type == "ROP" || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END" || token.type == "$") {
        return arg1;
    }
    string opt = AOP();
    string value = T();
    string result = getTemp();

    emit(opt,arg1,value,result);
    return _EXP(result);
}

// T->FT'
string Parser::T(){
    string value = F();
    return _T(value);
}

// T'->MOP F T' | <NULL>
string Parser::_T(const string& arg1) {
    // 空字
    if (token.type == "AOP" || token.type == "ROP" || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END" || token.type == "$") {
        return arg1;
    }

    string opt = MOP();
    string value = F();
    string this_result = getTemp();

    emit(opt, arg1, value, this_result);
    return _T(this_result);
}

// F—>IDFS|UINT|(EXP)
string Parser::F() {
    string result;
    if ("ID" == token.type) {
        result = IDFS();
        var existed = findIDFS(result);
        if(existed.name == "<null>"){
            cerr << "line " << token.line << " " << "ASNST: unstated identifier " << "\'" << result << "\'" << endl;
            exit(1);
        }
    }
    else if ("INT" == token.type) {
        result = UINT();
    }
    else if ("(" == token.type) {
        token = lexer.getNextToken();
        result = EXP();
        if (")" == token.type) {
            token = lexer.getNextToken();
        }
        else {
            cerr << "line " << token.line << " " << "curves unmatched" << endl;
            exit(1);
        }
    }
    else {
        cerr << "line " << token.line << " " << "Wrong Factor" << endl;
        exit(1);
    }
    return result;
}

// AOP—>+|-
string Parser::AOP() {
    if (token.type == "AOP") {
        string result = token.value;
        token = lexer.getNextToken();
        return result;
    }
    else {
        cerr << "line " << token.line << " " << "wrong add and substract operator" << endl;
        exit(1);
    }
    return "";
}

// MOP->*|/
string Parser::MOP() {
    if (token.type == "MOP") {
        string result = token.value;
        token = lexer.getNextToken();
        return result;
    }
    // else if token.type == 保留字 返回错误信息
    else if (token.type == "PROGRAM" || token.type == "BEGIN" || token.type == "END" || token.type == "CONST" || token.type == "VAR" || token.type == "WHILE" || token.type == "DO" || token.type == "IF" || token.type == "THEN") {
        cerr << "line " << token.line << " " << "Unexpected reserved identifier: " << token.type << endl;
        exit(1);
    }
    else {
        cerr << "line " << token.line << " " << "wrong multiplication and Division operator" << endl;
        exit(1);
    }
    return "";
}

// CONDST->IF COND THEN ST
void Parser::CONDST() {
    if ("IF" == token.type) {
        token = lexer.getNextToken();
        int filling_in = COND();
        if (token.type == "THEN") {
            token = lexer.getNextToken();
            ST();
            // get address to be filled into that code
            MidCode[filling_in].result = to_string(init);
        }
        else {
            cerr << "line " << token.line << " " << "error condition sentence" << endl;
            exit(1);
        }
    }
    else {
        cerr << "line " << token.line << " " << "error condition sentence" << endl;
        exit(1);
    }
}

// LOOP->WHILE COND DO ST
void Parser::LOOP() {
    if (token.type == "WHILE") {
        token = lexer.getNextToken();
        int start_address = init;
        int filling_in = COND();
        if (token.type == "DO") {
            token = lexer.getNextToken();
        }
        else {
            cerr << "line " << token.line << " " << "error in loop:lose DO" << endl;
            exit(1);
        }
        ST();
        emit("j","-","-",to_string(start_address));
        MidCode[filling_in].result = to_string(init);
    }
    else {
        cerr << "line " << token.line << " " << "error in loop:lose WHILE" << endl;
        exit(1);
    }
}

// COND->EXP ROP EXP
int Parser::COND() {
    string exp1 = EXP();
    string rop = ROP();
    string exp2 = EXP();
    emit("j"+rop, exp1, exp2, to_string(init+2));
    emit("j","-","-","-"); // wait for being filled
    return ptr - 1;
}

// ROP-> = | <> | < | <= | > | >=
string Parser::ROP() {
    if (token.type == "ROP") {
        string result = token.value;
        token = lexer.getNextToken();
        return result;
    }
    else {
        cerr << "line " << token.line << " " << "error condition operator" << endl;
        exit(1);
    }
    return "";
}

// M->空
void Parser::NULLST()
{
    return;
}

//void Parser::M() {
//
//    return;
//}