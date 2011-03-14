#include "TokenStack.h"
#include "Token.h"

namespace ExcelFormula
{
	void TokenStack::push(Token* pToken)
	{
		m_stack.push(pToken);
	}

	Token* TokenStack::pop()
	{
		if (m_stack.empty())
		{
			return NULL;
		}
		else
		{
			
			Token* efToken = m_stack.top();
			m_stack.pop();
			efToken->setSubtype(Token::Stop);
			return efToken;
			
			/*
			Token* topElement = m_stack.top();
			m_stack.pop();
			return topElement ;
			*/
		}
	}

	Token* TokenStack::getCurrent()
	{
		return m_stack.empty()?NULL:m_stack.top();
	}

};



