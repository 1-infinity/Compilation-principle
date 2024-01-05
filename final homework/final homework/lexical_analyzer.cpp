#include "lexical_analyzer.h"
#include <cctype>
#include<iostream>
#include <algorithm>

// ���캯��
Lexer::Lexer(const std::string& filename) : currentChar(' ') {
    file.open(filename, ios::in);
    if (!file.is_open()) {
        std::cerr << "Cannot open file! " << filename << std::endl;
        exit(1);
    }
    flag = false;
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

    generateTokens();
    it = tokens.begin();
}

Lexer::~Lexer() {
    file.close();
}

Token Lexer::getPeekChar() {
    string type;
    char str[2] = { file.peek(),0 };
    type.append(str);
    return Token(type, "-");
}

// ��ȡ��һ���ַ�
bool Lexer::readNextChar() {
    if (file.get(currentChar)) {
        return true;
    }
    else {
        currentChar = '$';
    }
    return true;
}

// �����հ��ַ�
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

// ɨ���ʶ��
Token Lexer::scanIdentifier() {
    string identifier;
    bool flag = true;
    while (isalnum(currentChar) && flag) {
        identifier += currentChar;
        flag = readNextChar();
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

// ɨ��ӷ������
Token Lexer::scanAopt() {
    string symbol(1, currentChar);
    readNextChar();
    return Token("AOP", symbol);
}
// ɨ��˷������
Token Lexer::scanMopt() {
    string symbol(1, currentChar);
    readNextChar();
    return Token("MOP", symbol);
}
// ɨ���߼������
Token Lexer::scanRopt() {
    string symbol(1, currentChar);
    readNextChar();

    // ����Ƿ��Ƕ��ַ������
    if (currentChar == '=' || currentChar == '>') {
        symbol += currentChar;
        if (currentChar == '=') {
            readNextChar();
            return Token(symbol, "-");
        }
        readNextChar();
    }

    return Token("ROP", symbol);
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
    else if (isAopt(string(1, currentChar))) {
        return scanAopt();
    }
    else if (isMopt(string(1, currentChar))) {
        return scanMopt();
    }
    else if (isRopt(string(1, currentChar))) {
        return scanRopt();
    }
    else if (currentChar == '$') {
        return Token("$", "-");
    }
    else {
        // �޷�ʶ����ַ�
        cerr << "error token: " << currentChar << endl;
        exit(1);
    }
}

list<Token> Lexer::generateTokens() {
    Token token;
    do {
        token = getNextToken();
        tokens.push_back(token);
    } while (token.type != "$");
    return tokens;
}

Token Lexer::getToken() {
    return *it;
}

Token Lexer::nextToken() {
    return *next(it);
}

Token Lexer::getNext() {
    ++it;
    return *it;
}