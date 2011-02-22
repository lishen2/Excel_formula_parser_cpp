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

			/* scan formula and translate it 
			* into tokens literally
			*/
			void parserToToken1();


			void parserToToken2();

			string getFormula(){return m_formula;}
			size_t getSize(){return m_tokens.size();}

			vector<ExcelFormulaToken>& getTokens()
			{
				return m_tokens;
			}

		private:
			ExcelFormula();

			string m_formula;
			
			vector<ExcelFormulaToken> m_tokens;

			RE* m_regex;
	
	}; //class ExcelFormula


} //namespace 

