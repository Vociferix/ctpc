#ifndef CTPC_CONST_INPUT_HPP
#define CTPC_CONST_INPUT_HPP

#include "input.hpp"

#include <algorithm>
#include <ranges>

namespace ctpc {

template <typename T, size_t N>
struct ConstInput {
    using value_type = T;
    static constexpr size_t length = N;

    T input[N];

    constexpr ConstInput(const T (&in)[N]) : input{} {
        std::copy(std::ranges::begin(in), std::ranges::end(in), std::ranges::begin(input));
    }

    constexpr decltype(auto) begin() const {
        return std::ranges::begin(input);
    }

    constexpr decltype(auto) end() const {
        return std::ranges::end(input);
    }

    constexpr auto to_fixed_string() const {
        return ctll::fixed_string(input);
    }
};

template <typename T, size_t N>
ConstInput(const T (&in)[N]) -> ConstInput<T, N>;

}

#endif
