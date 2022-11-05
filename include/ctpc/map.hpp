#ifndef CTPC_MAP_HPP
#define CTPC_MAP_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"

#include <tuple>

namespace ctpc {

namespace detail {

template <typename T>
struct is_tuple : std::false_type {};

template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type {};

template <typename M, typename T, size_t... IDX>
constexpr decltype(auto) map_tuple_impl(M& mapper, T&& values, [[maybe_unused]] std::index_sequence<IDX...> idx) {
    return mapper(std::forward<typename std::tuple_element<IDX, std::remove_cvref_t<T>>::type>(std::get<IDX>(values))...);
}

template <typename M, typename T>
constexpr decltype(auto) map_impl(M& mapper) {
    return mapper();
}

template <typename M, typename T>
constexpr decltype(auto) map_impl(M& mapper, T&& value) {
    if constexpr (is_tuple<std::remove_cvref_t<T>>::value) {
        return map_tuple_impl(mapper, std::forward<T>(value), std::make_index_sequence<std::tuple_size<std::remove_cvref_t<T>>::value>{});
    } else {
        return mapper(std::forward<T>(value));
    }
}

template <typename P, typename M>
struct MapParser {
  private:
    P parser_;
    M mapper_;

    template <typename T, ParseableBy<P> I>
    static constexpr auto call(T&& self, I input) {
        return self.parser_(input).map([&self] (auto&&... value) {
            return map_impl(self.mapper_, std::forward<decltype(value)>(value)...);
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
