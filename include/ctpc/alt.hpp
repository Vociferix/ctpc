#ifndef CTPC_ALT_HPP
#define CTPC_ALT_HPP

#include "input.hpp"
#include "parser.hpp"
#include "parse_result.hpp"

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
    constexpr auto call(I&& input) const -> ParseResultOf<Ret, I> {
        return fail<Ret>(std::forward<I>(input));
    }

  public:
    constexpr AltParser() = default;

    template <Input I>
    constexpr auto operator()(I&& input) const {
        return call<void>(std::forward<I>(input));
    }
};

template <typename P1, typename... PN>
struct AltParser<P1, PN...> {
  private:
    [[no_unique_address]] P1 parser_;
    [[no_unique_address]] AltParser<PN...> inner_;

    template <typename...>
    friend struct AltParser;

    template <typename Ret, typename T, ParseableBy<P1> I>
    static constexpr auto call_impl(T&& self, I&& input) -> ParseResultOf<Ret, I> {
        auto res = self.parser_(input);
        if (res) {
            return res;
        } else {
            return std::forward<T>(self).inner_.template call<Ret>(std::forward<I>(input));
        }
    }

    template <typename Ret, ParseableBy<P1> I>
    constexpr auto call(I&& input) & -> ParseResultOf<Ret, I> {
        return call_impl<Ret>(*this, std::forward<I>(input));
    }

    template <typename Ret, ParseableBy<P1> I>
    constexpr auto call(I&& input) const& -> ParseResultOf<Ret, I> {
        return call_impl<Ret>(*this, std::forward<I>(input));
    }

    template <typename Ret, ParseableBy<P1> I>
    constexpr auto call(I&& input) && -> ParseResultOf<Ret, I> {
        return call_impl<Ret>(std::move(*this), std::forward<I>(input));
    }

    template <typename I>
    using ret_t = typename std::remove_cvref_t<decltype(std::declval<P1&>()(std::declval<I>()))>::value_type;

  public:
    explicit constexpr AltParser(P1&& parser, PN&&... inner)
        : parser_(std::forward<P1>(parser)),
          inner_(std::forward<PN>(inner)...) {}

    template <ParseableBy<P1> I>
    constexpr auto operator()(I&& input) & {
        return call<ret_t<I>>(std::forward<I>(input));
    }

    template <ParseableBy<P1> I>
    constexpr auto operator()(I&& input) const& {
        return call<ret_t<I>>(std::forward<I>(input));
    }

    template <ParseableBy<P1> I>
    constexpr auto operator()(I&& input) && {
        return std::move(*this).template call<ret_t<I>>(std::forward<I>(input));
    }
};

}

struct Alt {
    template <typename... P>
    constexpr auto operator()(P&&... parsers) const -> detail::AltParser<P...> {
        return detail::AltParser<P...>(std::forward<P>(parsers)...);
    }
};

static constexpr Alt alt{};

}

#endif