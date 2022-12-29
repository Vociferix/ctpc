#ifndef CTPC_REINTERPRET_HPP
#define CTPC_REINTERPRET_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

#include "utils.hpp"

#include <algorithm>
#include <bit>
#include <ranges>

namespace ctpc {

template <typename T>
struct Reinterpret {
    template <Input I>
    constexpr auto operator()(I input) const {
        using In = std::ranges::range_value_t<I>;

        if constexpr (sizeof(In) == sizeof(T)) {
            return pass<void>(input | std::ranges::views::transform([] (const auto& value) {
                return std::bit_cast<T>(value);
            }));
        } else if constexpr (sizeof(In) < sizeof(T)) {
            static_assert(sizeof(T) % sizeof(In) == 0, "Invalid reinterpretation");
            return pass<void>(input | std::ranges::views::transform([i = size_t(0), tmp = std::array<In, sizeof(T) / sizeof(In)>()](const auto& value) mutable -> std::optional<std::array<In, sizeof(T) / sizeof(In)>> {
                tmp[i++] = value;
                if (i == sizeof(T) / sizeof(In)) {
                    i = 0;
                    return tmp;
                }
                return std::nullopt;
            }) | std::ranges::views::filter([](const auto& value) { return value.has_value(); }) | std::ranges::views::transform([](auto&& value) -> T {
                return std::bit_cast<T>(*std::forward<decltype(value)>(value));
            }));
        } else {
            static_assert(sizeof(In) % sizeof(T) == 0, "Invalid reinterpretation");
            return pass<void>(input | std::ranges::views::transform([](const auto& value) {
                return std::bit_cast<std::array<T, sizeof(In) / sizeof(T)>>(value);
            }) | std::ranges::views::join);
        }
    }
};

/// @ingroup ctpc_parsers
template <typename T>
static constexpr Reinterpret<T> reinterpret{};

}

#endif
