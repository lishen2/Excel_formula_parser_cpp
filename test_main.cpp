#include "gtest/gtest.h"
#include "StrUtils.h"
#include "ExcelFormula.h"
#include <iostream>

using std::cout;
using std::endl;
using ExcelFormulaParser::ExcelFormulaToken;
using ExcelFormulaParser::ExcelFormula;

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

//! test ExcelFormulaTokens functionality
TEST(ExcelFormula, ExcelFormulaTokens)
{
	ExcelFormulaParser::ExcelFormulaTokens tokens;
	EXPECT_EQ(0, tokens.size()) << "For empey token container";
	EXPECT_TRUE(tokens.isBOF()) << "For empey token container";
	EXPECT_TRUE(tokens.isEOF()) << "For empey token container";

	//! generate test toknes
	ExcelFormulaToken token1,emptyToken;
	ExcelFormulaToken token2(string("+"), ExcelFormulaToken::Operand,
		ExcelFormulaToken::Logical);
	ExcelFormulaToken token3(string("IF"), ExcelFormulaToken::Function,
		ExcelFormulaToken::Stop);
	ExcelFormulaToken token4(string("4"), ExcelFormulaToken::Argument,
		ExcelFormulaToken::Number);
	ExcelFormulaToken token5(string("Coati"), ExcelFormulaToken::Argument,
		ExcelFormulaToken::Text);
	
	//! insert into token container
	tokens.add(token1);
	tokens.add(token2);
	tokens.add(token3);
	tokens.add(token4);
	tokens.add(token5);

	EXPECT_TRUE(tokens.isBOF()) << "After insert 5 tokens.";
	EXPECT_FALSE(tokens.isEOF()) << "After insert 5 tokens.";
	EXPECT_EQ(5, tokens.size()) << "Test Length.";

	EXPECT_TRUE(tokens.getCurrent() == token1) << "Current pos :0";
	EXPECT_TRUE(tokens.getNext() == token2) << "Current pos :0";
	EXPECT_TRUE(tokens.getPrevious() == emptyToken) << "Current pos :0";

	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_FALSE(tokens.isBOF()) << "Current pos :1";
	EXPECT_FALSE(tokens.isEOF()) << "Current pos :1";

	EXPECT_TRUE(tokens.getCurrent() == token2) << "Current pos:1";

	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_TRUE(tokens.moveNext()); // move to next item.
	EXPECT_FALSE(tokens.moveNext()); // move to next item.

	EXPECT_FALSE(tokens.getCurrent() == token5) << "At last pos.";
	EXPECT_FALSE(tokens.isBOF()) << "At last pos.";
	EXPECT_TRUE(tokens.isEOF()) << "At last pos.";
}

TEST(ParserTest, ExcelFormulaStack)
{
	ExcelFormulaToken token1,emptyToken;
	ExcelFormulaToken token2(string("+"), ExcelFormulaToken::Operand,
		ExcelFormulaToken::Logical);
	ExcelFormulaToken token3(string("IF"), ExcelFormulaToken::Function,
		ExcelFormulaToken::Stop);
	ExcelFormulaToken token4(string("4"), ExcelFormulaToken::Argument,
		ExcelFormulaToken::Number);
	ExcelFormulaToken token5(string("Coati"), ExcelFormulaToken::Argument,
		ExcelFormulaToken::Text);

	ExcelFormulaStack stack;
	EXPECT_TRUE


}

/*
TEST(ParserTest, literalScanning)
{
	string formula1("=1+3+5");
	ExcelFormulaParser::ExcelFormula parser1(formula1);
	parser1.parserToToken1();
	vector<ExcelFormulaToken> tokens = parser1.getTokens();
	
	for(vector<ExcelFormulaToken>::iterator it = tokens.begin();
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


