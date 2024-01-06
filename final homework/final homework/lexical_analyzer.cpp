#include "lexical_analyzer.h"
#include <cctype>
#include<iostream>
#include <algorithm>

// 构造函数
Lexer::Lexer(const std::string& filename) : currentChar(' ') {
    file.open(filename, ios::in);
    if (!file.is_open()) {
        std::cerr << "Cannot open file! " << filename << std::endl;
        exit(1);
    }
    line = 1;

    keyWord.push_back("PROGRAM");
    keyWord.push_back("BEGIN");
    keyWord.push_back("END");
    keyWord.push_back("CONST");
    keyWord.push_back("VAR");
    keyWord.push_back("WHILE");
    keyWord.push_back("DO");
    keyWord.push_back("IF");
    keyWord.push_back("THEN");

    Aopt.push_back("+");
    Aopt.push_back("-");
    Mopt.push_back("*");
    Mopt.push_back("/");
    Ropt.push_back(":=");
    Ropt.push_back("=");
    Ropt.push_back("<>");
    Ropt.push_back("<");
    Ropt.push_back("<=");
    Ropt.push_back(">");
    Ropt.push_back(">=");
    Ropt.push_back(":");

    boundWord.push_back('(');
    boundWord.push_back(')');
    boundWord.push_back(';');
    boundWord.push_back(',');

}

Lexer::~Lexer() {
    file.close();
}

Token Lexer::getPeekChar() {
    string type;
    char str[2] = { file.peek(),0 };
    type.append(str);
    return Token(type, "-", line);
}

// 读取下一个字符
bool Lexer::readNextChar() {
    if (!file.get(currentChar)) {
        currentChar = '$';
    }
    else if (currentChar == '\n') {
        line++;
    }
    return true;
}

// 跳过空白字符
void Lexer::skipWhitespace() {
    while (isspace(currentChar) && readNextChar()) {    }
}

bool Lexer::isKeyword(const string& s) {
    vector<string>::iterator result = find(keyWord.begin(), keyWord.end(), s);

    if (result != keyWord.end()) {
        return true;
    }
    return false;
}

bool Lexer::isAopt(const string& s) {
    vector<string>::iterator result = find(Aopt.begin(), Aopt.end(), s);

    if (result != Aopt.end()) {
        return true;
    }
    return false;
}

bool Lexer::isMopt(const string& s) {
    vector<string>::iterator result = find(Mopt.begin(), Mopt.end(), s);

    if (result != Mopt.end()) {
        return true;
    }
    return false;
}

bool Lexer::isRopt(const string& s) {
    vector<string>::iterator result = find(Ropt.begin(), Ropt.end(), s);

    if (result != Ropt.end()) {
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
        return Token(identifier, "-",line);
    }
    else {
        return Token("ID", identifier, line);
    }
}

// 扫描数字
Token Lexer::scanNumber() {
    string number;
    while (isdigit(currentChar)) {
        number += currentChar;
        readNextChar();
    }
    return Token("INT", number, line);
}

// 扫描加法运算符
Token Lexer::scanAopt() {
    string symbol(1, currentChar);
    readNextChar();
    return Token("AOP", symbol, line);
}
// 扫描乘法运算符
Token Lexer::scanMopt() {
    string symbol(1, currentChar);
    readNextChar();
    return Token("MOP", symbol, line);
}
// 扫描逻辑运算符
Token Lexer::scanRopt() {
    string symbol(1, currentChar);
    readNextChar();

    // 检查是否是多字符运算符
    if (currentChar == '=' || currentChar == '>') {
        symbol += currentChar;
        if (currentChar == '=') {
            readNextChar();
            return Token(symbol, "-", line);
        }
        readNextChar();
    }

    return Token("ROP", symbol, line);
}

// 扫描界符
Token Lexer::scanBound() {
    string symbol(1, currentChar);
    readNextChar();

    return Token(symbol, "-", line);
}

// 获取下一个Token
Token Lexer::getNextToken() {

    skipWhitespace();
    if (currentChar == '$') {
        return Token("$", "-", line);
    }

    if (isalpha(currentChar)) {
        return scanIdentifier();
    }
    else if (isdigit(currentChar)) {
        return scanNumber();
    }
    else if (isBound(currentChar)) {
        return scanBound();
    }
    else if (isAopt(string(1, currentChar))) {
        return scanAopt();
    }
    else if (isMopt(string(1, currentChar))) {
        return scanMopt();
    }
    else if (isRopt(string(1, currentChar))) {
        return scanRopt();
    }
    else {
        // 无法识别的字符
        cerr << "error token: " << currentChar << endl;
        exit(1);
    }
}
