#include "Token.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace pcrecpp
{
	class RE;
}
using pcrecpp::RE;

namespace ExcelFormula
{
	class FormulaParser
	{
		public:
			FormulaParser(const char* szFormula);

			/* scan formula and translate it 
			* into tokens literally
			*/
			void parserToToken1();


			void parserToToken2();

			const char* getFormula(){return m_formula.c_str();}

			size_t getSize(){return m_tokens.size();}

			vector<Token*>& getTokens()
			{
				return m_tokens;
			}

		private:
			FormulaParser();

			string m_formula;
			
			vector<Token*> m_tokens;

			RE* m_regex;
	
	}; //class ExcelFormula


} //namespace 

