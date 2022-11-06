#ifndef CTPC_DELIMITED_HPP
#define CTPC_DELIMITED_HPP

#include "seq.hpp"
#include "ignore.hpp"

namespace ctpc {

struct Delimited {
    template <typename Prefix, typename P, typename Suffix>
    constexpr auto operator()(Prefix&& prefix, P&& parser, Suffix& suffix) const {
        return seq(
            ignore(std::forward<Prefix>(prefix)),
            std::forward<P>(parser),
            ignore(std::forward<Suffix>(suffix))
        );
    }
};

/// @ingroup ctpc_combinators
static constexpr Delimited delimited{};

}

#endif
