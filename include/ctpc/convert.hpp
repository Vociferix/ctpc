#ifndef CTPC_CONVERT_HPP
#define CTPC_CONVERT_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

#include "map.hpp"

namespace ctpc {

template <typename T>
struct Convert {
    template <typename P>
    constexpr auto operator()(P&& parser) const {
        return map(std::forward<P>(parser), [](auto&&... args) {
            return T{std::forward<decltype(args)>(args)...};
        });
    }
};

/// @ingroup ctpc_combinators
template <typename T>
static constexpr Convert<T> convert{};

}

#endif
