#ifndef _EXCEL_FORMULA_TOKEN_H
#define _EXCEL_FORMULA_TOKEN_H

#include <stack>
using std::stack;

namespace ExcelFormula
{
	class Token;
	
	class TokenStack
	{
		public:
			void push(Token*);

			Token* pop();

			Token* getCurrent();

		private:
			stack<Token*> m_stack;
	};

};



#endif




