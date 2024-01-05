
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

typedef struct {
    string name;
    string prop;
    string Register;
} var;

class Parser {
private:
    vector<Code> MidCode;
    var Vars[100];
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
    void UINT();
    void V();// 同上
    void IDFS();
    void CPLXST();
    void ST();
    void ASNST();
    string EXP();
    void _EXP();// 消除左递归
    string T();
    string _T();// 消除左递归
    string F();
    string AOP();
    string MOP();
    void CONDST();
    void LOOP();
    void COND();
    string ROP();
    void NULLST();
    void M(); // 空，打断
      
    string getTemp();// 寄存器值T1, T2, ...
    int emit(string op, string arg1, string arg2, string result);// 璐熻矗鍐欏叆涓�鏉′腑闂翠唬鐮侊紝杩斿洖璇ヤ唬鐮佺储寮�
    int getPreCodePtr();
    int deletePreCode();
public:
    Parser(const string& filename);
    vector<Code> ParserAndCodeGenerator() {
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
void Parser::PHEAD() {
    token = lexer.getToken();
    if (token.type == "PROGRAM") {
        token = lexer.getNext();
    }
    else {
        cerr << "PHEAD: Program doesn't start with \'PROGRAM\'!" << endl;
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
    IDFS();
    token = lexer.getToken();
    if (token.type == "ROP" && token.value == ":=") {
        token = lexer.getNext();
    }
    else {
        cerr << "CDF: Tempt to initilizing const values with error identifier" << endl;
        exit(1);
    }
    UINT();
}

// UINT->N{N}
void Parser::UINT() {
    token = lexer.getToken();
    if (token.type == "INT") {
        token = lexer.getNext();
    }
    else{
        cerr << "UINT: Using error type" << endl;
        exit(1);
    }
}

// V->'VAR'IDFS{,IDFS};
void Parser::V() {
    token = lexer.getToken();
    if (token.type == "VAR") {
        token = lexer.getNext();
    }
    else {
        cerr << "V: \'VAR\' symbol missing" << endl;
        exit(1);
    }

    while (true) {
        IDFS();
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
            cerr << "V: Unrecoginzed symbol when initilizing vars" << endl;
        }
    }
}

// IDFS —> L{L|D}
void Parser::IDFS() {
    token = lexer.getToken();
    if (token.type == "ID") {
        token = lexer.getNext();
        return;
    }
    else {
        cerr << "IDFS: Identifier format wrong" << endl;
        exit(1);
    }
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
        cerr << "ST: ST error" << endl;
    }
}

// CPLXST—>'BEGIN' ST{;ST}'END'
void Parser::CPLXST() {
    if (token.type == "BEGIN") {
        token = lexer.getNext();
    }
    else {
        cerr << "COLXST: Missing \'BEGIN\' with complex sentence" << endl;
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
            cerr << "CPLXST: Sentence ends with wrong codes" << endl;
            exit(1);
        }
    }
}

// ASNST—>IDFS:=EXP
void Parser::ASNST(){
    IDFS();
    if (token.type == "ROP" && token.value == ":=") {
        token = lexer.getNext();
    }
    else {
        cerr<<"ASNST: Assignment sentence wrong: not using \':=\'"<<endl;
        exit(1);
    }
    EXP();
}


// <EXP>—[+|-]T EXP'
string Parser::EXP() {
    if (token.value == "-" || token.value == "+") {
        token = lexer.getNext();
        T();
    }
    else if (token.type == "ID" || token.type == "INT" || token.type == "(") {
        T();
    }
    else {
        cerr << "EXP: Wrong expression" << endl;
        exit(1);
    }
    _EXP();
    return "";
}

// EXP'->AOP T EXP' | <NULL>
void Parser::_EXP() {
    if ((token.type == "ROP" && token.value == "=") || (token.type == "ROP" && token.value == "<>") || (token.type == "ROP" && token.value == "<") || (token.type == "ROP" && token.value == "<=") || (token.type == "ROP" && token.value == ">") || (token.type == "ROP" && token.value == ">=") || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END") {
        return;
    }
    MOP();
    T();
    _EXP();
}


