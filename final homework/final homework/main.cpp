#include<iostream>
#include <fstream>
#include <string>
#include "lexical_analyzer.h"
#include "parser.hpp"

using namespace std;

int main() {
	string filename;
	string line;

	Parser parser("F:\\2152592\\homework\\3-1\\编译原理\\期末项目\\Compilation-principle\\test.txt");
	parser.ParserAndCodeGenerator();
	//close file
}