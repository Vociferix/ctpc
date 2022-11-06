#ifndef CTPC_PRECEDED_HPP
#define CTPC_PRECEDED_HPP

#include "seq.hpp"
#include "ignore.hpp"

namespace ctpc {

struct Preceded {
    template <typename Prefix, typename P>
    constexpr auto operator()(Prefix&& prefix, P&& parser) const {
        return seq(ignore(std::forward<Prefix>(prefix)), std::forward<P>(parser));
    };
};

/// @ingroup ctpc_combinators
static constexpr Preceded preceded{};

}

#endif
