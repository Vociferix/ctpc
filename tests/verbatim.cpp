#include <ctpc/verbatim.hpp>
#include "test_utils.hpp"

using namespace ctpc;

TEST_CASE("single character char", "[verbatim]") {
    const auto parser = verbatim<"a">;
    auto res = parser("abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == "a"sv);
    REQUIRE(res.remaining() == "bcd"sv);
}

TEST_CASE("single character char8_t", "[verbatim]") {
    const auto parser = verbatim<"a">;
    auto res = parser(u8"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == u8"a"sv);
    REQUIRE(res.remaining() == u8"bcd"sv);
}

TEST_CASE("single character char16_t", "[verbatim]") {
    const auto parser = verbatim<"a">;
    auto res = parser(u"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == u"a"sv);
    REQUIRE(res.remaining() == u"bcd"sv);
}

TEST_CASE("single character char32_t", "[verbatim]") {
    const auto parser = verbatim<"a">;
    auto res = parser(U"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == U"a"sv);
    REQUIRE(res.remaining() == U"bcd"sv);
}

TEST_CASE("single character wchar_t", "[verbatim]") {
    const auto parser = verbatim<"a">;
    auto res = parser(L"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == L"a"sv);
    REQUIRE(res.remaining() == L"bcd"sv);
}

TEST_CASE("not single character char", "[verbatim]") {
    const auto parser = verbatim<"b">;
    auto res = parser("abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == "abcd"sv);
}

TEST_CASE("not single character char8_t", "[verbatim]") {
    const auto parser = verbatim<"b">;
    auto res = parser(u8"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == u8"abcd"sv);
}

TEST_CASE("not single character char16_t", "[verbatim]") {
    const auto parser = verbatim<"b">;
    auto res = parser(u"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == u"abcd"sv);
}

TEST_CASE("not single character char32_t", "[verbatim]") {
    const auto parser = verbatim<"b">;
    auto res = parser(U"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == U"abcd"sv);
}

TEST_CASE("not single character wchar_t", "[verbatim]") {
    const auto parser = verbatim<"b">;
    auto res = parser(L"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == L"abcd"sv);
}

TEST_CASE("multi character char", "[verbatim]") {
    const auto parser = verbatim<"ab">;
    auto res = parser("abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == "ab"sv);
    REQUIRE(res.remaining() == "cd"sv);
}

TEST_CASE("multi character char8_t", "[verbatim]") {
    const auto parser = verbatim<"ab">;
    auto res = parser(u8"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == u8"ab"sv);
    REQUIRE(res.remaining() == u8"cd"sv);
}

TEST_CASE("multi character char16_t", "[verbatim]") {
    const auto parser = verbatim<"ab">;
    auto res = parser(u"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == u"ab"sv);
    REQUIRE(res.remaining() == u"cd"sv);
}

TEST_CASE("multi character char32_t", "[verbatim]") {
    const auto parser = verbatim<"ab">;
    auto res = parser(U"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == U"ab"sv);
    REQUIRE(res.remaining() == U"cd"sv);
}

TEST_CASE("multi character wchar_t", "[verbatim]") {
    const auto parser = verbatim<"ab">;
    auto res = parser(L"abcd"sv);
    REQUIRE(res.passed() == true);
    REQUIRE(*res == L"ab"sv);
    REQUIRE(res.remaining() == L"cd"sv);
}

TEST_CASE("not multi character char", "[verbatim]") {
    const auto parser = verbatim<"ac">;
    auto res = parser("abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == "abcd"sv);
}

TEST_CASE("not multi character char8_t", "[verbatim]") {
    const auto parser = verbatim<"ac">;
    auto res = parser(u8"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == u8"abcd"sv);
}

TEST_CASE("not multi character char16_t", "[verbatim]") {
    const auto parser = verbatim<"ac">;
    auto res = parser(u"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == u"abcd"sv);
}

TEST_CASE("not multi character char32_t", "[verbatim]") {
    const auto parser = verbatim<"ac">;
    auto res = parser(U"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == U"abcd"sv);
}

TEST_CASE("not multi character wchar_t", "[verbatim]") {
    const auto parser = verbatim<"ac">;
    auto res = parser(L"abcd"sv);
    REQUIRE(res.passed() == false);
    REQUIRE(res.remaining() == L"abcd"sv);
}
