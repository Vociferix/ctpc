#ifndef CTPC_COUNT_HPP
#define CTPC_COUNT_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename P, typename R, typename T>
struct CountParser {
  private:
    size_t count_;
    CTPC_NO_UNIQUE_ADDR P parser_;
    CTPC_NO_UNIQUE_ADDR R reduce_;
    CTPC_NO_UNIQUE_ADDR T init_;

    template <ParseableBy<P> I>
    constexpr auto init() const {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, utils::DefaultReduceInit>) {
            using item_t = decltype(parser_(std::declval<I>()));
            using accum_t = decltype(utils::invoke_unpacked(reduce_, utils::default_init, std::declval<item_t>()));
            if constexpr (utils::Reservable<accum_t>) {
                accum_t accum{};
                accum.reserve(count_);
                return accum;
            } else {
                return accum_t{};
            }
        } else if constexpr (std::invocable<T, size_t>) {
            return init_(count_);
        } else if constexpr (std::invocable<T>) {
            return init_();
        } else {
            return init_;
        }
    }

  public:
    constexpr CountParser(P&& parser, size_t count, R&& reduce, T&& init)
        : count_(count),
          parser_(std::forward<P>(parser)),
          reduce_(std::forward<R>(reduce)),
          init_(std::forward<T>(init)) {}

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) const& {
        auto accum = init<I>();
        std::ranges::subrange in{input};
        for (size_t i = 0; i < count_; ++i) {
            auto res = parser_(in);
            if (!res) {
                return fail<decltype(accum)>(input);
            }
            in = res.remaining();
            accum = utils::invoke_unpacked(reduce_, std::move(accum), *std::move(res));
        }
        return pass<decltype(accum)>(in, std::move(accum));
    }
};

}

struct Count {
    template <typename P, typename R, typename T>
    constexpr auto operator()(P&& parser,
                              size_t count,
                              R&& reduce = utils::default_reduce,
                              T&& init = utils::default_reduce_init) const -> detail::CountParser<P, R, T> {
        return detail::CountParser<P, R, T>(std::forward<P>(parser), count, std::forward<R>(reduce), std::forward<T>(init));
    }
};

/// @ingroup ctpc_combinators
static constexpr Count count{};

}

#endif
