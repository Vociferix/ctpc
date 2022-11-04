#ifndef CTPC_PARSE_RESULT_HPP
#define CTPC_PARSE_RESULT_HPP

#include <initializer_list>
#include <ranges>
#include <type_traits>
#include <utility>

#include "input.hpp"
#include "maybe.hpp"

namespace ctpc {

struct failure_t {};

static inline constexpr failure_t failure{};

template <typename T, std::forward_iterator First, std::sentinel_for<First> Last = First>
class ParseResult {
  private:
    utils::Maybe<T> value_;
    std::ranges::subrange<First, Last> rem_;

    template <typename, std::forward_iterator F, std::sentinel_for<F> L>
    friend class ParseResult;

    template <typename U, typename M>
    static constexpr auto map(U&& value, std::ranges::subrange<First, Last> rem, M&& mapper) {
        if constexpr (std::is_void_v<T>) {
            using ret_t = decltype(mapper());
            if (value.has_value()) {
                if constexpr (std::is_void_v<ret_t>) {
                    mapper();
                    return ParseResult<ret_t, First, Last>{rem};
                } else {
                    return ParseResult<ret_t, First, Last>{rem, mapper()};
                }
            } else {
                return ParseResult<ret_t, First, Last>{rem, failure};
            }
        } else {
            using ret_t = decltype(mapper(*std::forward<U>(value)));
            if (value.has_value()) {
                if constexpr (std::is_void_v<ret_t>) {
                    mapper(*std::forward<U>(value));
                    return ParseResult<ret_t, First, Last>{rem};
                } else {
                    return ParseResult<ret_t, First, Last>{rem, mapper(*std::forward<U>(value))};
                }
            } else {
                return ParseResult<ret_t, First, Last>{rem, failure};
            }
        }
    }

  public:
    using value_type = T;

    template <typename U>
    explicit(!std::is_convertible_v<U, T>)
    constexpr ParseResult(std::ranges::subrange<First, Last> remaining, U&& value)
        : value_(std::forward<U>(value)),
          rem_(remaining) {}

    template <typename... Args>
    explicit(sizeof...(Args) == 0)
    constexpr ParseResult(std::ranges::subrange<First, Last> remaining, Args&&... args)
        : value_(std::in_place, std::forward<Args>(args)...),
          rem_(remaining) {}

    template <typename U, typename... Args>
    constexpr ParseResult(std::ranges::subrange<First, Last> remaining,
                          std::initializer_list<U> ilist,
                          Args&&... args)
        : value_(std::in_place, ilist, std::forward<Args>(args)...),
          rem_(remaining) {}

    constexpr ParseResult(std::ranges::subrange<First, Last> remaining, [[maybe_unused]] failure_t failure)
        : value_(utils::none),
          rem_(remaining) {}

    template <typename U, typename F, typename L>
    explicit(!std::is_convertible_v<U, T> || !std::is_convertible_v<F, First> || !std::is_convertible_v<L, Last>)
    constexpr ParseResult(const ParseResult<U, F, L>& other)
        : value_(other.value_),
          rem_(std::ranges::begin(other.rem_), std::ranges::end(other.rem_)) {}

    template <typename U, typename F, typename L>
    explicit(!std::is_convertible_v<U, T> || !std::is_convertible_v<F, First> || !std::is_convertible_v<L, Last>)
    constexpr ParseResult(ParseResult<U, F, L>&& other)
        : value_(std::move(other.value_)),
          rem_(std::ranges::begin(other.rem_), std::ranges::end(other.rem_)) {}

    constexpr bool passed() const noexcept {
        return value_.has_value();
    }

    constexpr bool failed() const noexcept {
        return !passed();
    }

    constexpr operator bool() const noexcept {
        return passed();
    }

    constexpr auto remaining() const {
        return rem_;
    }

    constexpr decltype(auto) value() & {
        return value_.value();
    }

    constexpr decltype(auto) value() const& {
        return value_.value();
    }

    constexpr decltype(auto) value() && {
        return std::move(value_).value();
    }

    constexpr decltype(auto) operator*() & {
        return *value_;
    }

    constexpr decltype(auto) operator*() const& {
        return *value_;
    }

    constexpr decltype(auto) operator*() && {
        return *std::move(value_);
    }

    constexpr decltype(auto) operator->() {
        return value_.operator->();
    }

    constexpr decltype(auto) operator->() const {
        return value_.operator->();
    }

    constexpr decltype(auto) begin() {
        return std::ranges::begin(rem_);
    }

    constexpr decltype(auto) begin() const {
        return std::ranges::begin(rem_);
    }

    constexpr decltype(auto) end() {
        return std::ranges::end(rem_);
    }

    constexpr decltype(auto) end() const {
        return std::ranges::end(rem_);
    }

    template <typename M>
    constexpr auto map(M&& mapper) & {
        return map(value_, rem_, std::forward<M>(mapper));
    }

    template <typename M>
    constexpr auto map(M&& mapper) const& {
        return map(value_, rem_, std::forward<M>(mapper));
    }

    template <typename M>
    constexpr auto map(M&& mapper) && {
        return map(std::move(value_), rem_, std::forward<M>(mapper));
    }
};

template <typename T, typename I>
using ParseResultOf = ParseResult<T, std::ranges::iterator_t<I>, std::ranges::sentinel_t<I>>;

template <typename T, Input I, typename... Args>
constexpr auto pass(I&& remaining, Args&&... args) {
    return ParseResultOf<T, I>(std::forward<I>(remaining), std::forward<Args>(args)...);
}

template <typename T, Input I, typename U, typename... Args>
constexpr auto pass(I&& remaining, std::initializer_list<U> ilist, Args&&... args) {
    return ParseResultOf<T, I>(std::forward<I>(remaining), ilist, std::forward<Args>(args)...);
}

template <typename T, Input I>
constexpr auto fail(I&& remaining) {
    return ParseResultOf<T, I>(std::forward<I>(remaining), failure);
}

}

#endif
