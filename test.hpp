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

extern std::vector< test::test_info_t > test_results;
extern std::string current_ts_name, current_tc_name;
extern uint64_t asserts_counter;

namespace test
{
  void run_tests( void );
  void print_results( void );
  void assert_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void assert_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void expect_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );
  void expect_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str );

  template< typename A, typename B > void assert_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 == exp2 );

    std::printf( ok
                 ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i\r\n"
                 : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i\r\n"
                 , asserts_counter
                 , exp1_str
                 , exp2_str
                 , file
                 , line );

    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    asserts_counter ++;

    if( !ok ){

      print_results();
      std::terminate();

    }
  }

  template< typename A, typename B > void assert_not_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 != exp2 );

    std::printf( ok
                 ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i\r\n"
                 : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i\r\n"
                 , asserts_counter
                 , exp1_str
                 , exp2_str
                 , file
                 , line );

    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    asserts_counter ++;

    if( !ok ){

      print_results();
      std::terminate();

    }
  }

  template< typename A, typename B > void expect_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 == exp2 );

    std::printf( ok
                 ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i\r\n"
                 : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i\r\n"
                 , asserts_counter
                 , exp1_str
                 , exp2_str
                 , file
                 , line );

    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    asserts_counter ++;
  }

  template< typename A, typename B > void expect_not_equal( A exp1, B exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( exp1 != exp2 );

    std::printf( ok
                 ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i\r\n"
                 : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i\r\n"
                 , asserts_counter
                 , exp1_str
                 , exp2_str
                 , file
                 , line );

    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    asserts_counter ++;
  }
}

#define ASSERT_EQ( A, B ) try { test::assert_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
    test::print_results();                                              \
    std::terminate();                                                   \
  }                                                                         \

#define ASSERT_NOT_EQ( A, B ) try { test::assert_not_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
    test::print_results();                                              \
    std::terminate();                                                   \
  }                                                                         \

#define EXPECT_EQ( A, B ) try { test::expect_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s )\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
  }                                                                         \
  std::cout << ""                                                       \

#define EXPECT_NOT_EQ( A, B ) try { test::expect_not_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s )\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
  }                                                                         \
  std::cout << ""                                                       \

#define ASSERT_STREQ( A, B ) try { test::assert_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
    test::print_results();                                              \
    std::terminate();                                                   \
  }                                                                         \

#define ASSERT_NOT_STREQ( A, B ) try { test::assert_not_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s ). Terminating ...\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
    test::print_results();                                              \
    std::terminate();                                                   \
  }                                                                         \

#define EXPECT_STREQ( A, B ) try { test::expect_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s )\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
  }                                                                         \
  std::cout << ""                                                       \

#define EXPECT_NOT_STREQ( A, B ) try { test::expect_not_str_equal( A, B, __FILE__, __LINE__, #A, #B ); } \
  catch( std::exception & e ){                                          \
    std::printf( "#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( %s )\r\n" \
                 , e.what()                                                 \
                 , asserts_counter                                      \
                 , #A                                                   \
                 , #B                                                   \
                 , __FILE__                                                 \
                 , __LINE__ );                                          \
                                                                        \
    asserts_counter ++;                                                     \
    test_results.push_back( std::make_tuple( asserts_counter,           \
                                             false,                         \
                                             current_ts_name,           \
                                             current_tc_name,           \
                                             std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ), \
                                             std::string( #A ),             \
                                             std::string( #B )));       \
  }                                                                         \
  std::cout << ""                                                       \

#define TEST( TestSuiteName, TestCaseName )                                 \
  void __attribute__(( used )) test_suite_name_##TestSuiteName##_test_case_name_##TestCaseName##_nested( void ); \
  void __attribute__(( used )) test_suite_name_##TestSuiteName##_##test_case_name_##TestCaseName() \
  {                                                                         \
    extern std::string current_ts_name, current_tc_name;                \
    current_ts_name = #TestSuiteName;                                   \
    current_tc_name = #TestCaseName;                                    \
    std::printf( "\r\n%s\r\n", "Running " #TestSuiteName ":" #TestCaseName " ... \r\n" ); \
    test_suite_name_##TestSuiteName##_test_case_name_##TestCaseName##_nested();     \
  }                                                                         \
                                                                        \
  volatile void * __attribute__(( used, section( "test_function_pointers" ))) test_suite_name_##TestSuiteName##_##test_case_name_##TestCaseName##_ptr \
    = reinterpret_cast< volatile void * >( test_suite_name_##TestSuiteName##_##test_case_name_##TestCaseName ); \
  void __attribute__(( used )) test_suite_name_##TestSuiteName##_test_case_name_##TestCaseName##_nested( void ) \

#endif /* TEST_HPP */
