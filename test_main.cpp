#include "gtest/gtest.h"
#include "StrUtils.h"
#include "Token.h"
#include "TokenArray.h"
#include "TokenStack.h"
#include <iostream>

using std::cout;
using std::endl;
using ExcelFormula::Token;
using ExcelFormula::TokenStack;
using ExcelFormula::TokenArray;

//! test trim function
TEST(StrUtilsTest, trimFuncTest)
{
	string testStr = "example string"; 
	EXPECT_STREQ("example string", StrUtils::trim(testStr).c_str());
	EXPECT_STREQ("example string", testStr.c_str());

	testStr = "\t example string"; 
	EXPECT_STREQ("example string", StrUtils::trim(testStr).c_str());
	EXPECT_STREQ("example string", testStr.c_str());

	testStr = "example string "; 
	EXPECT_STREQ("example string", StrUtils::trim(testStr).c_str());
	EXPECT_STREQ("example string", testStr.c_str());
	
	testStr = "     example string\t\t "; 
	EXPECT_STREQ("example string", StrUtils::trim(testStr).c_str());
	EXPECT_STREQ("example string", testStr.c_str());
}

//! test indexof function
TEST(StrUtilsTest, indexOfFuncTest)
{
	const char* strs[] = {"str1", "str2" ,"str3", "str4"};
	const int len = 4;

	EXPECT_EQ(0, StrUtils::indexOf(len, strs, "str1"));
	EXPECT_EQ(1, StrUtils::indexOf(len, strs, "str2"));
	EXPECT_EQ(2, StrUtils::indexOf(len, strs, "str3"));
	EXPECT_EQ(3, StrUtils::indexOf(len, strs, "str4"));
	EXPECT_EQ(-1, StrUtils::indexOf(len, strs, "str5"));

}

TEST(FormulaUtil, Token)
{
	Token token1,token2,
		  token3("+", Token::Operand, Token::Math),
		  token4("+", Token::Operand, Token::Math);

	EXPECT_TRUE(token1 == token2);
	EXPECT_FALSE(token1 == token3);
	EXPECT_TRUE(token3 == token4);
	EXPECT_STREQ("+ <Operand> <Math>", token3.getPrintableString());
}


//! test Tokens functionality
TEST(FormulaUtil, TokenArray)
{
	TokenArray tokens;
	EXPECT_EQ(0, tokens.size()) << "For empey token container";
	EXPECT_TRUE(tokens.isBOF()) << "For empey token container";
	EXPECT_TRUE(tokens.isEOF()) << "For empey token container";

	//! generate test toknes
	Token token1;
	Token token2("+", Token::Operand,
		Token::Logical);
	Token token3("IF", Token::Function,
		Token::Stop);
	Token token4("4", Token::Argument,
		Token::Number);
	Token token5("Coati", Token::Argument,
		Token::Text);
	
	//! insert into token container
	tokens.add(&token1);
	tokens.add(&token2);
	tokens.add(&token3);
	tokens.add(&token4);
	tokens.add(&token5);

	EXPECT_TRUE(tokens.isBOF()) << "After insert 5 tokens.";
	EXPECT_FALSE(tokens.isEOF()) << "After insert 5 tokens.";
	EXPECT_EQ(5, tokens.size()) << "Test Length.";

	EXPECT_TRUE(tokens.getCurrent() == &token1) << "Current pos :0";
	EXPECT_TRUE(tokens.getNext() == &token2) << "Current pos :0";
	EXPECT_TRUE(tokens.getPrevious() == NULL) << "Current pos :0";

	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_FALSE(tokens.isBOF()) << "Current pos :1";
	EXPECT_FALSE(tokens.isEOF()) << "Current pos :1";

	EXPECT_TRUE(tokens.getCurrent() == &token2) << "Current pos:1";

	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_FALSE(tokens.moveNext()); // move to next item.

	EXPECT_FALSE(tokens.getCurrent() == &token5) << "At last pos.";
	EXPECT_FALSE(tokens.isBOF()) << "At last pos.";
	EXPECT_TRUE(tokens.isEOF()) << "At last pos.";
}

TEST(FormulaUtil, TokenStack)
{
	Token token1;
	Token token2("+", Token::Operand,
		Token::Logical);
	Token token3("IF", Token::Function,
		Token::Stop);
	Token token4("4", Token::Argument,
		Token::Number);
	Token token5("Coati", Token::Argument,
		Token::Text);

	TokenStack stack;
	stack.push(&token1);
	stack.push(&token2);
	stack.push(&token3);
	stack.push(&token4);
	stack.push(&token5);

	//Token ttoken = stack.getCurrent();
	
	EXPECT_TRUE(stack.getCurrent() == &token5);
	EXPECT_TRUE(stack.pop() == &token5);
	EXPECT_TRUE(stack.pop() == &token4);
	EXPECT_TRUE(stack.pop() == &token3);
	EXPECT_TRUE(stack.pop() == &token2);
	EXPECT_TRUE(stack.pop() == &token1);
	EXPECT_TRUE(stack.pop() == NULL);

}

/*
TEST(ParserTest, literalScanning)
{
	string formula1("=1+3+5");
	ExcelFormulaParser::ExcelFormula parser1(formula1);
	parser1.parserToToken1();
	vector<Token> tokens = parser1.getTokens();
	
	for(vector<Token>::iterator it = tokens.begin();
			it != tokens.end();
			++it)
	{
		cout << it->getPrintableString();
	}

	cout << endl;
}
*/

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


