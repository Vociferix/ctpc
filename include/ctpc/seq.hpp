#ifndef CTPC_SEQ_HPP
#define CTPC_SEQ_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename... P>
struct SeqParser;

template <>
struct SeqParser<> {
  private:
    template <Input I, typename... Ret>
    constexpr auto call(I input, Ret&&... ret) const {
        if constexpr (sizeof...(Ret) == 0) {
            return pass<void>(input);
        } else if constexpr (sizeof...(Ret) == 1) {
            return pass<Ret...>(input, std::forward<Ret>(ret)...);
        } else {
            return pass<std::tuple<Ret...>>(input, std::forward<Ret>(ret)...);
        }
    }

    template <typename...>
    friend struct SeqParser;

  public:
    constexpr SeqParser() = default;

    constexpr auto operator()(Input auto input) const {
        return call(input);
    }
};

template <typename P1, typename... PN>
struct SeqParser<P1, PN...> {
  private:
    [[no_unique_address]] P1 parser_;
    [[no_unique_address]] SeqParser<PN...> inner_;

    template <typename...>
    friend struct SeqParser;

    template <typename T, ParseableBy<P1> I, typename... Ret>
    static constexpr auto call_impl(T&& self, I input, Ret&&... ret) {
        auto res = self.parser_(input);
        if constexpr (std::is_void_v<typename std::remove_cvref_t<decltype(res)>::value_type>) {
            if (res) {
                return std::forward<T>(self).inner_.call(res.remaining(), std::forward<Ret>(ret)...);
            } else {
                using res_t = std::remove_cvref_t<decltype(std::forward<T>(self).inner_.call(res.remaining(), std::forward<Ret>(ret)...))>;
                return fail<typename res_t::value_type>(input);
            }
        } else {
            if (res) {
                auto rem = res.remaining();
                return std::forward<T>(self).inner_.call(rem, std::forward<Ret>(ret)..., *std::move(res));
            } else {
                using res_t = std::remove_cvref_t<decltype(std::forward<T>(self).inner_.call(res.remaining(), std::forward<Ret>(ret)..., *std::move(res)))>;
                return fail<typename res_t::value_type>(input);
            }
        }
    }

    template <typename T, ParseableBy<P1> I>
    static constexpr auto call_first(T&& self, I input) {
        auto res = call_impl(std::forward<T>(self), input);
        if (res) {
            return res;
        } else {
            return fail<typename std::remove_cvref_t<decltype(res)>::value_type>(input);
        }
    }

    template <ParseableBy<P1> I, typename... Ret>
    constexpr auto call(I input, Ret&&... ret) & {
        return call_impl(*this, input, std::forward<Ret>(ret)...);
    }

    template <ParseableBy<P1> I, typename... Ret>
    constexpr auto call(I input, Ret&&... ret) const& {
        return call_impl(*this, input, std::forward<Ret>(ret)...);
    }

    template <ParseableBy<P1> I, typename... Ret>
    constexpr auto call(I input, Ret&&... ret) && {
        return call_impl(std::move(*this), input, std::forward<Ret>(ret)...);
    }

  public:
    explicit constexpr SeqParser(P1&& parser, PN&&... inner)
        : parser_(std::forward<P1>(parser)),
          inner_(std::forward<PN>(inner)...) {}

    constexpr auto operator()(ParseableBy<P1> auto input) & {
        return call_first(*this, input);
    }

    constexpr auto operator()(ParseableBy<P1> auto input) const& {
        return call_first(*this, input);
    }

    constexpr auto operator()(ParseableBy<P1> auto input) && {
        return call_first(std::move(*this), input);
    }
};

}

struct Seq {
    template <typename... P>
    constexpr auto operator()(P&&... elems) const -> detail::SeqParser<P...> {
        return detail::SeqParser<P...>(std::forward<P>(elems)...);
    }
};

/// @brief Combines parsers into a sequence
/// @ingroup ctpc_combinators
///
/// Combinator signature:
/// ```
/// seq(Parser parser...) -> std::tuple<...>
/// ```
///
/// Any number of parsers can be passed to `seq` to create a parser that will
/// parse all each component parser in order, and fail if any component fails.
/// The results from each component parser are combined into a `std::tuple`.
/// If a component's result is `void`, that result is skiped in the output
/// `std::tuple`. For example, if the component parsers return parse results
/// of `int`, `void`, and `int`, `seq`'s parse result will be
/// `std::tuple<int, int>`. `seq` may be used in combination with `ignore` to
/// ensure that a component's result is not included in the combined result.
///
/// The following is an example that combines, `seq`, `ignore`, and `map` to
/// combine three parsers, disregard the result of the second, and combine the
/// results from the first and third parsers into a single non-tuple value.
/// ```
/// static constexpr auto my_parser = map(
///     seq(first, ignore(second), third),
///     [] (auto first, auto third) { return first + third; }
/// );
/// ```
static constexpr Seq seq{};

}

#endif
