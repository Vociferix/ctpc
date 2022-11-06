#ifndef CTPC_REGEX_MATCH_HPP
#define CTPC_REGEX_MATCH_HPP

#include <ctre.hpp>
#include <ranges>

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

template <ctll::fixed_string REGEX>
struct RegexMatch {
    constexpr auto operator()(TextInput auto input) const {
        auto begin = std::ranges::begin(input);
        auto end = std::ranges::end(input);
        auto res = ctre::starts_with<REGEX>(begin, end);
        using ret_t = std::remove_cvref_t<decltype(std::ranges::subrange(begin, res.end()))>;
        if (res) {
            return pass<ret_t>(std::ranges::subrange(res.end(), end), std::ranges::subrange(res.begin(), res.end()));
        } else {
            return fail<ret_t>(input);
        }
    }
};

/// @ingroup ctpc_parsers
template <ctll::fixed_string REGEX>
static constexpr RegexMatch<REGEX> regex_match{};

}

#endif
