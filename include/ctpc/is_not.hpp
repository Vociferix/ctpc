#ifndef CTPC_IS_NOT_HPP
#define CTPC_IS_NOT_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

namespace detail {

template <typename P>
struct IsNotParser {
  private:
    [[no_unique_address]] P parser_;

    template <typename T, ParseableBy<P> I>
    static constexpr auto call(T&& self, I input) {
        auto res = std::forward<T>(self).parser_(input);
        if (res) {
            return fail<void>(input);
        } else {
            return pass<void>(input);
        }
    }

  public:
    explicit constexpr IsNotParser(P&& parser)
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

struct IsNot {
    template <typename P>
    constexpr auto operator()(P&& parser) const -> detail::IsNotParser<P> {
        return detail::IsNotParser<P>{std::forward<P>(parser)};
    }
};

/// @ingroup ctpc_combinators
static constexpr IsNot is_not{};

}

#endif
