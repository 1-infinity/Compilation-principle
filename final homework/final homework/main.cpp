#include<iostream>
#include <fstream>
#include <string>
#include "lexical_analyzer.h"
#include "parser.hpp"

using namespace std;

void usage(const char* prompt)
{
	clog <<"Usage: " << prompt << " [output_file.txt] [input_file.txt]" << endl;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		usage(*argv);
		return -1;
	}
	
	string filename;
	string line;
	Parser parser(argv[2]);
	vector<Code> code = parser.ParserAndCodeGenerator();
	ofstream outputFile(argv[1]);
	if (!outputFile.is_open()) {
		cerr << "无法打开输出文件" << endl;
		return 1;
	}
	parser.printSymbol(outputFile);
	parser.printCode(outputFile);
	
	/*outputFile << setw(10) << "address" << setw(10) << "operator" << setw(10) << "arg1" << setw(10) << "arg2" << setw(10) << "result" << endl;
	for (int i = 0; i < code.size(); i++) {
		outputFile << code[i] << endl;
	}*/
	// 关闭文件
	outputFile.close();
	string cmd = "notepad ";
	cmd.append(argv[1]);
	system(cmd.c_str());

	return 0;
}