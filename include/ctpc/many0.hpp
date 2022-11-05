#ifndef CTPC_MANY0_HPP
#define CTPC_MANY0_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

struct Many0DefaultReduce {
    template <typename Item>
    constexpr std::vector<std::remove_cvref_t<Item>> operator()(std::vector<std::remove_cvref_t<Item>> accum, Item&& item) const {
        accum.emplace_back(std::forward<Item>(item));
        return accum;
    }
};

static constexpr Many0DefaultReduce many0_default_reduce{};

struct Many0DefaultInit {};

static constexpr Many0DefaultInit many0_default_init{};

template <typename P, typename R, typename T>
struct Many0Parser {
  private:
    [[no_unique_address]] P parser_;
    [[no_unique_address]] R reduce_;
    [[no_unique_address]] T init_;

    template <ParseableBy<P> I, typename Self>
    static constexpr auto init(Self& self) {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, Many0DefaultInit>) {
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
            in = res.remaining();
            if (!res) {
                break;
            }
            accum = utils::invoke_unpacked(self.reduce_, *std::move(res));
        }
        return pass<decltype(accum)>(in, accum);
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
                              R&& reduce = detail::many0_default_reduce,
                              T&& init = detail::many0_default_init) const -> detail::Many0Parser<P, R, T> {
        return detail::Many0Parser<P, R, T>(std::forward<P>(parser), std::forward<R>(reduce), std::forward<T>(init));
    }
};

static constexpr Many0 many0{};

}

#endif
