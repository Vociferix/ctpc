#include <ctpc/utf.hpp>
#include "test_utils.hpp"
#include <string>
#include <iostream>

#define UTF_CONVERT_TEST(name, input, type, output) \
    TEST_CASE(name, "[utf_convert]") { \
        REQUIRE(ctpc::utils::utf_convert<type>(input) == output); \
    }

#define UTF_CONVERT_FROM_TEST(input_type, input) \
    UTF_CONVERT_TEST(#input_type " to char", input, char, "abcd"sv) \
    UTF_CONVERT_TEST(#input_type " to char8_t", input, char8_t, u8"abcd"sv) \
    UTF_CONVERT_TEST(#input_type " to char16_t", input, char16_t, u"abcd"sv) \
    UTF_CONVERT_TEST(#input_type " to char32_t", input, char32_t, U"abcd"sv) \
    UTF_CONVERT_TEST(#input_type " to wchar_t", input, wchar_t, L"abcd"sv)

UTF_CONVERT_FROM_TEST(char, "abcd"sv)
UTF_CONVERT_FROM_TEST(char8_t, u8"abcd"sv)
UTF_CONVERT_FROM_TEST(char16_t, u"abcd"sv)
UTF_CONVERT_FROM_TEST(char32_t, U"abcd"sv)
UTF_CONVERT_FROM_TEST(wchar_t, L"abcd"sv)
