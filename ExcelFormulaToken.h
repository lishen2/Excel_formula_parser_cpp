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

			bool operator==(ExcelFormulaToken& token) {
				if (token.getType() == getType()
					&& token.getSubtype() == getSubtype()
					&& token.getValue() == getValue())
					return true;
				else
					return false;
			}

			string& getValue() {return m_value;} 
			void setValue(string& value) {
				m_value = value;
				m_printableStr.clear();
			}

			ExcelFormulaTokenType getType() {return m_type;}
			void setType(ExcelFormulaTokenType type) {
				m_type = type;
				m_printableStr.clear();
			}

			ExcelFormulaTokenSubtype getSubtype() {return m_subtype;}
			void setSubtype(ExcelFormulaTokenSubtype subtype){
				m_subtype = subtype; 
				m_printableStr.clear();
			}

			//! return printable string
			string& getPrintableString()
			{
				if(m_printableStr.size() == 0)
				{
					m_printableStr = m_value + " ";
					
					switch(m_type)
					{
						case Operand:
							m_printableStr += "<Operand>";
							break;
						case Function:
							m_printableStr += "<Function>";
							break;
						case Subexpression:
							m_printableStr += "<Subexpression>";
							break;
						case Argument:
							m_printableStr += "<Argument>";
							break;
						case OperatorPrefix:
							m_printableStr += "<OperatorPrefix>";
							break;
						case OperatorInfix:
							m_printableStr += "<OperatorInfix>";
							break;
						case OperatorPostfix:
							m_printableStr += "<OperatorPostfix>";
							break;
						case Whitespace:
							m_printableStr += "<Whitespace>";
							break;
						case Unknown:
							m_printableStr += "<Unknown>";
							break;
					}//token type switch

					m_printableStr += " ";

					switch(m_subtype)
					{
						case Nothing:
							m_printableStr += "<Nothing>";
							break;
						case Start:
							m_printableStr += "<Start>";
							break;
						case Stop:
							m_printableStr += "<Stop>";
							break;
						case Text:
							m_printableStr += "<Text>";
							break;
						case Number:
							m_printableStr += "<Number>";
							break;
						case Logical:
							m_printableStr += "<Logical>";
							break;
						case Error:
							m_printableStr += "<Error>";
							break;
						case Range:
							m_printableStr += "<Range>";
							break;
						case Math:
							m_printableStr += "<Math>";
							break;
						case Concatenation:
							m_printableStr += "<Concatenation>";
							break;
						case Intersection:
							m_printableStr += "<Intersection>";
							break;
						case Union:
							m_printableStr += "<Union>";
							break;
					}//token subtype switch
				}
				return m_printableStr;
			}

		private:

			string m_value;
			string m_printableStr; //string used to print
			ExcelFormulaTokenType m_type;
			ExcelFormulaTokenSubtype m_subtype;
	}; //class ExcelFormulaToken
};

#endif


