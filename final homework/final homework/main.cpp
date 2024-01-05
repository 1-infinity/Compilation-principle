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
	cout << setw(10) << "address" << setw(10) << "operator" << setw(10) << "arg1" << setw(10) << "arg2" << setw(10) << "result" << endl;
	for (int i = 0; i < code.size(); i++) {
		cout << code[i] << endl;
	}
	//close file
}