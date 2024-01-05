#pragma once
#include <iostream>
#include <map>
#include <queue>
#include "lexical_analyzer.h"
using namespace std;

Token token;

enum Elem {
    ERROR,      //����
    P = 1,        //����
    SP,         //�ӳ���
    PHEAD,      //�����ײ�
    IDFS,       //��ʶ��
    C,          //����˵��
    V,          //����˵��
    ST,         //���
    CDF,        //��������
    UINT,       //�޷�������
    N,          //����
    L,          //��ĸ
    CPLXST,     //�������
    ASNST,      //��ֵ���
    CONDST,     //�������
    LOOP,       //ѭ�����
    NULLST,     //�����
    EXP,        //���ʽ
    T,          //����
    AOP,        //�ӷ������
    F,          //����
    MOP,        //�˷������
    COND,       //����
    ROP         //��ϵ�����
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

class Parser {
private:

    vector<Code> MidCode;
    var Vars[100];
    // ��ʼ��ַ
    int init;
    // MidCodeָ��
    int ptr;
    Lexer lexer;

    void P();
    void PHEAD();
    void SP();
    void C();// �������FISRT�����һ��token�Ѿ�����ȡ���˺������ٶ�ȡ
    void CDF();
    void UINT();
    void V();// ͬ��
    void IDFS();
    void CPLXST();
    void ST();
    void ASNST();
    void EXP();
    void _EXP();// ������ݹ�
    void T();
    void _T();// ������ݹ�
    void F();
    void AOP();
    void MOP();
    void CONDST();
    void LOOP();
    void COND();
    void ROP();
    void NULLST();
    void M(); // �գ����
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
    IDFS();
    token = lexer.getToken();
    if (token.type == "ROP" && token.value == ":=") {
        token = lexer.getNext();
    }
    else {
        cerr << "Tempt to initilizing const values with error identifier" << endl;
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
        cerr << "Using error type" << endl;
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
        cerr << "\'VAR\' symbol missing" << endl;
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
            cerr << "Unrecoginzed symbol when initilizing vars" << endl;
        }
    }
}

// IDFS ��> L{L|D}
void Parser::IDFS() {
    token = lexer.getToken();
    if (token.type == "ID") {
        token = lexer.getNext();
        return;
    }
    else {
        cerr << "Identifier format wrong" << endl;
        exit(1);
    }
}

// ST��>ASNST|CONDST|LOOP|CPLXST|NULLST
void Parser::ST() {
    // �����׷����ж�
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

// CPLXST��>'BEGIN' ST{;ST}'END'
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

// ASNST��>IDFS:=EXP
void Parser::ASNST(){
    IDFS();
    if (token.type == "ROP" && token.value == ":=") {
        token = lexer.getNext();
    }
    else {
        cerr<<"Assignment sentence wrong: not using \':=\'"<<endl;
        exit(1);
    }
    EXP();
}

// <EXP>��[+|-]T EXP'
void Parser::EXP() {
    if (token.value == "-" || token.value == "+") {
        token = lexer.getNext();
        T();
    }
    else if (token.type == "ID" || token.type == "INT" || token.type == "(") {
        T();
    }
    else {
        cerr << "Wrong expression" << endl;
        exit(1);
    }
    _EXP();
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

// T��>FT'
void Parser::T() {
    F();
    _T();
}

// T'->MOP F T' | <NULL>
void Parser::_T() {
    // ����
    if ((token.type == "ROP" && token.value == "+") || (token.type == "ROP" && token.value == "-") || (token.type == "ROP" && token.value == "=") || (token.type == "ROP" && token.value == "<>") || (token.type == "ROP" && token.value == "<") || (token.type == "ROP" && token.value == "<=") || (token.type == "ROP" && token.value == ">") || (token.type == "ROP" && token.value == ">=") || token.type == ";" || token.type == ")" || token.type == "THEN" || token.type == "DO" || token.type == "END") {
        return;
    }
    MOP();
    F();
    _T();
}

// F��>IDFS|UINT|(EXP)
void Parser::F() {
    token = lexer.getToken();
    if ("ID" == token.type) {
        IDFS();
    }
    else if ("INT" == token.type) {
        UINT();
    }
    else if ("(" == token.type) {
        EXP();
        token = lexer.getNext();
        if ("(" == token.type) {
            token = lexer.getNext();
        }
        else {
            cerr << "curves unmatched" << endl;
        }
    }
    else {
        cerr << "Wrong Factor" << endl;
    }
}

// AOP��>+|-
void Parser::AOP() {
    token = lexer.getToken();
    if ((token.type == "ROP" || token.value == "+") || (token.type == "ROP" || token.value == "-")) {
        token = lexer.getNext();
    }
    else {
        cerr << "wrong add and substract operator" << endl;
        exit(1);
    }
}

// MOP->*|/
void Parser::MOP() {
    token = lexer.getToken();
    if ((token.type == "ROP" || token.value == "*") || (token.type == "ROP" || token.value == "/")) {
        token = lexer.getNext();
    }
    else {
        cerr << "wrong add and substract operator" << endl;
        exit(1);
    }
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
        COND();
        if (token.type == "DO") {
            token = lexer.getNext();
        }
        else {
            cerr << "error in loop:lose DO" << endl;
            exit(1);
        }
        ST();
    }
    else {
        cerr << "error in loop:lose WHILE" << endl;
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
void Parser::ROP() {
    if (token.type == "ROP") {
        token = lexer.getNext();
    } 
    else {
        cerr << "error condition operator" << endl;
        exit(1);
    }
}

// M->��
void Parser::NULLST()
{
    return;
}

//void Parser::M() {
//
//    return;
//}
