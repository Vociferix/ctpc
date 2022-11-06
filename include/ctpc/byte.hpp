#ifndef CTPC_BYTE_HPP
#define CTPC_BYTE_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

struct Byte {
    template <ByteInput I>
    constexpr auto operator()(I input) const {
        auto i = std::ranges::begin(input);
        auto e = std::ranges::end(input);
        if (i == e) {
            return fail<std::byte>(input);
        } else {
            auto val = static_cast<std::byte>(*i);
            ++i;
            return pass<std::byte>(std::ranges::subrange(i, e), val);
        }
    }
};

/// @ingroup ctpc_parsers
static constexpr Byte byte{};

}

#endif
