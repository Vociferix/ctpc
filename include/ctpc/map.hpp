#ifndef CTPC_MAP_HPP
#define CTPC_MAP_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

#include <tuple>

namespace ctpc {

namespace detail {

template <typename P, typename M>
struct MapParser {
  private:
    CTPC_NO_UNIQUE_ADDR P parser_;
    CTPC_NO_UNIQUE_ADDR M mapper_;

  public:
    explicit constexpr MapParser(P&& parser, M&& mapper)
        : parser_(std::forward<P>(parser)),
          mapper_(std::forward<M>(mapper)) {}

    constexpr auto operator()(ParseableBy<P> auto input) const {
        return parser_(input).map([this] (auto&&... value) {
            return utils::invoke_unpacked(mapper_, std::forward<decltype(value)>(value)...);
        });
    }
};

}

struct Map {
    template <typename P, typename M>
    constexpr auto operator()(P&& parser, M&& mapper) const -> detail::MapParser<P, M> {
        return detail::MapParser<P, M>(std::forward<P>(parser), std::forward<M>(mapper));
    }
};

/// @brief Transforms the result of a parser
/// @ingroup ctpc_combinators
///
/// Combinator signature:
/// ```
/// map(Parser parser, U(T...) transform) -> U
/// ```
///
/// The result of the provided parser is transformed using the provided
/// callable. When the result of the provided parser is a `std::tuple`,
/// the elements of the `std::tuple` are passed to the callable as
/// separate arguments. When the result of the parser is `void` or
/// `std::tuple<>` (the empty tuple), the callable will be passed no
/// arguments.
///
/// When used in combination with `seq` (e.g.
/// `map(seq(...), callback)`), the callable will be passed an argument,
/// for each component of the `seq` call, since `seq` returns a
/// `std::tuple` of the components' results. Additionally, components
/// that have a result of type `void` will not have a corresponding
/// argument that's passed to the callable. This behavior can be
/// elicited by wrapping a component parser in `ignore`, if the
/// component doesn't already have a `void` result.
static constexpr Map map{};

}

#endif
