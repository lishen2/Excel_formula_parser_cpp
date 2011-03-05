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
			ExcelFormulaToken();

			ExcelFormulaToken(const char* szValue, ExcelFormulaTokenType type);

			ExcelFormulaToken(const char* szValue, ExcelFormulaTokenType type, ExcelFormulaTokenSubtype subtype);

			bool operator==(ExcelFormulaToken& token);

			string& getValue() {return m_value;} 
			void setValue(string& value) {
				m_value = value;
			}

			ExcelFormulaTokenType getType() {return m_type;}
			void setType(ExcelFormulaTokenType type) {
				m_type = type;
			}

			ExcelFormulaTokenSubtype getSubtype() {return m_subtype;}
			void setSubtype(ExcelFormulaTokenSubtype subtype){
				m_subtype = subtype; 
			}

			//! return printable string
			string getPrintableString();
		private:

			string m_value;
			//string m_printableStr; //string used to print
			ExcelFormulaTokenType m_type;
			ExcelFormulaTokenSubtype m_subtype;
	}; //class ExcelFormulaToken
};

#endif


