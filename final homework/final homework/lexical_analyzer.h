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

    //生成所有的token
    list<Token> tokens;
    // 获取当前的token
    Token getToken();
    // 获得到下一个token
    Token nextToken();
    // 查看下一个token
    Token getNext();

private:
    string name;
    ifstream file;
    char currentChar;
    Token currentToken;
    vector<string> keyWord;
    vector<string> Aopt;
    vector<string> Mopt;
    vector<string> Ropt;
    vector<char> boundWord;

    list<Token>::iterator it;

    bool readNextChar();
    void skipWhitespace();
    bool isKeyword(const string& s);
    //是否为加法运算符
    bool isAopt(const string& s);
    //是否为乘法运算符
    bool isMopt(const string& s);
    //是否为逻辑运算符
    bool isRopt(const string& s);
    bool isBound(const char s);
    Token scanIdentifier();
    Token scanNumber();

    Token scanAopt();
    Token scanMopt();
    Token scanRopt();

    Token scanBound();
    list<Token> generateTokens();

    Token getNextToken();
    Token getPeekChar();
};