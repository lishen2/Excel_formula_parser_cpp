#include "gtest/gtest.h"
#include "StrUtils.h"
#include "ExcelFormula.h"
#include <iostream>

using std::cout;
using std::endl;
using ExcelFormulaParser::ExcelFormulaToken;
using ExcelFormulaParser::ExcelFormula;

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


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


