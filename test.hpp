#ifndef TEST_HPP
#define TEST_HPP

#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <vector>

namespace test {
using success_t = bool;
using test_suite_name_t = std::string;
using test_case_name_t = std::string;
using location_t = std::string;
using expression_t = std::string;
using assert_number_t = uint64_t;
using test_info_t =
    std::tuple<assert_number_t, success_t, test_suite_name_t, test_case_name_t, location_t, expression_t, expression_t>;
}; // namespace test

struct opts_t {
  int verbose_level = 0;
  int threads_num = 0;
};

extern opts_t opts;

namespace test {
void run_tests(void);
bool assert_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                      const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name);
bool assert_not_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                          const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name);
bool expect_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                      const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name);
bool expect_not_str_equal(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                          const char *exp2_str, const std::string *p_ts_name, const std::string *p_tc_name);
bool assert_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                              const char *exp2_str);
bool assert_not_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                                  const char *exp2_str);
bool expect_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                              const char *exp2_str);
bool expect_not_str_equal_builtin(const char *exp1, const char *exp2, const char *file, int line, const char *exp1_str,
                                  const char *exp2_str);
void print_results(void);

static std::function<std::pair<std::string *, std::string *>(void)> *resolv_ts_tc_names = nullptr;
static std::condition_variable sync_var;
static bool notified = true;

extern std::map<std::string,
                std::map<std::string, std::vector<std::tuple<bool, std::string, std::string, std::string>>>>
    report;
extern std::vector<test::test_info_t> test_results;
extern std::mutex mtx;
extern uint64_t asserts_counter;
extern thread_local std::string ts_name, tc_name;
extern bool stub_res;

template <typename A, typename B>
bool assert_equal_builtin(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str) {
  bool ok = (exp1 == exp2);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  if (!ok)
    std::terminate();
  return ok;
}

template <typename A, typename B>
bool assert_not_equal_builtin(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str) {
  bool ok = (exp1 != exp2);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  if (!ok)
    std::terminate();
  return ok;
}

template <typename A, typename B>
bool expect_equal_builtin(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str) {
  bool ok = (exp1 == exp2);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  return ok;
}

template <typename A, typename B>
bool expect_not_equal_builtin(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str) {
  bool ok = (exp1 != exp2);
  std::lock_guard<std::mutex> lock(mtx);
  test_results.push_back(std::make_tuple(asserts_counter, ok, "none", "none",
                                         std::string(file) + ":" + std::to_string(line), std::string(exp1_str),
                                         std::string(exp2_str)));
  asserts_counter++;
  return ok;
}

template <typename A, typename B>
bool assert_equal(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str,
                  const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (exp1 == exp2);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<A> && !std::is_null_pointer_v<B>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<A>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<B>)) {
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
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "ASSERT_EQ", exp1_str, exp2_str));
  asserts_counter++;

  if (!ok)
    std::terminate();
  return ok;
}

template <typename A, typename B>
bool assert_not_equal(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str,
                      const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (exp1 != exp2);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<A> && !std::is_null_pointer_v<B>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<A>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<B>)) {
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
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "ASSERT_NOT_EQ", exp1_str, exp2_str));
  asserts_counter++;

  if (!ok)
    std::terminate();
  return ok;
}

