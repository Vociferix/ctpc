#ifndef CTPC_MAP_HPP
#define CTPC_MAP_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

#include <tuple>

namespace ctpc {

namespace detail {

template <typename P, typename M>
struct MapParser {
  private:
    P parser_;
    M mapper_;

    template <typename T, ParseableBy<P> I>
    static constexpr auto call(T&& self, I input) {
        return self.parser_(input).map([&self] (auto&&... value) {
            return utils::invoke_unpacked(self.mapper_, std::forward<decltype(value)>(value)...);
        });
    }

  public:
    explicit constexpr MapParser(P&& parser, M&& mapper)
        : parser_(std::forward<P>(parser)),
          mapper_(std::forward<M>(mapper)) {}

    constexpr auto operator()(ParseableBy<P> auto input) & {
        return call(*this, input);
    }

    constexpr auto operator()(ParseableBy<P> auto input) const& {
        return call(*this, input);
    }

    constexpr auto operator()(ParseableBy<P> auto input) && {
        return call(std::move(*this), input);
    }
};

}

struct Map {
    template <typename P, typename M>
    constexpr auto operator()(P&& parser, M&& mapper) const -> detail::MapParser<P, M> {
        return detail::MapParser<P, M>(std::forward<P>(parser), std::forward<M>(mapper));
    }
};

static constexpr Map map{};

}

#endif
