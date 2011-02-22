#include "ExcelFormula.h"
#include "StrUtils.h"
#include <vector>
#include <string> 
#include <stack>
#include "pcrecpp.h"

using std::vector;
using std::string;
using std::stack;

namespace ExcelFormulaParser
{
	class ExcelFormulaTokens{
		public:
			//			ExcelFormulaTokens():
			//			m_index(0){}
			//
			//			~ExcelFormulaTokens()
			//			{
			//				for(vector<ExcelFormulaToken*>::itarator it
			//					   	= m_tokens.begin();
			//						it != m_tokens.end();
			//						++it)
			//				{
			//					delete *it;
			//				}
			//
			//				m_tokens.clear();
			//			}

			size_t size() {return m_tokens.size();}

			bool isBOF() {return (m_index <= 0)?true:false;}

			bool isEOF() {return (m_index >= m_tokens.size() - 1)?true:false;}

			ExcelFormulaToken getCurrect()
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
			void push(ExcelFormulaToken token)
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



	//ExcelFormula class implement
	ExcelFormula::ExcelFormula(string& formula)
		:m_regex(new RE("^[1-9]{1}(\\.[0-9]+)?E{1}$"))
		{
			m_formula = StrUtils::trim(formula);
		}

	void ExcelFormula::parserToToken1()
	{
		if (m_formula.size() < 2 
				|| m_formula[0] != '=') return;

		ExcelFormulaTokens tokens1;
		ExcelFormulaStack stack;

		const char QUOTE_DOUBLE  = '"';
		const char QUOTE_SINGLE  = '\'';
		const char BRACKET_CLOSE = ']';
		const char BRACKET_OPEN  = '[';
		const char BRACE_OPEN    = '{';
		const char BRACE_CLOSE   = '}';
		const char PAREN_OPEN    = '(';
		const char PAREN_CLOSE   = ')';
		const char SEMICOLON     = ';';
		const char WHITESPACE    = ' ';
		const char COMMA         = ',';
		const char ERROR_START   = '#';

		const string OPERATORS_SN = "+-";
		const string OPERATORS_INFIX = "+-*/^&=><";
		const string OPERATORS_POSTFIX = "%";

		const char* ERRORS[] = {"#NULL!", "#DIV/0!", "#VALUE!", "#REF!", "#NAME?", "#NUM!", "#N/A"};
		const int ERRORS_LEN = 7; 

		const char* COMPARATORS_MULTI[] = {">=", "<=", "<>"};
		const int COMPARATORS_MULTI_LEN = 3;

		bool inString = false;
		bool inPath = false;
		bool inRange = false;
		bool inError = false;

		int index = 1;
		string value = "";      

		while(index < m_formula.size())
		{
			// state-dependent character evaluation (order is important)

			// double-quoted strings
			// embeds are doubled
			// end marks token

			if (inString) {    
				if (m_formula[index] == QUOTE_DOUBLE) {
					if (((index + 2) <= m_formula.size()) && (m_formula[index + 1] == QUOTE_DOUBLE)) {
						value += QUOTE_DOUBLE;
						index++;
					} else {
						inString = false;
						tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand, ExcelFormulaToken::Text));
						value = "";
					}      
				} else {
					value += m_formula[index];
				}
				index++;
				continue;
			}

			// single-quoted strings (links)
			// embeds are double
			// end does not mark a token

			if (inPath) {
				if (m_formula[index] == QUOTE_SINGLE) {
					if (((index + 2) <= m_formula.size()) && (m_formula[index + 1] == QUOTE_SINGLE)) {
						value += QUOTE_SINGLE;
						index++;
					} else {
						inPath = false;
					}
				} else {
					value += m_formula[index];
				}
				index++;
				continue; 
			}    

			// bracked strings (R1C1 range index or linked workbook name)
			// no embeds (changed to "()" by Excel)
			// end does not mark a token

			if (inRange) {
				if (m_formula[index] == BRACKET_CLOSE) {
					inRange = false;
				}
				value += m_formula[index];
				index++;
				continue;
			}

			// error values
			// end marks a token, determined from absolute list of values

