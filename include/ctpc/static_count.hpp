#ifndef CTPC_STATIC_COUNT_HPP
#define CTPC_STATIC_COUNT_HPP

#include <array>

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

namespace detail {

template <size_t N, typename P>
struct StaticCountParser {
  private:
    [[no_unique_address]] P parser_;

    template <typename Self, ParseableBy<P> I>
    static constexpr auto call(Self&& self, I input) {
        using item_t = std::remove_cvref_t<decltype(*self.parser_(input))>;
        std::array<item_t, N> ret{};
        std::ranges::subrange in{input};

        for (size_t i = 0; i < N; ++i) {
            auto res = self.parser_(in);
            if (!res) {
                return fail<std::array<item_t, N>>(input);
            }
            in = res.remaining();
            ret[i] = *std::move(res);
        }
        return pass<std::array<item_t, N>>(input, std::move(ret));
    }

  public:
    constexpr StaticCountParser(P&& parser)
        : parser_(std::forward<P>(parser)) {}

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) & {
        return call(*this, input);
    }

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) const& {
        return call(*this, input);
    }

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) && {
        return call(std::move(*this), input);
    }
};

}

template <size_t N>
struct StaticCount {
    template <typename P, typename R, typename T>
    constexpr auto operator()(P&& parser) const -> detail::StaticCountParser<N, P> {
        return detail::StaticCountParser<N, P>(std::forward<P>(parser));
    }
};

/// @ingroup ctpc_combinators
template <size_t N>
static constexpr StaticCount<N> static_count{};

}

#endif
