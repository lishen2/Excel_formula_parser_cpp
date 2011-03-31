#include "TokenArray.h"
#include "Token.h"

namespace ExcelFormula{

	TokenArray::TokenArray()
		:m_index(0)
	{}

	Token* TokenArray::getCurrent()
	{
		if (m_index >= m_tokens.size() || m_index < 0)
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

	void TokenArray::toVector(vector<Token*>& vecToken)
	{
		vecToken = m_tokens;
	}

	void TokenArray::fromVector(vector<Token*>& vecToken)
	{
		m_tokens = vecToken;
	}

	void TokenArray::releaseAll()
	{
		for(vector<Token*>::const_iterator it = m_tokens.begin();
				it != m_tokens.end();
				++it)
		{
			delete (*it);
		}
		m_tokens.clear();
	}
}


