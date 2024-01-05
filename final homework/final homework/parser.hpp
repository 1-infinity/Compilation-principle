
#pragma once
#include <iostream>
#include <map>
#include <queue>
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
    string _EXP();// 消除左递归
    string T();
    string _T();// 消除左递归
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
    cout << "(" << code.op << ',' << code.arg1 << ',' << code.arg2 << ',' << code.result << ')' << endl;
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
    token = lexer.getToken();
    if (token.type == "PROGRAM") {
        token = lexer.getNext();
    }
    else {
        cerr << "Program doesn't start with \'PROGRAM\'!" << endl;
        exit(1);
    }
    IDFS();
}

// SP-> [C][V]ST
void Parser::SP() {
    token = lexer.getToken();
    if (token.type == "CONST") {
        C();
    }
    if (token.type == "VAR") {
        V();
    }
    // sentence
    ST();
}

// C-> 'CONST'CDF{,CDF};
void Parser::C() {
    token = lexer.getToken();
    if (token.type == "CONST") {
        lexer.getNext();
        do {
            CDF();
            token = lexer.getToken();
            if (token.type == ",") {
                token = lexer.getNext();
                continue;
            }
            else if (token.type == ";") {
                token = lexer.getNext();
                break;
            }
            else
                cout << "Unrecoginzed symbol when initilizing const values" << endl;
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
    token = lexer.getToken();
    if (token.type == ":=") {
        token = lexer.getNext();
    }
    else {
        cerr << "Tempt to initilizing const values with error identifier" << endl;
        exit(1);
    }
    string number = UINT();
    emit("j" + token.value, number, "-", idfs);
    var new_var;
    new_var.name = idfs;
    new_var.type = VarType::Const;
    VarList.push_back(new_var);
}

// UINT->N{N}
string Parser::UINT() {
    token = lexer.getToken();
    if (token.type == "INT") {
        token = lexer.getNext();
    }
    else {
        cerr << "Using error type" << endl;
        exit(1);
    }
    return token.value;
}

// V->'VAR'IDFS{,IDFS};
void Parser::V() {
    token = lexer.getToken();
    if (token.type == "VAR") {
        token = lexer.getNext();
    }
    else {
        cerr << "\'VAR\' symbol missing" << endl;
        exit(1);
    }

    while (true) {
        string idfs = IDFS();
        if(existedIDFS(idfs)){
            cerr<<"CDF: Depulicated Const Statement"<<endl;
            exit(1);
        }
        var new_var;
        new_var.name = idfs;
        new_var.type = VarType::Var;
        VarList.push_back(new_var);
        token = lexer.getToken();
        if (token.type == ",") {
            token = lexer.getNext();
            continue;
        }
        else if (token.type == ";") {
            token = lexer.getNext();
            break;
        }
        else {
            cerr << "Unrecoginzed symbol when initilizing vars" << endl;
        }
    }
}

// IDFS —> L{L|D}
string Parser::IDFS() {
    token = lexer.getToken();
    if (token.type == "ID") {
        string result = token.value;
        token = lexer.getNext();
    }
    else {
        cerr << "Identifier format wrong" << endl;
        exit(1);
    }
    return token.value;
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
    else if ("END" == token.type) {
        NULLST();
    }
    else if (token.type == "ID") {
        ASNST();
    }
    else {
        cerr << "ST error" << endl;
    }
}

// CPLXST—>'BEGIN' ST{;ST}'END'
void Parser::CPLXST() {
    if (token.type == "BEGIN") {
        token = lexer.getNext();
    }
    else {
        cerr << "Missing \'BEGIN\' with complex sentence" << endl;
        exit(1);
    }
    while (true) {
        ST();
        if (";" == token.type) {
            token = lexer.getNext();
            continue;
        }
        else if ("END" == token.type) {
            break;
        }
        else {
            cerr << "sentence ends with wrong codes" << endl;
            exit(1);
        }
    }
}

// ASNST—>IDFS:=EXP
void Parser::ASNST(){
    string result = IDFS();
    var existed = findIDFS(result);
    if(existed.name == "<null>"){
        cerr<<"ASNST: unstated identifier"<<endl;
        exit(1);
    }
    else if(existed.type == VarType::Const) {
        cerr<<"ASNST: const identifier cannot be assigned."<<endl;
        exit(1);
    }
    string opt = "";
    if (token.type == ":=") {
        opt = token.value;
        token = lexer.getNext();
    }
    else {
        cerr << "Assignment sentence wrong: not using \':=\'" << endl;
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
        token = lexer.getNext();
        T_part = T();
    }
    else if(token.type == "AOP" && token.value == "-") {
        token = lexer.getNext();
        T_part = getTemp();
        emit("uminus", T(), "-", T_part); // return register name
    }
    else if (token.type == "ID" || token.type == "INT" || token.type == "(") {
        T_part = T();
    }
    else {
        cerr << "Wrong expression" << endl;
        exit(1);
    }
    emit("-",T_part,"-","-");
    return _EXP();
}

// EXP'->AOP T EXP' | <NULL>
string Parser::_EXP() {
    int pre = getPreCodePtr();
    if (token.type == "ROP" || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END") {
        string ret = MidCode[pre].arg1;
        deletePreCode();
        return ret;
    }
    string opt = AOP();
    string value = T();
    string result = getTemp();

    MidCode[pre].op = opt;
    MidCode[pre].arg2 = value;
    MidCode[pre].result = result;
    emit("",result,"","");
    return _EXP();
}

// T->FT'
string Parser::T(){
    string value = F();
    int index = emit("", value, "", "");// 绗竴涓狥actor鐨勪唬鐮佺敓鎴愶紝鍚庢湡浼氭敼
    return _T();
}

// T'->MOP F T' | <NULL>
string Parser::_T() {
    // 空字
    int pre = getPreCodePtr();
    if (token.type == "AOP" || token.type == "ROP" || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END") {
        string ret = MidCode[pre].arg1; // 鍙栨渶鍚庝竴涓殑缁撴灉
        deletePreCode();
        return ret;
    }

    string opt = MOP();
    string second_factor = F();
    string this_result = getTemp();

    MidCode[pre].op = opt;
    MidCode[pre].arg2 = second_factor;
    MidCode[pre].result = this_result;
    emit("", this_result, "", "");
    return _T();
}

// F—>IDFS|UINT|(EXP)
string Parser::F() {
    token = lexer.getToken();
    string result;
    if ("ID" == token.type) {
        result = IDFS();
        var existed = findIDFS(result);
        if(existed.name == "<null>"){
            cerr<<"ASNST: unstated identifier"<<endl;
            exit(1);
        }
    }
    else if ("INT" == token.type) {
        result = UINT();
    }
    else if ("(" == token.type) {
        token = lexer.getNext();
        result = EXP();
        if (")" == token.type) {
            token = lexer.getNext();
        }
        else {
            cerr << "curves unmatched" << endl;
            exit(1);
        }
    }
    else {
        cerr << "Wrong Factor" << endl;
        exit(1);
    }
    return result;
}

// AOP—>+|-
string Parser::AOP() {
    token = lexer.getToken();
    if (token.type == "AOP") {
        token = lexer.getNext();
    }
    else {
        cerr << "wrong add and substract operator" << endl;
        exit(1);
    }
    return token.value;
}

// MOP->*|/
string Parser::MOP() {
    token = lexer.getToken();
    if (token.type == "MOP") {
        token = lexer.getNext();
    }
    else {
        cerr << "wrong add and substract operator" << endl;
        exit(1);
    }
    return token.value;
}

// CONDST->IF COND THEN ST
void Parser::CONDST() {
    token = lexer.getToken();
    if ("IF" == token.type) {
        token = lexer.getNext();
        int filling_in = COND();
        if (token.type == "THEN") {
            token = lexer.getNext();
            ST();
            // get address to be filled into that code
            MidCode[filling_in].result = to_string(init);
        }
        else {
            cerr << "error condition sentence" << endl;
            exit(1);
        }
    }
    else {
        cerr << "error condition sentence" << endl;
        exit(1);
    }
}

// LOOP->WHILE COND DO ST
void Parser::LOOP() {
    if (token.type == "WHILE") {
        token = lexer.getNext();
        int start_address = init;
        int filling_in = COND();
        if (token.type == "DO") {
            token = lexer.getNext();
        }
        else {
            cerr << "error in loop:lose DO" << endl;
            exit(1);
        }
        ST();
        emit("j","-","-",to_string(start_address));
        MidCode[filling_in].result = to_string(init);
    }
    else {
        cerr << "error in loop:lose WHILE" << endl;
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
    return init - 1;
}

// ROP-> = | <> | < | <= | > | >=
string Parser::ROP() {
    if (token.type == "ROP") {
        token = lexer.getNext();
    }
    else {
        cerr << "error condition operator" << endl;
        exit(1);
    }
    return token.value;
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