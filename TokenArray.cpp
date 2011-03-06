#include "TokenArray.h"

namespace ExcelFormula{

	TokenArray::TokenArray()
		:m_index(0)
	{}

	Token* TokenArray::getCurrent()
	{
		if (m_index >= m_tokens.size())
		{
			return NULL;
		}
		else
		{
			return m_tokens[m_index];
		}
	}

	Token* TokenArray::getNext()
	{
		if (isEOF())
		{
			return NULL;
		}
		else
		{
			return m_tokens[m_index + 1];
		}
	}

	Token* TokenArray::getPrevious()
	{
		if (isBOF())
		{
			return NULL;
		}
		else
		{
			return m_tokens[m_index - 1];
		}
	}

	Token* TokenArray::add(Token* pToken)
	{
		m_tokens.push_back(pToken);
		return pToken;
	}

	bool TokenArray::moveNext()
	{
		if (isEOF()) return false;
		++m_index;
		return true;
	}

}


