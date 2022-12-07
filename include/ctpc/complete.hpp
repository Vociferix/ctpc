#ifndef CTPC_COMPLETE_HPP
#define CTPC_COMPLETE_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename P>
struct CompleteParser {
  private:
    CTPC_NO_UNIQUE_ADDR P parser_;

  public:
    explicit constexpr CompleteParser(P&& parser)
        : parser_(std::forward<P>(parser)) {}

    constexpr auto operator()(Input auto input) const {
        auto res = parser_(input);
        if (res && !res.remaining().empty()) {
            return fail<typename std::remove_cvref_t<decltype(res)>::value_type>(input);
        }
        return res;
    }
}

}

struct Complete {
    template <typename P>
    constexpr auto operator()(P&& parser) const -> detail::CompleteParser<P> {
        return detail::CompleteParser<P>(std::forward<P>(parser));
    }
};

/// @ingroup ctpc_combinators
/// @brief Requires that the passed parser consumes all input
///
/// @details
/// The returned parser succeeds only if the passed parser
/// consumes all input. On success, the result of the inner
/// parser is returned.
static constexpr Complete complete{};

}

#endif
