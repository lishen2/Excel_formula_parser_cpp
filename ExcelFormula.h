#include "ExcelFormulaToken.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace pcrecpp
{
	class RE;
}
using pcrecpp::RE;

namespace ExcelFormulaParser
{
	class ExcelFormula
	{
		public:
			ExcelFormula(string& formula);

			// used to parser formula to token
			void parserToToken();

			string getFormula(){return m_formula;}
			size_t getSize(){return m_tokens.size();}

		private:
			ExcelFormula();

			string m_formula;
			
			vector<ExcelFormulaToken> m_tokens;

			RE* m_regex;
	
	}; //class ExcelFormula


} //namespace 

