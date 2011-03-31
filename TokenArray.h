#ifndef _EXCELFORMUAL_TOKEN_ARRAY_H__
#define _EXCELFORMUAL_TOKEN_ARRAY_H__

#include <vector>

using std::vector;

namespace ExcelFormula{

	class Token;


	class TokenArray{
		public:
			TokenArray();

			void toVector(vector<Token*>&);
			void fromVector(vector<Token*>&);

			size_t size() {return m_tokens.size();}

			bool isBOF() {return (m_index <= 0)?true:false;}

			bool isEOF() {return (m_index >= m_tokens.size())?true:false;}

			Token* getCurrent();

			Token* getNext();

			Token* getPrevious();

			Token* add(Token*);

			bool moveNext();

			void reset(){m_index = 0;}

			void releaseAll();

		private:
			size_t m_index;
			vector<Token*> m_tokens;
	};
}

#endif


