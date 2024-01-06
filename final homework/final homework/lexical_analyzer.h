#pragma once
#include<string>
#include<vector>
#include <fstream>
#include<list>

using namespace std;

struct Token {
    string type;
    string value;
    int line;
    Token(string Type, string Value,int Line) {
        type = Type;
        value = Value;
        line = Line;
    }
    Token() {

    }
};
class Lexer {
public:
    Lexer(const std::string& filename);
    ~Lexer();
    Token getNextToken();

private:

    string name;
    ifstream file;
    char currentChar;
    // 当前所在行
    int line;

    vector<string> keyWord;
    vector<string> Aopt;
    vector<string> Mopt;
    vector<string> Ropt;
    vector<char> boundWord;

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

    Token getPeekChar();
};