// T鈥�>FT'
string Parser::T(){
    string value = F();
    int index = emit("", value, "", "");// 绗竴涓狥actor鐨勪唬鐮佺敓鎴愶紝鍚庢湡浼氭敼
    string str = _T();
    if(str == "<null>"){
        return value; // 鍏堝墠emit鐨勪唬鐮佽閫掑綊鍑芥暟鍒犻櫎
    }
    else {
        return str; // 姝よ〃杈惧紡鏈�缁堢粨鏋滅殑涓存椂瀛樺偍锛圱褰㈠紡锛�
    }
}

// T'->MOP F T' | <NULL>
string Parser::_T() {
    // 空字
    int pre = getPreCodePtr();
    if ((token.type == "ROP" && token.value == "+") || (token.type == "ROP" && token.value == "-") || (token.type == "ROP" && token.value == "=") || (token.type == "ROP" && token.value == "<>") || (token.type == "ROP" && token.value == "<") || (token.type == "ROP" && token.value == "<=") || (token.type == "ROP" && token.value == ">") || (token.type == "ROP" && token.value == ">=") || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END") {
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
    emit("",this_result,"","");
    return _T();
}

// F—>IDFS|UINT|(EXP)
string Parser::F() {
    token = lexer.getToken();
    if ("ID" == token.type) {
        IDFS();
    }
    else if ("INT" == token.type) {
        UINT();
    }
    else if ("(" == token.type) {
        token = lexer.getNext();
        EXP();
        if (")" == token.type) {
            token = lexer.getNext();
        }
        else {
            cerr << "F: curves unmatched" << endl;
            exit(1);
        }
    }
    else {
        cerr << "F: Wrong Factor" << endl;
        exit(1);
    }
    return "";
}

// AOP—>+|-
string Parser::AOP() {
    token = lexer.getToken();
    if ((token.type == "ROP" && token.value == "+") || (token.type == "ROP" && token.value == "-")) {
        token = lexer.getNext();
        return token.value;
    }
    else {
        cerr << "AOP: wrong add and substract operator" << endl;
        exit(1);
    }
    return "";
}

// MOP->*|/
string Parser::MOP() {
    token = lexer.getToken();
    if ((token.type == "ROP" && token.value == "*") || (token.type == "ROP" && token.value == "/")) {
        token = lexer.getNext();
        return token.value;
    }
    else {
        cerr << "MOP: wrong add and substract operator" << endl;
        exit(1);
    }
    return "";
}

// CONDST->IF COND THEN ST
void Parser::CONDST() {
    token = lexer.getToken();
    if ("IF" == token.type) {
        token = lexer.getNext();
        COND();
        if (token.type == "THEN") {
            token = lexer.getNext();
            ST();
        }
        else {
            cerr << "CONDST: error condition sentence" << endl;
            exit(1);
        }
    }
    else {
        cerr << "CONDST: error condition sentence" << endl;
        exit(1);
    }
}

// LOOP->WHILE COND DO ST
void Parser::LOOP() {
    if (token.type == "WHILE") {
        token = lexer.getNext();
        COND();
        if (token.type == "DO") {
            token = lexer.getNext();
        }
        else {
            cerr << "LOOP: error in loop:lose DO" << endl;
            exit(1);
        }
        ST();
    }
    else {
        cerr << "LOOP: error in loop:lose WHILE" << endl;
        exit(1);
    }
}
 
// COND->EXP ROP EXP
void Parser::COND() {
    EXP();
    ROP();
    EXP();
}

// ROP-> = | <> | < | <= | > | >=
string Parser::ROP() {
    if (token.type == "ROP") {
        token = lexer.getNext();
    } 
    else {
        cerr << "ROP: error condition operator" << endl;
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
