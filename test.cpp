#include "test.hpp"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

opts_t opts;
char *progname;

extern volatile void (*__start_testcases)(void);
extern volatile void (*__stop_testcases)(void);

namespace test {
std::mutex mtx;
std::vector<test::test_info_t> test_results;
std::map<std::string, std::map<std::string, std::vector<std::tuple<bool, std::string, std::string, std::string>>>>
    report;
uint64_t asserts_counter;
bool stub_res;
thread_local std::string ts_name = "", tc_name = "";

void run_tests() {
  std::thread *threads = new std::thread[opts.threads_num];
  uint64_t tcs_count = &__stop_testcases - &__start_testcases;
  if (opts.threads_num > tcs_count)
    opts.threads_num = tcs_count;
  uint64_t tc_per_thread_num = (opts.threads_num) ? tcs_count / opts.threads_num : tcs_count;

  volatile void (**last_tcs)(void) = &__start_testcases + opts.threads_num * tc_per_thread_num;
  uint64_t tc_leftover = (opts.threads_num) ? (&__stop_testcases - &__start_testcases) % opts.threads_num
                                            : &__stop_testcases - &__start_testcases;

  auto thread_task = [](volatile void (**start_addr)(void), uint64_t num) -> void {
    for (uint64_t i = 0; i < num; i++) {
      (*(start_addr + i))();
    }
  };

  for (uint64_t i = 0; i < opts.threads_num; i++) {
    threads[i] = std::thread(thread_task, &__start_testcases + (i * tc_per_thread_num), tc_per_thread_num);
  }

  std::thread leftover;
  if (tc_leftover)
    leftover = std::thread(thread_task, last_tcs, tc_leftover);
  for (unsigned int i = 0; i < opts.threads_num; i++)
    threads[i].join();
  if (leftover.joinable())
    leftover.join();
  delete[] threads;
}

bool assert_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                              const char *exp2_str) {
  bool ok = (std::strcmp(exp1, exp2) == 0);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  if (!ok)
    std::terminate();
  return ok;
}

bool assert_not_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                                  const char *exp2_str) {
  bool ok = (std::strcmp(exp1, exp2) != 0);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  if (!ok)
    std::terminate();
  return ok;
}

bool expect_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                              const char *exp2_str) {
  bool ok = (std::strcmp(exp1, exp2) == 0);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  return ok;
}

bool expect_not_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                                  const char *exp2_str) {
  bool ok = (std::strcmp(exp1, exp2) != 0);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  return ok;
}

bool assert_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                      const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (std::strcmp(exp1, exp2) == 0);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<decltype(exp1)> && !std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp1)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \""
                  << "nullptr"
                  << "\" )" << std::endl
                  << std::endl;
    }
  }

  test_results.push_back(std::make_tuple(asserts_counter, ok, *p_ts_name, *p_tc_name,
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "ASSERT_STREQ", exp1_str, exp2_str));
  asserts_counter++;

  if (!ok)
    std::terminate();
  return ok;
}

bool assert_not_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                          const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (std::strcmp(exp1, exp2) != 0);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<decltype(exp1)> && !std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp1)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \""
                  << "nullptr"
                  << "\" )" << std::endl
                  << std::endl;
    }
  }

  test_results.push_back(std::make_tuple(asserts_counter, ok, *p_ts_name, *p_tc_name,
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "ASSERT_NOT_STREQ", exp1_str, exp2_str));
  asserts_counter++;

  if (!ok)
    std::terminate();
  return ok;
}

bool expect_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                      const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (std::strcmp(exp1, exp2) == 0);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<decltype(exp1)> && !std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp1)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \""
                  << "nullptr"
                  << "\" )" << std::endl
                  << std::endl;
    }
  }

  test_results.push_back(std::make_tuple(asserts_counter, ok, *p_ts_name, *p_tc_name,
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "EXPECT_EQ", exp1_str, exp2_str));
  asserts_counter++;
  return ok;
}

