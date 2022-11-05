#ifndef CTPC_VERBATIM_HPP
#define CTPC_VERBATIM_HPP

#include <algorithm>
#include <ranges>
#include <span>
#include <string_view>
#include <ctll/fixed_string.hpp>

#include "input.hpp"
#include "parser.hpp"
#include "parse_result.hpp"
#include "const_input.hpp"
#include "utf.hpp"

namespace ctpc {

template <ConstInput MATCH, typename = void>
struct Verbatim {
    using match_type = std::remove_cvref_t<decltype(MATCH)>;

    template <InputOf<typename std::remove_cvref_t<decltype(MATCH)>::value_type> I>
    constexpr auto operator()(I input) const {
        using ret_t = std::span<typename match_type::value_type, match_type::length>;
        std::ranges::subrange in{input};
        ret_t match{MATCH.input};
        auto ibegin = std::ranges::begin(in);
        auto iend = std::ranges::end(in);
        auto mbegin = std::ranges::begin(match);
        auto mend = std::ranges::end(match);
        while (ibegin != iend && mbegin != mend) {
            if (*ibegin != *mbegin) {
                return fail<ret_t>(input);
            }
            ++ibegin;
            ++mbegin;
        }
        if (mbegin != mend) {
            return fail<ret_t>(input);
        }
        return pass<ret_t>(
            std::ranges::subrange(ibegin, iend),
            match
        );
    }
};

namespace detail {

template <typename To, typename Range>
constexpr size_t verbatim_converted_size(Range&& range) {
    size_t count = 0;
    std::ranges::subrange(std::ranges::begin(range), std::ranges::end(range) - 1);
    for ([[maybe_unused]] auto c : utils::utf_convert<To>(std::ranges::subrange(std::ranges::begin(range), std::ranges::end(range) - 1))) {
        ++count;
    }
    return count;
}

template <typename To, size_t N, typename Range>
constexpr std::array<To, N> verbatim_convert(Range&& range) {
    std::array<To, N> ret{};
    auto converted = utils::utf_convert<To>(std::forward<Range>(range));
    auto it = std::ranges::begin(converted);
    for (size_t i = 0; i < N - 1; ++i, ++it) {
        ret[i] = *it;
    }
    return ret;
}

}

template <ConstInput MATCH>
struct Verbatim<MATCH, std::enable_if_t<utils::is_text_char_v<typename std::remove_cvref_t<decltype(MATCH)>::value_type>>> {
    template <typename To>
    static constexpr auto converted_match = detail::verbatim_convert<To, detail::verbatim_converted_size<To>(MATCH) + 1>(MATCH);

    template <TextInput I>
    constexpr ParseResultOf<std::basic_string_view<std::remove_cvref_t<decltype(*std::ranges::begin(std::declval<I>()))>>, I>
    operator()(I input) const {
        using input_char = std::remove_cvref_t<decltype(*std::ranges::begin(input))>;
        std::ranges::subrange in{input};
        std::basic_string_view<input_char> match{converted_match<input_char>.data()};
        auto ibegin = std::ranges::begin(in);
        auto iend = std::ranges::end(in);
        auto mbegin = std::ranges::begin(match);
        auto mend = std::ranges::end(match);
        while (ibegin != iend && mbegin != mend) {
            if (*ibegin != *mbegin) {
                return fail<std::basic_string_view<input_char>>(input);
            }
            ++ibegin;
            ++mbegin;
        }
        if (mbegin != mend) {
            return fail<std::basic_string_view<input_char>>(input);
        }
        return pass<std::basic_string_view<input_char>>(
            std::ranges::subrange(ibegin, iend),
            match
        );
    }
};

template <ConstInput MATCH>
static constexpr Verbatim<MATCH> verbatim{};

}

#endif
