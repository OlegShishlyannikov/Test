#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <cstdint>
#include <cstring>
#include <tuple>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <mutex>
#include <thread>
#include <functional>

namespace test
{
  using success_t = bool;
  using test_suite_name_t = std::string;
  using test_case_name_t = std::string;
  using location_t = std::string;
  using expression_t = std::string;
  using assert_number_t = uint64_t;
  using test_info_t = std::tuple< assert_number_t, success_t, test_suite_name_t, test_case_name_t, location_t, expression_t, expression_t >;
};

struct opts_t
{
  int verbose_level = 0;
  int threads_num = 0;
};

extern opts_t opts;

namespace test
{
  void run_tests( void );
  void assert_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void assert_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void expect_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void expect_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void print_results( void );

  extern std::map< std::string, std::map< std::string, std::vector< std::tuple< bool, std::string, std::string, std::string >>>> report;
  extern std::vector< test::test_info_t > test_results;
  extern std::mutex mtx;
  extern uint64_t asserts_counter;
  extern thread_local std::string ts_name, tc_name;

  template< typename A, typename B > void assert_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 == exp2 );
	std::lock_guard< std::mutex > lock( mtx );
	
    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line
											  , std::hash< std::thread::id >()( std::this_thread::get_id() ));

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< A > && !std::is_null_pointer_v< B >))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< A >)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< B >)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}	  
	  }	  
	}

    test_results.push_back( std::make_tuple( asserts_counter, ok, ts_name, tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( ok, "ASSERT_EQ", exp1_str, exp2_str ));
    asserts_counter ++;
	
    if( !ok ) std::terminate();
  }

  template< typename A, typename B > void assert_not_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 != exp2 );
	std::lock_guard< std::mutex > lock( mtx );

    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line
											  , std::hash< std::thread::id >()( std::this_thread::get_id() ));

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< A > && !std::is_null_pointer_v< B >))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< A >)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< B >)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}	  
	  }	  
	}

    test_results.push_back( std::make_tuple( asserts_counter, ok, ts_name, tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( ok, "ASSERT_NOT_EQ", exp1_str, exp2_str ));
    asserts_counter ++;
	
    if( !ok ) std::terminate();
  }

  template< typename A, typename B > void expect_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 == exp2 );
	std::lock_guard< std::mutex > lock( mtx );
	
    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line
											  , std::hash< std::thread::id >()( std::this_thread::get_id() ));

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< A > && !std::is_null_pointer_v< B >))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< A >)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< B >)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}	  
	  }	  
	}

    test_results.push_back( std::make_tuple( asserts_counter, ok, ts_name, tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( ok, "EXPECT_EQ", exp1_str, exp2_str ));
    asserts_counter ++;
  }

  template< typename A, typename B > void expect_not_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 != exp2 );
	std::lock_guard< std::mutex > lock( mtx );

    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line
											  , std::hash< std::thread::id >()( std::this_thread::get_id() ));
	
	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< A > && !std::is_null_pointer_v< B >))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< A >)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< B >)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}	  
	  }	  
	}

	test_results.push_back( std::make_tuple( asserts_counter, ok, ts_name, tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( ok, "EXPECT_NOT_EQ", exp1_str, exp2_str ));
    asserts_counter ++;
  }
}

#define ASSERT_EQ( A, B ) try { test::assert_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
                                             false,						\
                                             ts_name,					\
                                             tc_name,					\
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),			\
                                             std::string( #B )));       \
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "ASSERT_EQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define ASSERT_NOT_EQ( A, B ) try { test::assert_not_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
											 false,						\
											 ts_name,					\
											 tc_name,					\
											 std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
											 std::string( #A ),			\
											 std::string( #B )));		\
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "ASSERT_NOT_EQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define EXPECT_EQ( A, B ) try { test::expect_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,						\
                                             ts_name,					\
                                             tc_name,					\
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),			\
                                             std::string( #B )));       \
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "EXPECT_EQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define EXPECT_NOT_EQ( A, B ) try { test::expect_not_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
											 false,						\
											 ts_name,					\
											 tc_name,					\
											 std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
											 std::string( #A ),			\
											 std::string( #B )));		\
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "EXPECT_NOT_EQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define ASSERT_STREQ( A, B ) try { test::assert_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
											 false,						\
											 ts_name,					\
											 tc_name,					\
											 std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
											 std::string( #A ),			\
											 std::string( #B )));		\
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "ASSERT_STREQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define ASSERT_NOT_STREQ( A, B ) try { test::assert_not_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
											 false,						\
											 ts_name,					\
											 tc_name,					\
											 std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
											 std::string( #A ),			\
											 std::string( #B )));		\
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "ASSERT_NOT_STREQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define EXPECT_STREQ( A, B ) try { test::expect_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
											 false,						\
											 ts_name,					\
											 tc_name,					\
											 std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
											 std::string( #A ),			\
											 std::string( #B )));		\
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "EXPECT_STREQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define EXPECT_NOT_STREQ( A, B ) try { test::expect_not_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
	using namespace test;												\
	std::lock_guard< std::mutex > lock( test::mtx );					\
	if( opts.verbose_level > 1 ) std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
											  , asserts_counter			\
											  , __FILE__				\
											  , __LINE__				\
											  , e.what() );				\
                                                                        \
	asserts_counter ++;													\
	test_results.push_back( std::make_tuple( asserts_counter,			\
											 false,						\
											 ts_name,					\
											 tc_name,					\
											 std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
											 std::string( #A ),			\
											 std::string( #B )));		\
	report.at( ts_name ).at( tc_name ).push_back( std::make_tuple( false, "EXPECT_NOT_STREQ", std::string( #A ), std::string( #B ) )); \
	std::terminate();													\
  }																		\
  if( opts.verbose_level > 1 ) std::cout << ""							\

#define TEST( TestSuiteName, TestCaseName )								\
  volatile void __attribute__(( used, weak )) test_suite_name_##TestSuiteName##_test_case_name_##TestCaseName##_nested(); \
  volatile void __attribute__(( used )) test_suite_name_##TestSuiteName##_##test_case_name_##TestCaseName() \
  {																		\
	test::mtx.lock();													\
	test::ts_name = #TestSuiteName;										\
	test::tc_name = #TestCaseName;										\
	if( test::report.find( #TestSuiteName ) == test::report.end() )		\
	  test::report.insert( std::make_pair( #TestSuiteName, std::map< std::string, std::vector< std::tuple< bool, std::string, std::string, std::string >>>() )); \
	if( test::report.at( #TestSuiteName ).find( #TestCaseName ) == test::report.at( #TestSuiteName ).end() ) \
	  test::report.at( #TestSuiteName ).insert( std::make_pair( #TestCaseName, std::vector< std::tuple< bool, std::string, std::string, std::string >>() )); \
	if( opts.verbose_level > 1 ) std::printf( "\r\nRunning %s : %s ... \r\n\r\n", #TestSuiteName, #TestCaseName ); \
	test::mtx.unlock();													\
	test_suite_name_##TestSuiteName##_test_case_name_##TestCaseName##_nested(); \
  }																		\
																		\
  volatile void * __attribute__(( used, section( "test_function_pointers" ))) test_suite_name_##TestSuiteName##_##test_case_name_##TestCaseName##_ptr \
	= reinterpret_cast< volatile void * >( test_suite_name_##TestSuiteName##_##test_case_name_##TestCaseName ); \
  volatile void __attribute__(( used )) test_suite_name_##TestSuiteName##_test_case_name_##TestCaseName##_nested() \

#endif /* TEST_HPP */
