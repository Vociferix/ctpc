#ifndef CTPC_VERBATIM_HPP
#define CTPC_VERBATIM_HPP

#include <ranges>
#include <ctll/fixed_string.hpp>

#include "input.hpp"
#include "parser.hpp"
#include "parse_result.hpp"
#include "const_input.hpp"

namespace ctpc {

namespace detail {

template <typename T, typename M>
concept VerbatimTextInput = TextInput<T> && TextInput<M>;

template <typename T, typename M>
concept VerbatimStringyInput = InputOf<T, typename M::value_type> && (InputOf<T, unsigned char> || InputOf<T, uint8_t>);

template <typename T, typename M>
concept VerbatimNormalInput = InputOf<T, typename M::value_type> && !TextInput<T> && !InputOf<T, unsigned char> && !InputOf<T, uint8_t>;

}

template <ConstInput MATCH>
struct Verbatim {
    using match_type = std::remove_cvref_t<decltype(MATCH)>;

    constexpr auto operator()(detail::VerbatimTextInput<match_type> auto input) const {
        using input_char_t = std::remove_cvref_t<std::ranges::range_value_t<std::remove_cvref_t<decltype(input)>>>;

        if constexpr (std::is_same_v<input_char_t, std::remove_cvref_t<typename match_type::value_type>>) {
            const auto& match = MATCH;
            using pass_t = decltype(match);

            auto ibegin = std::ranges::begin(input);
            auto iend = std::ranges::end(input);
            const auto* mbegin = match.input;
            const auto* mend = mbegin + (match_type::length - 1);
            while (ibegin != iend && mbegin != mend) {
                if (*ibegin != *mbegin) {
                    return fail<pass_t>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
                }
            }
            if (mbegin != mend) {
                return fail<pass_t>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
            }

            return pass<pass_t>(std::ranges::subrange(ibegin, iend), std::ranges::subrange(match));
        } else {
            const auto& orig_match = MATCH;
            const auto match_fixed_string = orig_match.to_fixed_string();
            const auto& match = match_fixed_string;
            input_char_t chars[match_type::length] = {};
            auto ibegin = std::ranges::begin(input);
            auto iend = std::ranges::end(input);
            for (size_t i = 0; i < match_type::length; ++i, ++ibegin) {
                if (!(ibegin != iend)) {
                    return fail<decltype(orig_match)>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
                }
                chars[i] = *ibegin;
            }
            if (!match.is_same_as(ctll::fixed_string(chars))) {
                return fail<decltype(orig_match)>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
            }
            return pass<decltype(orig_match)>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)), orig_match);
        }
    }

    constexpr auto operator()(detail::VerbatimStringyInput<match_type> auto input) const {
        const auto& match = MATCH;
        using pass_t = decltype(match);

        auto ibegin = std::ranges::begin(input);
        auto iend = std::ranges::end(input);
        const auto* mbegin = match.input;
        const auto* mend = mbegin + (match_type::length - 1);
        while (ibegin != iend && mbegin != mend) {
            if (*ibegin != *mbegin) {
                return fail<pass_t>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
            }
        }
        if (mbegin != mend) {
            return fail<pass_t>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
        }

        return pass<pass_t>(std::ranges::subrange(ibegin, iend), std::ranges::subrange(match));
    }

    constexpr auto operator()(detail::VerbatimNormalInput<match_type> auto input) const {
        const auto& match = MATCH;
        using pass_t = decltype(match);

        auto ibegin = std::ranges::begin(input);
        auto iend = std::ranges::end(input);
        auto mbegin = std::ranges::begin(match);
        auto mend = std::ranges::end(match);
        while (ibegin != iend && mbegin != mend) {
            if (*ibegin != *mbegin) {
                return fail<pass_t>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
            }
        }
        if (mbegin != mend) {
            return fail<pass_t>(std::ranges::subrange(std::ranges::begin(input), std::ranges::end(input)));
        } 

        return pass<pass_t>(std::ranges::subrange(ibegin, iend), std::ranges::subrange(match));
    }
};

template <ConstInput MATCH>
static constexpr Verbatim<MATCH> verbatim{};

}

#endif
