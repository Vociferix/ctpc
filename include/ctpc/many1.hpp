#ifndef CTPC_MANY1_HPP
#define CTPC_MANY1_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename P, typename R, typename T>
struct Many1Parser {
  private:
    CTPC_NO_UNIQUE_ADDR P parser_;
    CTPC_NO_UNIQUE_ADDR R reduce_;
    CTPC_NO_UNIQUE_ADDR T init_;

    template <ParseableBy<P> I>
    constexpr auto init() const {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, utils::DefaultReduceInit>) {
            using item_t = decltype(parser_(std::declval<I>()));
            using accum_t = decltype(utils::invoke_unpacked(reduce_, utils::default_init, std::declval<item_t>()));
            return accum_t{};
        } else if constexpr (std::invocable<T>) {
            return init_();
        } else {
            return init_;
        }
    }

  public:
    constexpr Many1Parser(P&& parser, R&& reduce, T&& init)
        : parser_(std::forward<P>(parser)),
          reduce_(std::forward<R>(reduce)),
          init_(std::forward<T>(init)) {}

    constexpr auto operator()(Input auto input) const {
        std::optional<decltype(init<decltype(input)>())> accum = std::nullopt;
        std::ranges::subrange in{input};
        {
            auto res = parser_(in);
            if (!res) {
                return fail<decltype(init<T>())>(in);
            }
            in = res.remaining();
            accum = utils::invoke_unpacked(reduce_, init<T>(), *std::move(res));
        }

        for (;;) {
            auto res = parser_(in);
            if (!res) {
                break;
            }
            in = res.remaining();
            accum = utils::invoke_unpacked(reduce_, *std::move(accum), *std::move(res));
        }

        return pass<decltype(accum)>(in, *std::move(accum));
    }
};

}

struct Many1 {
    template <typename P, typename R, typename T>
    constexpr auto operator()(P&& parser,
                              R&& reduce = utils::default_reduce,
                              T&& init = utils::default_reduce_init) const -> detail::Many1Parser<P, R, T> {
        return detail::Many1Parser<P, R, T>(std::forward<P>(parser), std::forward<R>(reduce), std::forward<T>(init));
    }
};

/// @ingroup ctpc_combinators
static constexpr Many1 many1{};

}

#endif
