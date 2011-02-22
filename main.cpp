#include "ExcelFormula.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using ExcelFormulaParser::ExcelFormulaToken;
using ExcelFormulaParser::ExcelFormula;

int main(int argc, char* argv[])
{
	string formula1("=1+3+5");
	ExcelFormulaParser::ExcelFormula parser1(formula1);
	parser1.parserToToken1();
	vector<ExcelFormulaToken> tokens = parser1.getTokens();

	cout << "Formual:" << formula1 << endl;
	cout << "Tokens:";
	
	for(vector<ExcelFormulaToken>::iterator it = tokens.begin();
			it != tokens.end();
			++it)
	{
		cout << it->getPrintableString();
	}

	cout << endl;

	return 0;
}
