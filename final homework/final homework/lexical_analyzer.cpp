#include "lexical_analyzer.h"
#include <cctype>
#include<iostream>
#include <algorithm>

// ���캯��
Lexer::Lexer(const std::string& filename) : file(filename), currentChar(' ') {
    if (!file.is_open()) {
        std::cerr << "�޷����ļ� " << filename << std::endl;
        exit(1);
    }
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
        return { identifier, "-"};
    }
    else {
        return { "ID", identifier};
    }
}

// ɨ������
Token Lexer::scanNumber() {
    string number;
    while (isdigit(currentChar)) {
        number += currentChar;
        readNextChar();
    }
    return { "INT", number};
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

    return { symbol, "-" };
}

// ɨ����
Token Lexer::scanBound() {
    string symbol(1, currentChar);
    readNextChar();

    return { symbol, "-" };
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
        return { "ENDFILE", "-" };
    }
    else {
        // �޷�ʶ����ַ�
        cerr << "�޷�ʶ����ַ�: " << currentChar << endl;
        exit(1);
    }
}

