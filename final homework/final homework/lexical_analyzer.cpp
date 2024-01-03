#include "lexical_analyzer.h"
#include <cctype>
#include<iostream>
#include <algorithm>

// 构造函数
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

Token Lexer::getPeekChar(){
    // 忽略空格
    string type;
    char str[2]={file.peek(),0};
    type.append(str);
    return Token(type,"-");
}

// 读取下一个字符
void Lexer::readNextChar() {
    file.get(currentChar);
}

// 跳过空白字符
void Lexer::skipWhitespace() {
    while (isspace(currentChar) && currentChar != EOF) {
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

// 扫描标识符
Token Lexer::scanIdentifier() {
    string identifier;
    while (isalnum(currentChar) && currentChar != EOF) {
        identifier += currentChar;
        readNextChar();
    }

    // 检查是否是关键字
    if (isKeyword(identifier)) {
        return Token(identifier, "-");
    }
    else {
        return Token("ID", identifier);
    }
}

// 扫描数字
Token Lexer::scanNumber() {
    string number;
    while (isdigit(currentChar)) {
        number += currentChar;
        readNextChar();
    }
    return Token("INT", number);
}

// 扫描
Token Lexer::scanOpt() {
    string symbol(1, currentChar);
    readNextChar();

    // 检查是否是多字符运算符
    if (currentChar == '='|| currentChar == '>') {
        symbol += currentChar;
        readNextChar();
    }

    return Token("ROP", symbol);
}

// 扫描界符
Token Lexer::scanBound() {
    string symbol(1, currentChar);
    readNextChar();

    return Token(symbol, "-");
}

// 获取下一个Token
Token Lexer::getNextToken() {
    skipWhitespace();

    if (isalpha(currentChar)) {
        // std::cout<<"isAlpha"<<endl;
        return scanIdentifier();
    }
    else if (isdigit(currentChar)) {
        // std::cout<<"isDigit"<<endl;
        return scanNumber();
    }
    else if (isBound(currentChar)) {
        // std::cout<<"isChar"<<endl;
        return scanBound();
    }
    else if (isOpt(string(1, currentChar))) {
        // std::cout<<"isOpt"<<endl;
        return scanOpt();
    }
    else if (file.eof()) {
        // std::cout<<"isEOF"<<endl;
        return Token("ENDFILE", "-");
    }
    else {
        // 无法识别的字符
        cerr << "error token: " << currentChar << endl;
        exit(1);
    }
}

