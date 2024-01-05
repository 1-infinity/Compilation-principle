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

	//close file
}