			if (inError) {
				value += m_formula[index];
				index++;
				if (StrUtils::indexOf(ERRORS_LEN, ERRORS, value.c_str()) != -1) {
					inError = false;
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand, ExcelFormulaToken::Error));
					value = "";
				}
				continue;
			}

			// scientific notation check

			if ((OPERATORS_SN).find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 1) {
					if (m_regex->PartialMatch(value.c_str())) {
						value+= m_formula[index];
						index++;
						continue;
					}
				}
			}

			// independent character evaluation (order not important)

			// establish state-dependent character evaluations

			if (m_formula[index] == QUOTE_DOUBLE) {  
				if (value.size() > 0) {  // unexpected
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Unknown));
					value = "";
				}
				inString = true;
				index++;
				continue;
			}

			if (m_formula[index] == QUOTE_SINGLE) {
				if (value.size() > 0) { // unexpected
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Unknown));
					value = "";
				}
				inPath = true;
				index++;
				continue;
			}

			if (m_formula[index] == BRACKET_OPEN) {
				inRange = true;
				value += BRACKET_OPEN;
				index++;
				continue;
			}

			if (m_formula[index] == ERROR_START) {
				if (value.size() > 0) { // unexpected
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Unknown));
					value = "";
				}
				inError = true;
				value += ERROR_START;
				index++;
				continue;
			}

			// mark start and end of arrays and array rows

			if (m_formula[index] == BRACE_OPEN) {  
				if (value.size() > 0) { // unexpected
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Unknown));
					value = "";
				}
				stack.push(tokens1.add(ExcelFormulaToken("ARRAY", ExcelFormulaToken::Function, ExcelFormulaToken::Start)));
				stack.push(tokens1.add(ExcelFormulaToken("ARRAYROW", ExcelFormulaToken::Function, ExcelFormulaToken::Start)));
				index++;
				continue;
			}

			if (m_formula[index] == SEMICOLON) {  
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				tokens1.add(stack.pop());
				tokens1.add(ExcelFormulaToken(",", ExcelFormulaToken::Argument));
				stack.push(tokens1.add(ExcelFormulaToken("ARRAYROW", ExcelFormulaToken::Function, ExcelFormulaToken::Start)));
				index++;
				continue;
			}

			if (m_formula[index] == BRACE_CLOSE) {  
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				tokens1.add(stack.pop());
				tokens1.add(stack.pop());
				index++;
				continue;
			}

			// trim white-space

			if (m_formula[index] == WHITESPACE) {
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				tokens1.add(ExcelFormulaToken("", ExcelFormulaToken::Whitespace));
				index++;
				while ((m_formula[index] == WHITESPACE) && (index < m_formula.size())) { 
					index++;
				}
				continue;
			}

			// multi-character comparators

			if ((index + 2) <= m_formula.size()) {
				if (StrUtils::indexOf(COMPARATORS_MULTI_LEN, COMPARATORS_MULTI, m_formula.substr(index, 2).c_str()) != -1) {
					if (value.size() > 0) {
						tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
						value = "";
					}
					tokens1.add(ExcelFormulaToken(m_formula.substr(index, 2).c_str(), ExcelFormulaToken::OperatorInfix, ExcelFormulaToken::Logical));
					index += 2;
					continue;     
				}
			}

			// standard infix operators

			if ((OPERATORS_INFIX).find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				tokens1.add(ExcelFormulaToken(string(&(m_formula[index])), ExcelFormulaToken::OperatorInfix));
				index++;
				continue;     
			}

			// standard postfix operators (only one)

			static string operatorsProtfixStr(OPERATORS_POSTFIX);
			if (operatorsProtfixStr.find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				tokens1.add(ExcelFormulaToken(string(&(m_formula[index])), ExcelFormulaToken::OperatorPostfix));
				index++;
				continue;     
			}

			// start subexpression or function

			if (m_formula[index] == PAREN_OPEN) {
				if (value.size() > 0) {
					stack.push(tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Function, ExcelFormulaToken::Start)));
					value = "";
				} else {
					stack.push(tokens1.add(ExcelFormulaToken("", ExcelFormulaToken::Subexpression, ExcelFormulaToken::Start)));
				}
				index++;
				continue;
			}

			// function, subexpression, or array parameters, or operand unions

			if (m_formula[index] == COMMA) {
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				if (stack.getCurrent().getType() != ExcelFormulaToken::Function) {
					tokens1.add(ExcelFormulaToken(",", ExcelFormulaToken::OperatorInfix, ExcelFormulaToken::Union));
				} else {
					tokens1.add(ExcelFormulaToken(",", ExcelFormulaToken::Argument));
				}
				index++;
				continue;
			}

			// stop subexpression

			if (m_formula[index] == PAREN_CLOSE) {
				if (value.size() > 0) {
					tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
					value = "";
				}
				tokens1.add(stack.pop());
				index++;
				continue;
			}

			// token accumulation

			value += m_formula[index];
			index++;

		}

		// dump remaining accumulation

		if (value.size() > 0) {
			tokens1.add(ExcelFormulaToken(value, ExcelFormulaToken::Operand));
		}


	}//function 

}

