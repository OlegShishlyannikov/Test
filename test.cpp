#include "test.hpp"

std::string current_ts_name, current_tc_name;
std::vector< test::test_info_t > test_results;
uint64_t asserts_counter;

extern volatile void( *__start_test_function_pointers )( void );
extern volatile void( *__stop_test_function_pointers )( void );

namespace test
{  
  void run_tests()
  {
    uint64_t sec_size = &__stop_test_function_pointers - &__start_test_function_pointers;
    for( uint64_t i = 0; i < sec_size; i ++ ) (*( &__start_test_function_pointers + i ))();
  }
  
  void print_results( void )
  {
    using fail_count_t = uint64_t;
    using success_count_t = uint64_t;

    std::map< test_suite_name_t, std::map< test_case_name_t, std::pair< success_count_t, fail_count_t >>> results;

    for( test_info_t & info : test_results ){

      if( results.find( std::get< 2 >( info )) == results.end() ){

        results.insert( std::make_pair( std::get< 2 >( info ), std::map< test_case_name_t, std::pair< success_count_t, fail_count_t >>() ));
        goto fill_test_suite_results;

      }     else {

      fill_test_suite_results:

        std::map< test_case_name_t, std::pair< success_count_t, fail_count_t >> & test_suite_info = results.at( std::get< 2 >( info ));

        if( test_suite_info.find( std::get< 3 >( info )) == test_suite_info.end() ){

          test_suite_info.insert( std::make_pair( std::get< 3 >( info ), std::pair< success_count_t, fail_count_t >( std::make_pair( 0u, 0u ))));
          goto fill_test_case_results;

        } else {

        fill_test_case_results:

          std::pair< success_count_t, fail_count_t > & counts = test_suite_info.at( std::get< 3 >( info ));

          if( std::get< 1 >( info )) counts.first ++;
          else counts.second ++;

        }
      }
    }

    for( const std::pair< test_suite_name_t, std::map< test_case_name_t, std::pair< success_count_t, fail_count_t >>> & test_suite_results : results ){

      success_count_t test_suite_success_count = 0;
      fail_count_t test_suite_fail_count = 0;

      for( const std::pair< test_case_name_t, std::pair< success_count_t, fail_count_t >> & test_case_results : test_suite_results.second ){

        test_suite_success_count += test_case_results.second.first;
        test_suite_fail_count += test_case_results.second.second;

      }

      std::printf( "\r\n[\e[33mSummary\e[39m] : \r\n\r\nIn [\e[33m\"%s\"\e[39m] testsuite There are \e[32m%lu\e[39m success asserts and \e[31m%lu\e[39m failed asserts :\r\n",
                   test_suite_results.first.c_str(),
                   test_suite_success_count,
                   test_suite_fail_count );

      for( const std::pair< test_case_name_t, std::pair< success_count_t, fail_count_t >> & test_case_results : test_suite_results.second ){

        std::printf( "\r\n\tIn testcase [\e[33m\"%s\"\e[39m]:\r\n\r\n\t\tSuccess : \e[32m%lu\e[39m\r\n\t\tFailed : \e[31m%lu\e[39m\r\n",
                     test_case_results.first.c_str(),
                     test_case_results.second.first,
                     test_case_results.second.second );

        std::for_each( test_results.begin(), test_results.end()
                       , [ &test_case_results ]
                       ( test_info_t & element ) -> void
                         {
                           if(( !std::get< 1 >( element )) && ( std::get< 3 >( element ) == test_case_results.first ))
                             std::printf( "\r\n\t\t#%lu [\e[31mFAIL\e[39m] at %s\r\n\t\t\tIn testsuite %s, testcase %s\r\n\t\t\tExpressions : %s, %s\r\n",
                                          std::get< 0 >( element ),
                                          std::get< 4 >( element ).c_str(),
                                          std::get< 2 >( element ).c_str(),
                                          std::get< 3 >( element ).c_str(),
                                          std::get< 5 >( element ).c_str(),
                                          std::get< 6 >( element ).c_str() );
                         });
      }
    }
  }

  void assert_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) == 0 );

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

  void assert_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) != 0 );

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
  
  void expect_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) == 0 );

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

  void expect_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) != 0 );

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
