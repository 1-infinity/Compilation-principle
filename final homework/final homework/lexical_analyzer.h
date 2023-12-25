#pragma once
#include<string>
#include<vector>
#include <fstream>

using namespace std;

struct Token {
	string type;
	string value;
};
class Lexer {
public:
    Lexer(const std::string& filename);
    Token getNextToken();
    vector<string> keyWord = { "PROGRAM","BEGIN","END","CONST","VAR","WHILE","DO","IF","THEN" };
    vector<string> opt = { "+", "-", "*", "/",":=" ,"=", "<>", "<", "<=", ">", ">=" , ":"};
    vector<char> boundWord = { '(',')',';' , ',' };
private:
    string name;
    ifstream file;
    char currentChar;
    Token currentToken;

    void readNextChar();
    void skipWhitespace();
    bool isKeyword(const string& s);
    bool isOpt(const string& s);
    bool isBound(const char s);
    Token scanIdentifier();
    Token scanNumber();
    Token scanOpt();
    Token scanBound();

};