bool expect_not_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                          const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (std::strcmp(exp1, exp2) != 0);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<decltype(exp1)> && !std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp1)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<decltype(exp2)>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \""
                  << "nullptr"
                  << "\" )" << std::endl
                  << std::endl;
    }
  }

  test_results.push_back(std::make_tuple(asserts_counter, ok, *p_ts_name, *p_tc_name,
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "EXPECT_NOT_STREQ", exp1_str, exp2_str));
  asserts_counter++;
  return ok;
}

void print_results(void) {
  std::printf("\r\n[\e[33mSUMMARY\e[39m] :\r\n");

  for (const std::pair<std::string,
                       std::map<std::string, std::vector<std::tuple<bool, std::string, std::string, std::string>>>>
           &testsuite_info : report) {

    uint64_t ts_pass_count = 0;
    uint64_t ts_fails_count = 0;
    std::printf("\tIn testsuite [\e[33m%s\e[39m] :\r\n", testsuite_info.first.c_str());

    for (const std::pair<std::string, std::vector<std::tuple<bool, std::string, std::string, std::string>>>
             &testcase_info : testsuite_info.second) {

      uint64_t tc_pass_count = 0;
      uint64_t tc_fails_count = 0;
      std::printf("\t\tIn testcase [\e[33m%s\e[39m] :\r\n", testcase_info.first.c_str());

      for (const std::tuple<bool, std::string, std::string, std::string> &check_info : testcase_info.second) {
        if (!std::get<1>(check_info).empty()) {

          if (opts.verbose_level > 0)
            std::printf((std::get<0>(check_info)) ? "\t\t\t[\e[32mOK\e[39m] (%s), ( \e[33m%s\e[39m, "
                                                    "\e[33m%s\e[39m )\r\n"
                                                  : "\t\t\t[\e[31mFAIL\e[39m] (%s), ( "
                                                    "\e[33m%s\e[39m, \e[33m%s\e[39m )\r\n",
                        std::get<1>(check_info).c_str(), std::get<2>(check_info).c_str(),
                        std::get<3>(check_info).c_str());

          (std::get<0>(check_info)) ? tc_pass_count++ : tc_fails_count++;
        }
      }

      ts_pass_count += tc_pass_count;
      ts_fails_count += tc_fails_count;
      std::printf("\r\n");
      std::printf("\t\t\tTotal passed - \e[32m%lu\e[39m, failed - "
                  "\e[31m%lu\e[39m in \"\e[33m%s\e[39m\" testcase\r\n",
                  tc_pass_count, tc_fails_count, testcase_info.first.c_str());
      std::printf("\r\n");
    }

    std::printf("\t\tTotal passed - \e[32m%lu\e[39m, failed - \e[31m%lu\e[39m "
                "in \"\e[33m%s\e[39m\" testsuite\r\n",
                ts_pass_count, ts_fails_count, testsuite_info.first.c_str());
    std::printf("\r\n\r\n");
  }
}
} // namespace test

void usage(void) {
  std::printf("Usage : %s [opts]\r\n\t-v [-vv] : Verbosity level (default is "
              "0).\r\n\t-t [digit] : Number of threads (default is "
              "1).\r\n\r\n\tExample : %s -v -t $(nproc)\r\n",
              progname, progname);
  std::exit(0);
}

int main(int argc, char *argv[]) {
  static const char *opt_str = "t:vh?";
  progname = argv[0];
  int opt = getopt(argc, argv, opt_str);

  while (opt != -1) {
    switch (opt) {

    case 't':
      opts.threads_num = atoi(optarg);
      break;

    case 'v':
      opts.verbose_level++;
      break;

    case 'h':
    case '?':
      usage();
      break;

    default:
      break;
    }

    opt = getopt(argc, argv, opt_str);
  }

  test::run_tests();
  test::print_results();
  return 0;
}
