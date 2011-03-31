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

		//! keywords used in parsing excel formual
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
						m_tmpAry.add(MakeToken(value.c_str(), Token::Operand, Token::Text));
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
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand, Token::Error));
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
					m_tmpAry.add(MakeToken(value.c_str(), Token::Unknown));
					value = "";
				}
				inString = true;
				index++;
				continue;
			}

			if (m_formula[index] == QUOTE_SINGLE) {
				if (value.size() > 0) { // unexpected
					m_tmpAry.add(MakeToken(value.c_str(), Token::Unknown));
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
					m_tmpAry.add(MakeToken(value.c_str(), Token::Unknown));
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
					m_tmpAry.add(MakeToken(value.c_str(), Token::Unknown));
					value = "";
				}
				m_tmpStack.push(m_tmpAry.add(MakeToken("ARRAY", Token::Function, Token::Start)));
				m_tmpStack.push(m_tmpAry.add(MakeToken("ARRAYROW", Token::Function, Token::Start)));
				index++;
				continue;
			}

			if (m_formula[index] == SEMICOLON) {  
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				m_tmpAry.add(m_tmpStack.pop());
				m_tmpAry.add(MakeToken(",", Token::Argument));
				m_tmpStack.push(m_tmpAry.add(MakeToken("ARRAYROW", Token::Function, Token::Start)));
				index++;
				continue;
			}

			if (m_formula[index] == BRACE_CLOSE) {  
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				m_tmpAry.add(m_tmpStack.pop());
				m_tmpAry.add(m_tmpStack.pop());
				index++;
				continue;
			}

			// trim white-space

			if (m_formula[index] == WHITESPACE) {
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				m_tmpAry.add(MakeToken("", Token::Whitespace));
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
						m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
						value = "";
					}
					m_tmpAry.add(MakeToken(m_formula.substr(index, 2).c_str(), Token::OperatorInfix, Token::Logical));
					index += 2;
					continue;     
				}
			}

			// standard infix operators

			if ((OPERATORS_INFIX).find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				char buf[2] = {0};
				buf[0] = m_formula[index];
				m_tmpAry.add(MakeToken(buf, Token::OperatorInfix));
				index++;
				continue;     
			}

			// standard postfix operators (only one)

			static string operatorsProtfixStr(OPERATORS_POSTFIX);
			if (operatorsProtfixStr.find_first_of(m_formula[index]) != string::npos) {
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				char buf[2] = {0};
				buf[0] = m_formula[index];
				m_tmpAry.add(MakeToken(buf, Token::OperatorPostfix));
				index++;
				continue;     
			}

			// start subexpression or function

			if (m_formula[index] == PAREN_OPEN) {
				if (value.size() > 0) {
					m_tmpStack.push(m_tmpAry.add(MakeToken(value.c_str(), Token::Function, Token::Start)));
					value = "";
				} else {
					m_tmpStack.push(m_tmpAry.add(MakeToken("", Token::Subexpression, Token::Start)));
				}
				index++;
				continue;
			}

			// function, subexpression, or array parameters, or operand unions

			if (m_formula[index] == COMMA) {
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				if (m_tmpStack.getCurrent()->getType() != Token::Function) {
					m_tmpAry.add(MakeToken(",", Token::OperatorInfix, Token::Union));
				} else {
					m_tmpAry.add(MakeToken(",", Token::Argument));
				}
				index++;
				continue;
			}

			// stop subexpression

			if (m_formula[index] == PAREN_CLOSE) {
				if (value.size() > 0) {
					m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
					value = "";
				}
				m_tmpAry.add(m_tmpStack.pop());
				index++;
				continue;
			}

			// token accumulation

			value += m_formula[index];
			index++;

		}

		// dump remaining accumulation

		if (value.size() > 0) {
			m_tmpAry.add(MakeToken(value.c_str(), Token::Operand));
		}

		m_tmpAry.toVector(m_tokens);

	}//function  parserToToken1()

	void FormulaParser::parserToToken2()
	{
		// move tokenList to new set, excluding unnecessary white-space m_tmpAry and converting necessary ones to intersections

		TokenArray tmpAry2;
		m_tmpAry.reset();
		for(Token* pToken = m_tmpAry.getCurrent();
				!m_tmpAry.isEOF(); 
				m_tmpAry.moveNext(),pToken = m_tmpAry.getCurrent())
		{
			if (pToken == NULL) continue;

			if (pToken->getType() != Token::Whitespace) {
				tmpAry2.add(pToken->clone());
				continue;        
			}

			if (m_tmpAry.isBOF() || m_tmpAry.isEOF()) continue;

			Token* previous = m_tmpAry.getPrevious();

			if (previous == NULL) continue;

			if (!(
						((previous->getType() == Token::Function) && (previous->getSubtype() == Token::Stop)) || 
						((previous->getType() == Token::Subexpression) && (previous->getSubtype() == Token::Stop)) || 
						(previous->getType() == Token::Operand)
				 )
			   ) continue;

			Token* next = m_tmpAry.getNext();

			if (next == NULL) continue;

			if (!(
						((next->getType() == Token::Function) && (next->getSubtype() == Token::Start)) || 
						((next->getType() == Token::Subexpression) && (next->getSubtype() == Token::Start)) ||
						(next->getType() == Token::Operand)
				 )
			   ) continue;

			tmpAry2.add(MakeToken("", Token::OperatorInfix, Token::Intersection));
		}

		// move m_tmpAry to final list, switching infix "-" operators to prefix when appropriate, switching infix "+" operators 
		// to noop when appropriate, identifying operand and infix-operator subtypes, and pulling "@" from function names

		m_tmpAry.releaseAll();
		tmpAry2.reset();
		for(Token* pToken = tmpAry2.getCurrent();
				!tmpAry2.isEOF();
				tmpAry2.moveNext(), pToken = tmpAry2.getCurrent())
		{
			if (pToken == NULL) continue;

			Token* previous = tmpAry2.getPrevious();
			Token* next = tmpAry2.getNext();

			if ((pToken->getType() == Token::OperatorInfix) && (pToken->getStrValue().compare("-") == 0)) {
				if (tmpAry2.isBOF())
					pToken->setType(Token::OperatorPrefix);
				else if (
						((previous->getType() == Token::Function) && (previous->getSubtype() == Token::Stop)) || 
						((previous->getType() == Token::Subexpression) && (previous->getSubtype() == Token::Stop)) || 
						(previous->getType() == Token::OperatorPostfix) || 
						(previous->getType() == Token::Operand)
						)
					pToken->setSubtype(Token::Math);
				else
					pToken->setType(Token::OperatorPrefix);

				m_tmpAry.add(pToken);
				continue;          
			}

			if ((pToken->getType() == Token::OperatorInfix) && (pToken->getStrValue().compare("+") == 0)) {
				if (tmpAry2.isBOF())
					continue;
				else if (
						((previous->getType() == Token::Function) && (previous->getSubtype() == Token::Stop)) || 
						((previous->getType() == Token::Subexpression) && (previous->getSubtype() == Token::Stop)) || 
						(previous->getType() == Token::OperatorPostfix) || 
						(previous->getType() == Token::Operand)
						)
					pToken->setSubtype(Token::Math);
				else
					continue;

				m_tmpAry.add(pToken);
				continue;          
			}

			if ((pToken->getType() == Token::OperatorInfix) && (pToken->getSubtype() == Token::Nothing)) {
				static string marks("<>=");
				if (marks.find_first_of(*(pToken->getValue())) != string::npos) 
					pToken->setSubtype(Token::Logical);
				else if (pToken->getStrValue().compare("&") == 0)
					pToken->setSubtype(Token::Concatenation);
				else
					pToken->setSubtype(Token::Math);

				m_tmpAry.add(pToken);
				continue;
			}

			if ((pToken->getType() == Token::Operand) && (pToken->getSubtype() == Token::Nothing)) {
				/*          double d;
							bool isNumber = double.TryParse(pToken.Value, System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.CurrentCulture, out d);
							if (!isNumber)
							if ((token.Value == "TRUE") || (token.Value == "FALSE")) 
							token.Subtype = Token::Logical;
							else 
							token.Subtype = Token::Range;            
							else
							token.Subtype = Token::Number;          

							m_tmpAry.Add(token);
							continue;*/
			}

			if (pToken->getType() == Token::Function) {
				if (pToken->getStrValue().size() > 0) {
					if (*(pToken->getValue()) == '@') {
						pToken->setValue(pToken->getStrValue().substr(1).c_str());
					}
				}
			}

			m_tmpAry.add(pToken);
		}//while

		m_tmpAry.toVector(m_tokens);
	}

	void FormulaParser::clear()
	{
		for(vector<Token*>::const_iterator it = m_tokens.begin();
				it != m_tokens.end();
				++it)
		{
			delete *it;	
		}

		m_tokens.clear();
	}//clear()


}// namespace ExcelFormula

