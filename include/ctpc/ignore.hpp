#ifndef CTPC_IGNORE_HPP
#define CTPC_IGNORE_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

namespace detail {

template <typename P>
struct IgnoreParser {
  private:
    P parser_;

    template <typename T, ParseableBy<P> I>
    static constexpr auto call(T&& self, I input) {
        return std::forward<T>(self).parser_(input).map([] ([[maybe_unused]] auto&&... vals) -> void {});
    }

  public:
    explicit constexpr IgnoreParser(P&& parser)
        : parser_(std::forward<P>(parser)) {}

    constexpr auto operator()(ParseableBy<P> auto input) & {
        return call(*this, input);
    }

    constexpr auto operator()(ParseableBy<P> auto input) const& {
        return call(*this, input);
    }

    constexpr auto operator()(ParseableBy<P> auto input) && {
        return call(std::move(*this), input);
    }
};

}

struct Ignore {
    template <typename P>
    constexpr auto operator()(P&& parser) const -> detail::IgnoreParser<P> {
        return detail::IgnoreParser<P>{std::forward<P>(parser)};
    }
};

/// @ingroup ctpc_combinators
static constexpr Ignore ignore{};

}

#endif
