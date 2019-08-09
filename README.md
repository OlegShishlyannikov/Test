# Test
Simple testing framework

## Usage: 
	.. code:: bash
	$ <app_build>.elf -t <number_of_threads> -v[verbosity]; # At example: ./app -t 8 -vv
	..
### Existing asserts and expectations:
	.. code:: c++
	EXPECT_EQ(exp1, exp2, YOUR_COMMENT);
	EXPECT_NOT_EQ(exp1, exp2, YOUR_COMMENT);
	EXPECT_STREQ(exp1, exp2, YOUR_COMMENT);
	EXPECT_NOT_STREQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_EQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_NOT_EQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_STREQ(exp1, exp2, YOUR_COMMENT);
	ASSERT_NOT_STREQ(exp1, exp2, YOUR_COMMENT);
	..
### To create test:
	.. code:: c++
	TEST(TestSuiteName, TestCaseName" )
	{
		ASSERT_EQ(2 + 2, 4, "2 + 2 = 4");
		EXPECT_STREQ("Foo", "Foo", "Equal strings");
	}
	..
### Or without any testsuite or testcase:
	.. code:: c++
	EXPECT_EQ(2*2, 4, "2*2 = 4");
	..
