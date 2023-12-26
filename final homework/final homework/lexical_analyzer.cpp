#include "lexical_analyzer.h"
#include <cctype>
#include<iostream>
#include <algorithm>

// ���캯��
Lexer::Lexer(const std::string& filename) : currentChar(' ') {
    file.open(filename,ios::in);
    if (!file.is_open()) {
        std::cerr << "Cannot open file! " << filename << std::endl;
        exit(1);
    }
    keyWord.push_back("PROGRAM");
    keyWord.push_back("BEGIN");
    keyWord.push_back("END");
    keyWord.push_back("CONST");
    keyWord.push_back("VAR");
    keyWord.push_back("WHILE");
    keyWord.push_back("DO");
    keyWord.push_back("IF");
    keyWord.push_back("THEN");

    opt.push_back("+");
    opt.push_back("-");
    opt.push_back("*");
    opt.push_back("/");
    opt.push_back(":=");
    opt.push_back("=");
    opt.push_back("<>");
    opt.push_back("<");
    opt.push_back("<=");
    opt.push_back(">");
    opt.push_back(">=");
    opt.push_back(":");

    boundWord.push_back('(');
    boundWord.push_back(')');
    boundWord.push_back(';');
    boundWord.push_back(',');

}

Lexer::~Lexer(){
    file.close();
}

// ��ȡ��һ���ַ�
void Lexer::readNextChar() {
    file.get(currentChar);
}

// �����հ��ַ�
void Lexer::skipWhitespace() {
    while (isspace(currentChar)) {
        readNextChar();
    }
}

bool Lexer::isKeyword(const string& s) {
    vector<string>::iterator result = find(keyWord.begin(), keyWord.end(), s);

    if (result != keyWord.end()) {
        return true;
    }
    return false;
}

bool Lexer::isOpt(const string& s) {
    vector<string>::iterator result = find(opt.begin(), opt.end(), s);

    if (result != opt.end()) {
        return true;
    }
    return false;
}

bool Lexer::isBound(const char s) {
    vector<char>::iterator result = find(boundWord.begin(), boundWord.end(), s);

    if (result != boundWord.end()) {
        return true;
    }
    return false;
}

// ɨ���ʶ��
Token Lexer::scanIdentifier() {
    string identifier;
    while (isalnum(currentChar)) {
        identifier += currentChar;
        readNextChar();
    }

    // ����Ƿ��ǹؼ���
    if (isKeyword(identifier)) {
        return Token(identifier, "-");
    }
    else {
        return Token("ID", identifier);
    }
}

// ɨ������
Token Lexer::scanNumber() {
    string number;
    while (isdigit(currentChar)) {
        number += currentChar;
        readNextChar();
    }
    return Token("INT", number);
}

// ɨ��
Token Lexer::scanOpt() {
    string symbol(1, currentChar);
    readNextChar();

    // ����Ƿ��Ƕ��ַ������
    if (currentChar == '='|| currentChar == '>') {
        symbol += currentChar;
        readNextChar();
    }

    return Token(symbol, "-");
}

// ɨ����
Token Lexer::scanBound() {
    string symbol(1, currentChar);
    readNextChar();

    return Token(symbol, "-");
}

// ��ȡ��һ��Token
Token Lexer::getNextToken() {
    skipWhitespace();

    if (isalpha(currentChar)) {
        return scanIdentifier();
    }
    else if (isdigit(currentChar)) {
        return scanNumber();
    }
    else if (isBound(currentChar)) {
        return scanBound();
    }
    else if (isOpt(string(1, currentChar))) {
        return scanOpt();
    }
    else if (file.eof()) {
        return Token("ENDFILE", "-");
    }
    else {
        // �޷�ʶ����ַ�
        cerr << "error token: " << currentChar << endl;
        exit(1);
    }
}

