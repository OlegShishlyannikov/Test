# Test
Simple testing framework

## Usage: 

### Existing asserts and expectations:

	```CPP
	EXPECT_EQ( exp1, exp2 );
	EXPECT_NOT_EQ( exp1, exp2 );
	EXPECT_STREQ( exp1, exp2 );
	EXPECT_NOT_STREQ( exp1, exp2 );
	ASSERT_EQ( exp1, exp2 );
	ASSERT_NOT_EQ( exp1, exp2 );
	ASSERT_STREQ( exp1, exp2 );
	ASSERT_NOT_STREQ( exp1, exp2 );
	```

### Creating tests in same of separate file (this functions must be statically linked to executable):

	```CPP
	TEST( "Test suite name", "Test case name" )
	{
	ASSERT_EQ( 2 + 2, 4 );
	EXPECT_STREQ( "Foo", "Foo" );
	...
	}
	```
