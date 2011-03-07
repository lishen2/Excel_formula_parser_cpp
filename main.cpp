#include "FormulaParser.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using ExcelFormula::Token;
using ExcelFormula::FormulaParser;

int main(int argc, char* argv[])
{
	const char szFormula[] = "=1+3+5";
	FormulaParser parser1(szFormula);
	parser1.parserToToken1();
	vector<Token*> tokens = parser1.getTokens();

	cout << "Formual:" << szFormula << endl;
	cout << "Tokens:" << endl;
	
	for(vector<Token*>::iterator it = tokens.begin();
			it != tokens.end();
			++it)
	{
		cout << (*it)->getPrintableString() << endl;
	}

	cout << endl;

	return 0;
}
