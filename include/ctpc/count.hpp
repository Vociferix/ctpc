#ifndef CTPC_COUNT_HPP
#define CTPC_COUNT_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

namespace detail {

template <typename P, typename R, typename T>
struct CountParser {
  private:
    size_t count_;
    [[no_unique_address]] P parser_;
    [[no_unique_address]] R reduce_;
    [[no_unique_address]] T init_;

    template <ParseableBy<P> I, typename Self>
    static constexpr auto init(Self& self) {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, utils::DefaultReduceInit>) {
            using item_t = decltype(*self.parser_(std::declval<I>()));
            using accum_t = decltype(self.reduce_({}, std::declval<item_t>()));
            if constexpr (utils::Reservable<accum_t>) {
                accum_t accum{};
                accum.reserve(self.count_);
                return accum;
            } else {
                return accum_t{};
            }
        } else if constexpr (std::invocable<T, size_t>) {
            return self.init_(self.count_);
        } else if constexpr (std::invocable<T>) {
            return self.init_();
        } else {
            return self.init_;
        }
    }

    template <typename Self, ParseableBy<P> I>
    static constexpr auto call(Self&& self, I input) {
        auto accum = init<T>(self);
        std::ranges::subrange in{input};
        for (size_t i = 0; i < self.count_; ++i) {
            auto res = self.parser_(in);
            if (!res) {
                return fail<decltype(accum)>(input);
            }
            in = res.remaing();
            accum = utils::invoke_unpacked(self.reduce_, *std::move(res));
        }
        return pass<decltype(accum)>(in, std::move(accum));
    }

  public:
    constexpr CountParser(P&& parser, size_t count, R&& reduce, T&& init)
        : count_(count),
          parser_(std::forward<P>(parser)),
          reduce_(std::forward<R>(reduce)),
          init_(std::forward<T>(init)) {}

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) & {
        return call(*this, input);
    }

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) const& {
        return call(*this, input);
    }

    template <ParseableBy<P> I>
    constexpr auto operator()(I input) && {
        return call(std::move(*this), input);
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
