#include "ExcelFormulaToken.h"
#include <vector>
#include <string>
#include <stack>

using std::vector;
using std::string;
using std::stack;

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


	class ExcelFormulaTokens{
		public:
			ExcelFormulaTokens()
				:m_index(0)
			{}

			size_t size() {return m_tokens.size();}

			bool isBOF() {return (m_index <= 0)?true:false;}

			bool isEOF() {return (m_index >= m_tokens.size())?true:false;}

			ExcelFormulaToken getCurrent()
			{
				if (m_index >= m_tokens.size())
				{
					return ExcelFormulaToken();
				}
				else
				{
					return m_tokens[m_index];
				}
			}

			ExcelFormulaToken getNext()
			{
				if (isEOF())
				{
					return ExcelFormulaToken();
				}
				else
				{
					return m_tokens[m_index + 1];
				}
			}

			ExcelFormulaToken getPrevious()
			{
				if (isBOF())
				{
					return ExcelFormulaToken();
				}
				else
				{
					return m_tokens[m_index - 1];
				}
			}

			ExcelFormulaToken add(ExcelFormulaToken token)
			{
				m_tokens.push_back(token);
				return token;
			}

			bool moveNext()
			{
				if (isEOF()) return false;
				++m_index;
				return true;
			}

			void Reset()
			{
				m_index = 0;
			}

		private:
			size_t m_index;
			vector<ExcelFormulaToken> m_tokens;
	};


	class ExcelFormulaStack
	{
		public:
			void push(ExcelFormulaToken& token)
			{
				m_stack.push(token);
			}

			ExcelFormulaToken pop()
			{
				if (m_stack.empty())
				{
					return ExcelFormulaToken();
				}
				else
				{
					ExcelFormulaToken efToken = m_stack.top();
					string emptyStr;
					efToken.setValue(emptyStr);
					efToken.setSubtype(ExcelFormulaToken::Stop);
					m_stack.pop();
					return efToken;
				}
			}

			ExcelFormulaToken getCurrent()
			{
				m_stack.empty()?ExcelFormulaToken():m_stack.top();
			}

		private:
			stack<ExcelFormulaToken> m_stack;
	};

} //namespace 

