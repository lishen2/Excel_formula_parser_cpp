#include "FormulaParser.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using ExcelFormula::Token;
using ExcelFormula::FormulaParser;

void printIntend(int i)
{
	for(int j = 0; j < i; ++j)
	{
		cout << "|";
	}
}


void printParser(FormulaParser& parser)
{
	vector<Token*> tokens = parser.getTokens();
	cout << "Formual:" << parser.getFormula() << endl;
	cout << "Tokens:" << endl;
	
	int intend = 0;
	for(vector<Token*>::iterator it = tokens.begin();
			it != tokens.end();
			++it)
	{
		if((*it)->getSubtype() == Token::Start)
		{
			++intend;
		}
		printIntend(intend);
		cout << (*it)->getPrintableString() << endl;
		if((*it)->getSubtype() == Token::Stop)
		{
			--intend;
		}
	}

	cout << endl;
}


int main(int argc, char* argv[])
{
	const char szFormula1[] = "=1+3+5";
	FormulaParser parser1(szFormula1);
	parser1.parserToToken1();
	parser1.parserToToken2();
	printParser(parser1);

	
	const char szFormula2[] = "=IF(R13C3>DATE(2002,1,6),0,IF(ISERROR(R[41]C[2]),0,IF(R13C3>=R[41]C[2],0, IF(AND(R[23]C[11]>=55,R[24]C[11]>=20),R53C3,0))))";
	FormulaParser parser2(szFormula2);
	parser2.parserToToken1();
	parser2.parserToToken2();
	printParser(parser2);

	return 0;
}
