#include<iostream>
#include <fstream>
#include <string>
#include "lexical_analyzer.h"
#include "parser.hpp"

using namespace std;

int main() {
	string filename;
	string line;
	Parser parser("test.txt");
	vector<Code> code = parser.ParserAndCodeGenerator();
	ofstream outputFile("output.txt");
	if (!outputFile.is_open()) {
		cerr << "无法打开输出文件" << endl;
		return 1;
	}
	outputFile << setw(10) << "address" << setw(10) << "operator" << setw(10) << "arg1" << setw(10) << "arg2" << setw(10) << "result" << endl;
	for (int i = 0; i < code.size(); i++) {
		outputFile << code[i] << endl;
	}
	// 关闭文件
	outputFile.close();

	return 0;
}