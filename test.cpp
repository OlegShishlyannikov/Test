#include "test.hpp"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

opts_t opts;

std::string current_ts_name, current_tc_name;
std::vector< test::test_info_t > test_results;
std::map< std::string, std::map< std::string, std::vector< std::tuple< bool, std::string, std::string, std::string >>>> report;
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

  void assert_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) == 0 );

    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line );

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< decltype( exp1 )> && !std::is_null_pointer_v< decltype( exp2 )>))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< decltype( exp1 )>)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< decltype( exp2 )>)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}
	  
	  }	  
	}
	
    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( current_ts_name ).at( current_tc_name ).push_back( std::make_tuple( ok, "ASSERT_STREQ", exp1_str, exp2_str ));
    asserts_counter ++;

    if( !ok ) std::terminate();
  }

  void assert_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) != 0 );

    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line );

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< decltype( exp1 )> && !std::is_null_pointer_v< decltype( exp2 )>))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< decltype( exp1 )>)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< decltype( exp2 )>)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}
	  
	  }	  
	}
	
    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( current_ts_name ).at( current_tc_name ).push_back( std::make_tuple( ok, "ASSERT_NOT_STREQ", exp1_str, exp2_str ));
    asserts_counter ++;

    if( !ok ) std::terminate();
  }

  void expect_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) == 0 );

    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line );

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< decltype( exp1 )> && !std::is_null_pointer_v< decltype( exp2 )>))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< decltype( exp1 )>)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< decltype( exp2 )>)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}
	  
	  }	  
	}
	
    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( current_ts_name ).at( current_tc_name ).push_back( std::make_tuple( ok, "EXPECT_EQ", exp1_str, exp2_str ));
    asserts_counter ++;
  }

  void expect_not_str_equal( const char * exp1, const char * exp2, const char * file, int line, const char * exp1_str, const char * exp2_str )
  {
    success_t ok = ( std::strcmp( exp1, exp2 ) != 0 );

    if( opts.verbose_level > 1 ) std::printf( ok
											  ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i\r\n"
											  : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i\r\n"
											  , asserts_counter
											  , exp1_str
											  , exp2_str
											  , file
											  , line );

	if( !ok ){

	  if constexpr (( !std::is_null_pointer_v< decltype( exp1 )> && !std::is_null_pointer_v< decltype( exp2 )>))
					 if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  else if constexpr (( std::is_null_pointer_v< decltype( exp1 )>)) {

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << "nullptr" << "\", \"" << exp2 << "\" )" << std::endl << std::endl; } else {}
	  
	  } else if constexpr (( std::is_null_pointer_v< decltype( exp2 )>)){

		if( opts.verbose_level > 1 ){ std::cout << "( \"" << exp1 << "\", \"" << "nullptr" << "\" )" << std::endl << std::endl; } else {}
	  
	  }	  
	}

    test_results.push_back( std::make_tuple( asserts_counter, ok, current_ts_name, current_tc_name, std::string( file ) + ":" + std::to_string( line ), std::string( exp1_str ), std::string( exp2_str )));
    report.at( current_ts_name ).at( current_tc_name ).push_back( std::make_tuple( ok, "EXPECT_NOT_STREQ", exp1_str, exp2_str ));
    asserts_counter ++;
  }

  void print_results( void )
  {
    std::printf( "\r\n[\e[33mSUMMARY\e[39m] :\r\n" );

    for( const std::pair< std::string, std::map< std::string, std::vector< std::tuple< bool, std::string, std::string, std::string >>>> & testsuite_info : report ){

      uint64_t ts_pass_count = 0;
      uint64_t ts_fails_count = 0;
      std::printf( "\tIn testsuite [\e[33m%s\e[39m] :\r\n", testsuite_info.first.c_str() );

      for( const std::pair< std::string, std::vector< std::tuple< bool, std::string, std::string, std::string >>> & testcase_info : testsuite_info.second ){

        uint64_t tc_pass_count = 0;
        uint64_t tc_fails_count = 0;
        std::printf( "\t\tIn testcase [\e[33m%s\e[39m] :\r\n", testcase_info.first.c_str() );

        for( const std::tuple< bool, std::string, std::string, std::string > & check_info : testcase_info.second ){
          if( !std::get< 1 >( check_info ).empty() ){

            if( opts.verbose_level > 0 ) std::printf(( std::get< 0 >( check_info ))
													 ? "\t\t\t[\e[32mOK\e[39m] (%s), ( \e[33m%s\e[39m, \e[33m%s\e[39m )\r\n"
													 : "\t\t\t[\e[31mFAIL\e[39m] (%s), ( \e[33m%s\e[39m, \e[33m%s\e[39m )\r\n",
													 std::get< 1 >( check_info ).c_str(),
													 std::get< 2 >( check_info ).c_str(),
													 std::get< 3 >( check_info ).c_str() );

            ( std::get< 0 >( check_info )) ? tc_pass_count ++ : tc_fails_count ++;
          }
        }

        ts_pass_count += tc_pass_count;
        ts_fails_count += tc_fails_count;
		std::printf( "\r\n" );
        std::printf( "\t\t\tTotal passed - \e[32m%lu\e[39m, failed - \e[31m%lu\e[39m in \"\e[33m%s\e[39m\" testcase\r\n", tc_pass_count, tc_fails_count, testcase_info.first.c_str() );
        std::printf( "\r\n" );
      }

      std::printf( "\t\tTotal passed - \e[32m%lu\e[39m, failed - \e[31m%lu\e[39m in \"\e[33m%s\e[39m\" testsuite\r\n", ts_pass_count, ts_fails_count, testsuite_info.first.c_str() );
      std::printf( "\r\n\r\n" );
    }
  }
}

void usage( void )
{
  std::printf( "Usage : ./<project_name>.elf [opts]\r\n\t -v : verbosity level\r\n" );
}

int main( int argc, char * argv[] )
{
  static const char * opt_str = "vh?";

  int opt = getopt( argc, argv, opt_str );

  while( opt != -1 ) {
	switch( opt ) {
	  
	case 'v' :
	  opts.verbose_level ++;
	  break;
                 
	case 'h' :
	case '?' :
	  usage();
	  break;
                 
	default : break;
	}
         
	opt = getopt( argc, argv, opt_str );
  }
     
  test::run_tests();
  test::print_results();
  return 0;
}
