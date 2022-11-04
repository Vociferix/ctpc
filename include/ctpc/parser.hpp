#ifndef CTPC_PARSER_HPP
#define CTPC_PARSER_HPP

#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

namespace detail {

template <typename T, typename I>
struct is_parse_result_of : std::false_type {};

template <typename T, typename I>
struct is_parse_result_of<ParseResult<T, std::ranges::iterator_t<I>, std::ranges::sentinel_t<I>>, I>
    : std::true_type {};

template <typename T, typename I>
static inline constexpr bool is_parse_result_of_v = is_parse_result_of<T, I>::value;

template <typename T, typename I>
concept parse_result_of = is_parse_result_of_v<std::remove_cvref_t<T>, std::remove_cvref_t<I>>;

template <typename P, typename I>
concept ParserOf1 = Input<I> && requires(P&& parser, I&& input) {
    { std::forward<P>(parser)(std::forward<I>(input)) } -> parse_result_of<I>;
};

template <typename I, typename P>
concept ParseableBy1 = ParserOf1<P, I>;

}

template <typename P, typename... I>
concept ParserOf = (detail::ParserOf1<P, I> && ... && true);

template <typename I, typename... P>
concept ParseableBy = (detail::ParseableBy1<I, P> && ... && true);

}

#endif