template <typename A, typename B>
bool expect_equal(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str,
                  const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (exp1 == exp2);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s == %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s != %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<A> && !std::is_null_pointer_v<B>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<A>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<B>)) {
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

template <typename A, typename B>
bool expect_not_equal(A exp1, B exp2, const char *file, int line, const char *exp1_str, const char *exp2_str,
                      const std::string *p_ts_name, const std::string *p_tc_name) {
  bool ok = (exp1 != exp2);
  std::lock_guard<std::mutex> lock(mtx);

  if (opts.verbose_level > 1 || !ok)
    std::printf(ok ? "#%lu [\e[32mOK\e[39m] (%s != %s) At %s:%i, in thread #0x%lx\r\n"
                   : "#%lu [\e[31mFAIL\e[39m] (%s == %s) At %s:%i, in thread "
                     "#0x%lx\r\n",
                asserts_counter, exp1_str, exp2_str, file, line,
                std::hash<std::thread::id>()(std::this_thread::get_id()));

  if (!ok) {
    if constexpr ((!std::is_null_pointer_v<A> && !std::is_null_pointer_v<B>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \"" << exp1 << "\", \"" << exp2 << "\" )" << std::endl << std::endl;
    } else if constexpr ((std::is_null_pointer_v<A>)) {
      if (opts.verbose_level > 1)
        std::cout << "( \""
                  << "nullptr"
                  << "\", \"" << exp2 << "\" )" << std::endl
                  << std::endl;
    } else if constexpr ((std::is_null_pointer_v<B>)) {
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
  report.at(*p_ts_name).at(*p_tc_name).push_back(std::make_tuple(ok, "EXPECT_NOT_EQ", exp1_str, exp2_str));
  asserts_counter++;
  return ok;
}
} // namespace test

#define ASSERT_EQ(A, B, COMMENT)                                                                                       \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::assert_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                         \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res                                                                                                     \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "ASSERT_EQ", std::string(#A), std::string(#B)));                         \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::assert_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                                       \
        std::printf(COMMENT);                                                                                          \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define ASSERT_NOT_EQ(A, B, COMMENT)                                                                                   \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::assert_not_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                     \
        std::printf(COMMENT);                                                                                          \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "ASSERT_NOT_EQ", std::string(#A), std::string(#B)));                     \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::assert_not_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                                   \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define EXPECT_EQ(A, B, COMMENT)                                                                                       \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::expect_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                         \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "EXPECT_EQ", std::string(#A), std::string(#B)));                         \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::expect_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                                       \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define EXPECT_NOT_EQ(A, B, COMMENT)                                                                                   \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::expect_not_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                     \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "EXPECT_NOT_EQ", std::string(#A), std::string(#B)));                     \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::expect_not_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                                   \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define ASSERT_STREQ(A, B, COMMENT)                                                                                    \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::assert_str_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                     \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "ASSERT_STREQ", std::string(#A), std::string(#B)));                      \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res test::assert_str_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                                     \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define ASSERT_NOT_STREQ(A, B, COMMENT)                                                                                \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::assert_not_str_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                 \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "ASSERT_NOT_STREQ", std::string(#A), std::string(#B)));                  \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::assert_not_str_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                               \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        std::terminate();                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define EXPECT_STREQ(A, B, COMMENT)                                                                                    \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::expect_str_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                     \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "EXPECT_STREQ", std::string(#A), std::string(#B)));                      \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::expect_str_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                                   \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define EXPECT_NOT_STREQ(A, B, COMMENT)                                                                                \
  (test::stub_res = [&](auto, auto) -> bool {                                                                          \
    if (test::resolv_ts_tc_names) {                                                                                    \
      auto [p_ts_name, p_tc_name] = (*test::resolv_ts_tc_names)();                                                     \
      try {                                                                                                            \
        bool res = test::expect_not_str_equal(A, B, __FILE__, __LINE__, #A, #B, p_ts_name, p_tc_name);                 \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, *p_ts_name, *p_tc_name,                         \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        report.at(*p_ts_name)                                                                                          \
            .at(*p_tc_name)                                                                                            \
            .push_back(std::make_tuple(false, "EXPECT_NOT_STREQ", std::string(#A), std::string(#B)));                  \
        std::printf(COMMENT);                                                                                          \
        return false;                                                                                                  \
      }                                                                                                                \
    } else {                                                                                                           \
      try {                                                                                                            \
        bool res = test::expect_str_not_equal_builtin(A, B, __FILE__, __LINE__, #A, #B);                               \
        if (!res)                                                                                                      \
          std::printf(COMMENT "\r\n");                                                                                 \
        return res;                                                                                                    \
      } catch (std::exception & e) {                                                                                   \
        using namespace test;                                                                                          \
        std::lock_guard<std::mutex> lock(test::mtx);                                                                   \
        std::printf("#%lu [\e[31mFAIL\e[39m] At %s:%i due to std exception ( "                                         \
                    "%s ). Terminating ...\r\n",                                                                       \
                    asserts_counter, __FILE__, __LINE__, e.what());                                                    \
        asserts_counter++;                                                                                             \
        test_results.push_back(std::make_tuple(asserts_counter, false, "none", "none",                                 \
                                               std::string(__FILE__) + ":" + std::to_string(__LINE__),                 \
                                               std::string(#A), std::string(#B)));                                     \
        return false;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
  }(A, B))

#define TEST(TestSuiteName, TestCaseName)                                                                              \
  volatile void __attribute__((used, weak)) test_suite_##TestSuiteName##_test_case_##TestCaseName##_code();            \
  volatile void __attribute__((used)) test_suite_##TestSuiteName##_##test_case_##TestCaseName() {                      \
    static std::string *p_ts_name = &test::ts_name, *p_tc_name = &test::tc_name;                                       \
    static std::function<std::pair<std::string *, std::string *>(void)> f =                                            \
        []() -> std::pair<std::string *, std::string *> {                                                              \
      return {p_ts_name, p_tc_name};                                                                                   \
    };                                                                                                                 \
    test::ts_name = #TestSuiteName;                                                                                    \
    test::tc_name = #TestCaseName;                                                                                     \
    std::unique_lock<std::mutex> lock(test::mtx);                                                                      \
    while (!test::notified) {                                                                                          \
      test::sync_var.wait(lock);                                                                                       \
    }                                                                                                                  \
    test::resolv_ts_tc_names = &f;                                                                                     \
    if (test::report.find(#TestSuiteName) == test::report.end())                                                       \
      test::report.insert(std::make_pair(                                                                              \
          #TestSuiteName,                                                                                              \
          std::map<std::string, std::vector<std::tuple<bool, std::string, std::string, std::string>>>()));             \
    if (test::report.at(#TestSuiteName).find(#TestCaseName) == test::report.at(#TestSuiteName).end())                  \
      test::report.at(#TestSuiteName)                                                                                  \
          .insert(                                                                                                     \
              std::make_pair(#TestCaseName, std::vector<std::tuple<bool, std::string, std::string, std::string>>()));  \
    if (opts.verbose_level > 1)                                                                                        \
      std::printf("\r\nRunning %s : %s ... \r\n\r\n", #TestSuiteName, #TestCaseName);                                  \
    lock.unlock();                                                                                                     \
    test::notified = false;                                                                                            \
    test_suite_##TestSuiteName##_test_case_##TestCaseName##_code();                                                    \
    test::notified = true;                                                                                             \
    test::sync_var.notify_one();                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  volatile const void *__attribute__((used, section("testcases")))                                                     \
      test_suite_##TestSuiteName##_##test_case_##TestCaseName##_ptr =                                                  \
          reinterpret_cast<volatile const void *>(test_suite_##TestSuiteName##_##test_case_##TestCaseName);            \
  volatile void __attribute__((used)) test_suite_##TestSuiteName##_test_case_##TestCaseName##_code()

#endif /* TEST_HPP */
