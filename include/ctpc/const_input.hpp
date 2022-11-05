#ifndef CTPC_CONST_INPUT_HPP
#define CTPC_CONST_INPUT_HPP

#include "input.hpp"

#include <algorithm>
#include <ranges>
#include <span>

namespace ctpc {

template <typename T, size_t N>
struct ConstInput {
    using value_type = T;
    static constexpr size_t length = N;

    T input[N];

    constexpr ConstInput(const T (&in)[N]) : input{} {
        std::copy(std::ranges::begin(in), std::ranges::end(in), std::ranges::begin(input));
    }

    constexpr ConstInput(std::span<const T, N> in) : input{} {
        std::copy(std::ranges::begin(in), std::ranges::end(in), std::ranges::begin(input));
    }

    constexpr decltype(auto) begin() const {
        return std::ranges::begin(input);
    }

    constexpr decltype(auto) end() const {
        return std::ranges::end(input);
    }
};

template <typename T, size_t N>
ConstInput(T (&in)[N]) -> ConstInput<std::remove_const_t<T>, N>;

template <typename T, size_t N>
ConstInput(std::span<T, N>) -> ConstInput<std::remove_const_t<T>, N>;

template <typename T, size_t N>
ConstInput(const std::array<T, N>&) -> ConstInput<T, N>;

}

#endif
