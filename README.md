# Test
Simple testing framework

## Usage: 
	```bash
	$ <app_build>.elf -t <number_of_threads> -v[verbosity]; # At example: ./app -t 8 -vv
	```
### Existing asserts and expectations:
	```cpp
	EXPECT_EQ(exp1, exp2, YOUR_COMMENT);
	EXPECT_NOT_EQ(exp1, exp2, YOUR_COMMENT);
	EXPECT_STREQ(exp1, exp2, YOUR_COMMENT);
	EXPECT_NOT_STREQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_EQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_NOT_EQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_STREQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_NOT_STREQ(exp1, exp2, YOUR_COMMENT);
	```
### To create test:
	```cpp
	TEST(TestSuiteName, TestCaseName" )
	{
		ASSERT_EQ(2 + 2, 4, "2 + 2 = 4");
		EXPECT_STREQ("Foo", "Foo", "Equal strings");
	}
	```
### Or without any testsuite or testcase:
	```cpp
	EXPECT_EQ(2*2, 4, "2*2 = 4");
	```
