#ifndef _EXCELFORMULAPARSER_H__
#define _EXCELFORMULAPARSER_H__

#include <string>

using std::string;

namespace ExcelFormula
{
	class Token{

		public:
			enum TokenType 
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

			enum TokenSubtype 
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
			/**
			 * Default constructor, generate a empty token
			 */
			Token();

			/**
			 * @param const char* string value of the token
			 * @param TokenType type of the token
			 */
			Token(const char*, TokenType);
			
			/**
			 * @param const char* string value of the token
			 * @param TokenType type of the token
			 * @param TokenSubType subtype of the token
			 */
			Token(const char*, TokenType, TokenSubtype);

			bool operator==(Token& token);

			//! retrieve the token value
			const char* getValue() {return m_value.c_str();} 
			const string& getStrValue() {return m_value;}

			//! set the token value
			void setValue(const char* value) {
				m_value = value;
			}

			TokenType getType() {return m_type;}
			void setType(TokenType type) {
				m_type = type;
			}

			TokenSubtype getSubtype() {return m_subtype;}
			void setSubtype(TokenSubtype subtype){
				m_subtype = subtype; 
			}

			//! get printable token string
			const string getPrintableString();

			//! duplicate a another token
			Token* clone();
		private:

			string m_value;
			//string m_printableStr; //string used to print
			TokenType m_type;
			TokenSubtype m_subtype;
	}; //class Token


	//! manage token alloc and free
	class TokenAllocer
	{
		public:
			static Token* getToken();
			static Token* getToken(const char* szValue, Token::TokenType type);
			static Token* getToken(const char* szValue, Token::TokenType type, Token::TokenSubtype subtype);
			static void freeToken(Token*);

	};


};

#endif


