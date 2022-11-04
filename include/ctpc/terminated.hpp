#ifndef CTPC_TERMINATED_HPP
#define CTPC_TERMINATED_HPP

#include "seq.hpp"
#include "ignore.hpp"

namespace ctpc {

struct Terminated {
    template <typename P, typename Suffix>
    constexpr auto operator()(P&& parser, Suffix&& suffix) const {
        return seq(std::forward<P>(parser), ignore(std::forward<Suffix>(suffix)));
    }
};

static constexpr Terminated terminated{};

}

#endif
