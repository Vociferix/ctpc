#ifndef CTPC_TESTS_TEST_UTILS_HPP
#define CTPC_TESTS_TEST_UTILS_HPP

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <string_view>
#include <type_traits>

using namespace std::string_view_literals;

namespace ctpc::detail {

template <typename T>
struct IsSubrange : std::false_type {};

template <typename I, typename S, std::ranges::subrange_kind K>
struct IsSubrange<std::ranges::subrange<I, S, K>> : std::true_type {};

template <typename T>
concept Subrange = IsSubrange<T>::value;

template <typename T>
concept NotSubrange = !IsSubrange<T>::value;

}

constexpr bool operator==(ctpc::detail::Subrange auto lhs, ctpc::detail::Subrange auto rhs) {
    return std::ranges::equal(lhs, rhs);
}

constexpr bool operator==(ctpc::detail::Subrange auto lhs, ctpc::detail::NotSubrange auto rhs) {
    return lhs == std::ranges::subrange(rhs);
}

constexpr bool operator==(ctpc::detail::NotSubrange auto lhs, ctpc::detail::Subrange auto rhs) {
    return std::ranges::subrange(lhs) == rhs;
}

#endif
