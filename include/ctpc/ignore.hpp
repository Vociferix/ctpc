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
    static constexpr auto call(T&& self, I&& input) {
        return std::forward<T>(self).parser_(std::forward<I>(input)).map([] ([[maybe_unused]] auto&&... vals) -> void {});
    }

  public:
    explicit constexpr IgnoreParser(P&& parser)
        : parser_(std::forward<P>(parser)) {}

    template <ParseableBy<P> I>
    constexpr auto operator()(I&& input) & {
        return call(*this, std::forward<I>(input));
    }

    template <ParseableBy<P> I>
    constexpr auto operator()(I&& input) const& {
        return call(*this, std::forward<I>(input));
    }

    template <ParseableBy<P> I>
    constexpr auto operator()(I&& input) && {
        return call(std::move(*this), std::forward<I>(input));
    }
};

}

struct Ignore {
    template <typename P>
    constexpr auto operator()(P&& parser) const -> detail::IgnoreParser<P> {
        return detail::IgnoreParser<P>{std::forward<P>(parser)};
    }
};

static constexpr Ignore ignore{};

}

#endif
