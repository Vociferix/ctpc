#ifndef CTPC_ALT_HPP
#define CTPC_ALT_HPP

#include "input.hpp"
#include "parser.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename... P>
struct AltParser;

template <>
struct AltParser<> {
  private:
    template <typename...>
    friend struct AltParser;

    template <typename Ret, Input I>
    constexpr auto call(I input) const -> ParseResultOf<Ret, I> {
        return fail<Ret>(input);
    }

  public:
    constexpr AltParser() = default;

    constexpr auto operator()(Input auto input) const {
        return call<void>(input);
    }
};

template <typename P1, typename... PN>
struct AltParser<P1, PN...> {
  private:
    CTPC_NO_UNIQUE_ADDR P1 parser_;
    CTPC_NO_UNIQUE_ADDR AltParser<PN...> inner_;

    template <typename...>
    friend struct AltParser;

    template <typename Ret, ParseableBy<P1> I>
    constexpr auto call(I input) const -> ParseResultOf<Ret, I> {
        auto res = parser_(input);
        if (res) {
            return res;
        } else {
            return inner_.template call<Ret>(input);
        }
    }

    template <typename I>
    using ret_t = typename std::remove_cvref_t<decltype(std::declval<P1&>()(std::declval<I>()))>::value_type;

  public:
    explicit constexpr AltParser(P1&& parser, PN&&... inner)
        : parser_(std::forward<P1>(parser)),
          inner_(std::forward<PN>(inner)...) {}

    constexpr auto operator()(ParseableBy<P1> auto input) const {
        return call<ret_t<decltype(input)>>(input);
    }
};

}

struct Alt {
    template <typename... P>
    constexpr auto operator()(P&&... parsers) const -> detail::AltParser<P...> {
        return detail::AltParser<P...>(std::forward<P>(parsers)...);
    }
};

/// @ingroup ctpc_combinators
static constexpr Alt alt{};

}

#endif
