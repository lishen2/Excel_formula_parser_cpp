#include "StrUtils.h"
#include "string.h"

/******************************************
// StrUtil class implement
//const char StrUtil::m_empChar[] = {' ', '\t'};
//const int StrUtil::m_empLen = 2;
 ******************************************/

// StrUtil functions
string StrUtils::trim(string& value)
{
	//trim begin
	for(size_t i = 0; i < value.size(); ++i)
	{
		if (!isEmpChar(value[i]))
		{
			value.erase(0, i);
			break;
		}
	}

	//trim end
	for(size_t i = value.size() - 1;
			i >= 0; --i)
	{
		if (!isEmpChar(value[i]))
		{
			value.erase(i + 1);
			break;
		}
	}

	return value;
} //fn trim

bool StrUtils::isEmpChar(const char c)
{
	static const char empChar[] = {' ', '\t'};
	static const int empLen = 2;

	for(int i = 0; i < empLen; ++i)
	{
		if (c == empChar[i]) return true;
	}

	return false;
}

int StrUtils::indexOf(const int argc, const char* array[], const char* szStr)
{
	int retVal = -1;
	for(int i = 0; i < argc; ++i)
	{
		if (strcmp(array[i], szStr) == 0)
			return i;
	}
	return retVal;
}

