#pragma once
#include<string>
#include<vector>
#include <fstream>
#include<list>

using namespace std;

struct Token {
    string type;
    string value;
    Token(string Type, string Value) {
        type = Type;
        value = Value;
    }
    Token() {

    }
};
class Lexer {
public:
    Lexer(const std::string& filename);
    ~Lexer();

    //�������е�token
    list<Token> tokens;
    // ��ȡ��ǰ��token
    Token getToken();
    // ��õ���һ��token
    Token nextToken();
    // �鿴��һ��token
    Token getNext();

private:
    string name;
    ifstream file;
    char currentChar;
    Token currentToken;
    vector<string> keyWord;
    vector<string> opt;
    vector<char> boundWord;

    list<Token>::iterator it;

    bool readNextChar();
    void skipWhitespace();
    bool isKeyword(const string& s);
    bool isOpt(const string& s);
    bool isBound(const char s);
    Token scanIdentifier();
    Token scanNumber();
    Token scanOpt();
    Token scanBound();
    list<Token> generateTokens();

    Token getNextToken();
    Token getPeekChar();
};