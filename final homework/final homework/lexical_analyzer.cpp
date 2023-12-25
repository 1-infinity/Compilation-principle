#include "lexical_analyzer.h"
#include <cctype>
#include<iostream>
#include <algorithm>

// 构造函数
Lexer::Lexer(const std::string& filename) : file(filename), currentChar(' ') {
    if (!file.is_open()) {
        std::cerr << "无法打开文件 " << filename << std::endl;
        exit(1);
    }
}

// 读取下一个字符
void Lexer::readNextChar() {
    file.get(currentChar);
}

// 跳过空白字符
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

// 扫描标识符
Token Lexer::scanIdentifier() {
    string identifier;
    while (isalnum(currentChar)) {
        identifier += currentChar;
        readNextChar();
    }

    // 检查是否是关键字
    if (isKeyword(identifier)) {
        return { identifier, "-"};
    }
    else {
        return { "ID", identifier};
    }
}

// 扫描数字
Token Lexer::scanNumber() {
    string number;
    while (isdigit(currentChar)) {
        number += currentChar;
        readNextChar();
    }
    return { "INT", number};
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

    return { symbol, "-" };
}

// 扫描界符
Token Lexer::scanBound() {
    string symbol(1, currentChar);
    readNextChar();

    return { symbol, "-" };
}

// 获取下一个Token
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
        return { "ENDFILE", "-" };
    }
    else {
        // 无法识别的字符
        cerr << "无法识别的字符: " << currentChar << endl;
        exit(1);
    }
}

