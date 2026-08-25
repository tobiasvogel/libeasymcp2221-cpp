/**
 * @file TestHarness.h
 * @brief Minimal dependency-free unit-test helpers.
 */

#ifndef LIBEASYMCP2221_CPP_TEST_HARNESS_H
#define LIBEASYMCP2221_CPP_TEST_HARNESS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace test_harness {

inline void fail(
    const char* expression,
    const char* file,
    int line,
    const std::string& detail = {})
{
    std::ostringstream message;
    message << file << ':' << line << ": assertion failed: " << expression;
    if (!detail.empty()) {
        message << " (" << detail << ')';
    }
    throw std::runtime_error(message.str());
}

template<typename Left, typename Right>
void expectEqual(
    const Left& left,
    const Right& right,
    const char* expression,
    const char* file,
    int line)
{
    if (!(left == right)) {
        fail(expression, file, line);
    }
}

template<typename Function>
void run(const char* name, Function&& function, int& failures)
{
    try {
        function();
        std::cout << "[PASS] " << name << '\n';
    }
    catch (const std::exception& error) {
        ++failures;
        std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
    }
    catch (...) {
        ++failures;
        std::cerr << "[FAIL] " << name << ": unknown exception\n";
    }
}

} // namespace test_harness

#define EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            ::test_harness::fail(#expr, __FILE__, __LINE__); \
        } \
    } while (false)

#define EXPECT_FALSE(expr) EXPECT_TRUE(!(expr))

#define EXPECT_EQ(left, right) \
    ::test_harness::expectEqual( \
        (left), (right), #left " == " #right, __FILE__, __LINE__)

#endif
