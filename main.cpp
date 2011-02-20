#include "ExcelFormula.h"
#include <string>

using std::string;


int main(int argc, char* argv[])
{
	string formula1("=1+3+5");
	ExcelFormulaParser::ExcelFormula parser1(formula1);
	//parser1.

	return 0;
}
