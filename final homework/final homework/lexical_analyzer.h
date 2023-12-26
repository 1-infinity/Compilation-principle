#pragma once
#include<string>
#include<vector>
#include <fstream>

using namespace std;

struct Token {
	string type;
	string value;
    Token(string Type, string Value) {
        type = Type;
        value = Value;
    }
    Token(){

    }
};
class Lexer {
public:
    Lexer(const std::string& filename);
    ~Lexer();
    Token getNextToken();
    vector<string> keyWord;
    vector<string> opt;
    vector<char> boundWord;
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
