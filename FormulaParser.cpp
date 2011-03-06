#include "FormulaParser.h"
#include "TokenStack.h"
#include "TokenArray.h"
#include "StrUtils.h"
#include "pcrecpp.h"

#define MakeToken TokenAllocer::getToken

namespace ExcelFormula
{

	//ExcelFormula class implement
	FormulaParser::FormulaParser(const char* szFormula)
		:m_regex(new RE("^[1-9]{1}(\\.[0-9]+)?E{1}$"))
		{
			m_formula = szFormula;
			StrUtils::trim(m_formula);
		}

	void FormulaParser::parserToToken1()
	{
		if (m_formula.size() < 2 
				|| m_formula[0] != '=') return;

		TokenArray tokens1;
		TokenStack stack;

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
						tokens1.add(MakeToken(value.c_str(), Token::Operand, Token::Text));
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
					tokens1.add(MakeToken(value.c_str(), Token::Operand, Token::Error));
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
					tokens1.add(MakeToken(value.c_str(), Token::Unknown));
					value = "";
				}
				inString = true;
				index++;
				continue;
			}

			if (m_formula[index] == QUOTE_SINGLE) {
				if (value.size() > 0) { // unexpected
					tokens1.add(MakeToken(value.c_str(), Token::Unknown));
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
					tokens1.add(MakeToken(value.c_str(), Token::Unknown));
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
					tokens1.add(MakeToken(value.c_str(), Token::Unknown));
					value = "";
				}
				stack.push(tokens1.add(MakeToken("ARRAY", Token::Function, Token::Start)));
				stack.push(tokens1.add(MakeToken("ARRAYROW", Token::Function, Token::Start)));
				index++;
				continue;
			}

			if (m_formula[index] == SEMICOLON) {  
				if (value.size() > 0) {
					tokens1.add(Token(value, Token::Operand));
					value = "";
				}
				tokens1.add(stack.pop());
				tokens1.add(MakeToken(",", Token::Argument));
				stack.push(tokens1.add(MakeToken("ARRAYROW", Token::Function, Token::Start)));
				index++;
				continue;
			}

			if (m_formula[index] == BRACE_CLOSE) {  
				if (value.size() > 0) {
					tokens1.add(MakeToken(value.c_str(), Token::Operand));
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
					tokens1.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				tokens1.add(Token("", Token::Whitespace));
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
						tokens1.add(MakeToken(value.c_str(), Token::Operand));
						value = "";
					}
					tokens1.add(MakeToken(m_formula.substr(index, 2).c_str(), Token::OperatorInfix, Token::Logical));
					index += 2;
					continue;     
				}
			}

			// standard infix operators

			if ((OPERATORS_INFIX).find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 0) {
					tokens1.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				char buf[2] = {0};
				buf[0] = m_formula[index];
				tokens1.add(MakeToken(buf, Token::OperatorInfix));
				index++;
				continue;     
			}

			// standard postfix operators (only one)

			static string operatorsProtfixStr(OPERATORS_POSTFIX);
			if (operatorsProtfixStr.find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 0) {
					tokens1.add(Token(value, Token::Operand));
					value = "";
				}
				tokens1.add(Token(string(&(m_formula[index])), Token::OperatorPostfix));
				index++;
				continue;     
			}

			// start subexpression or function

			if (m_formula[index] == PAREN_OPEN) {
				if (value.size() > 0) {
					stack.push(tokens1.add(Token(value, Token::Function, Token::Start)));
					value = "";
				} else {
					stack.push(tokens1.add(Token("", Token::Subexpression, Token::Start)));
				}
				index++;
				continue;
			}

			// function, subexpression, or array parameters, or operand unions

			if (m_formula[index] == COMMA) {
				if (value.size() > 0) {
					tokens1.add(Token(value, Token::Operand));
					value = "";
				}
				if (stack.getCurrent().getType() != Token::Function) {
					tokens1.add(Token(",", Token::OperatorInfix, Token::Union));
				} else {
					tokens1.add(Token(",", Token::Argument));
				}
				index++;
				continue;
			}

			// stop subexpression

			if (m_formula[index] == PAREN_CLOSE) {
				if (value.size() > 0) {
					tokens1.add(Token(value, Token::Operand));
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
			tokens1.add(Token(value, Token::Operand));
		}


	}//function 

}

