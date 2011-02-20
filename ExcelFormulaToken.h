#ifndef _EXCELFORMULAPARSER_H__
#define _EXCELFORMULAPARSER_H__

#include <string>

using std::string;

namespace ExcelFormulaParser
{
	class ExcelFormulaToken{

		public:
			enum ExcelFormulaTokenType 
			{
				Operand,
				Function,
				Subexpression,
				Argument,
				OperatorPrefix,
				OperatorInfix,
				OperatorPostfix,
				Whitespace,
				Unknown
			};

			enum ExcelFormulaTokenSubtype 
			{
				Nothing,
				Start,
				Stop,
				Text,
				Number,
				Logical,
				Error,
				Range,
				Math,
				Concatenation,
				Intersection,
				Union
			};

		public:
			ExcelFormulaToken():
				m_value(),
				m_type(Unknown),
				m_subtype(Nothing){}

			ExcelFormulaToken(string value, ExcelFormulaTokenType type): m_value(value),
			m_type(type),
			m_subtype(Nothing) {}


			ExcelFormulaToken(string value, ExcelFormulaTokenType type, ExcelFormulaTokenSubtype subtype):
				m_value(value),
				m_type(type),
				m_subtype(subtype) {} 

			string& getValue() {return m_value;} 
			void setValue(string& value) {m_value = value;}

			ExcelFormulaTokenType getType() {return m_type;}
			void setType(ExcelFormulaTokenType type) {m_type = type;}

			ExcelFormulaTokenSubtype getSubtype() {return m_subtype;}
			void setSubtype(ExcelFormulaTokenSubtype subtype){m_subtype = subtype; }

		private:

			string m_value;
			ExcelFormulaTokenType m_type;
			ExcelFormulaTokenSubtype m_subtype;
	}; //class ExcelFormulaToken
};

#endif


