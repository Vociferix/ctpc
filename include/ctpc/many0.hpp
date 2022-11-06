#ifndef CTPC_MANY0_HPP
#define CTPC_MANY0_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename P, typename R, typename T>
struct Many0Parser {
  private:
    [[no_unique_address]] P parser_;
    [[no_unique_address]] R reduce_;
    [[no_unique_address]] T init_;

    template <ParseableBy<P> I, typename Self>
    static constexpr auto init(Self& self) {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, utils::DefaultReduceInit>) {
            using item_t = decltype(*self.parser_(std::declval<I>()));
            using accum_t = decltype(self.reduce_({}, std::declval<item_t>()));
            return accum_t{};
        } else if constexpr (std::invocable<T>) {
            return self.init_();
        } else {
            return self.init_;
        }
    }

    template <typename Self, ParseableBy<P> I>
    static constexpr auto call(Self&& self, I input) -> ParseResultOf<decltype(init<I>()), I> {
        auto accum = init<I>(self);
        std::ranges::subrange in{input};
        for (;;) {
            auto res = self.parser_(in);
            if (!res) {
                break;
            }
            in = res.remaining();
            accum = utils::invoke_unpacked(self.reduce_, *std::move(res));
        }
        return pass<decltype(accum)>(in, std::move(accum));
    }

  public:
    constexpr Many0Parser(P&& parser, R&& reduce, T&& init)
        : parser_(std::forward<P>(parser)),
          reduce_(std::forward<R>(reduce)),
          init_(std::forward<T>(init)) {}

    constexpr auto operator()(Input auto input) & {
        return call(*this, input);
    }

    constexpr auto operator()(Input auto input) const& {
        return call(*this, input);
    }

    constexpr auto operator()(Input auto input) && {
        return call(std::move(*this), input);
    }
};

}

struct Many0 {
    template <typename P, typename R, typename T>
    constexpr auto operator()(P&& parser,
                              R&& reduce = utils::default_reduce,
                              T&& init = utils::default_reduce_init) const -> detail::Many0Parser<P, R, T> {
        return detail::Many0Parser<P, R, T>(std::forward<P>(parser), std::forward<R>(reduce), std::forward<T>(init));
    }
};

/// @ingroup ctpc_combinators
static constexpr Many0 many0{};

}

#endif
