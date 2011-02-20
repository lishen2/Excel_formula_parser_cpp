#include "gtest/gtest.h"
#include "StrUtils.h"

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


int main(int argc, char* argv[])
{

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


