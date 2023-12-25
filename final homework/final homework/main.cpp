#include<iostream>
#include <fstream>
#include <string>
#include "lexical_analyzer.h"

using namespace std;

int main() {
	string filename;
	string line;
	//请输入文件名
	//cout << "please input file name" << endl;
	//cin >> filename;
	
	//open file
	Lexer lexer(".\\test.txt");
	Token token;
	do {
		token = lexer.getNextToken();
		std::cout << "类型: " << token.type << ", 值: " << token.value << std::endl;
	} while (token.type != "END");
	//close file
}