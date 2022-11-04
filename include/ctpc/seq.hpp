#ifndef CTPC_SEQ_HPP
#define CTPC_SEQ_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename... P>
struct SeqParser;

template <>
struct SeqParser<> {
  private:
    template <Input I, typename... Ret>
    constexpr auto call(I&& input, Ret&&... ret) const {
        if constexpr (sizeof...(Ret) == 0) {
            return pass<void>(std::forward<I>(input));
        } else if constexpr (sizeof...(Ret) == 1) {
            return pass<Ret...>(std::forward<I>(input), std::forward<Ret>(ret)...);
        } else {
            return pass<std::tuple<Ret...>>(std::forward<I>(input), std::forward<Ret>(ret)...);
        }
    }

    template <typename...>
    friend struct SeqParser;

  public:
    constexpr SeqParser() = default;

    template <Input I>
    constexpr auto operator()(I&& input) const {
        return call(std::forward<I>(input));
    }
};

template <typename P1, typename... PN>
struct SeqParser<P1, PN...> {
  private:
    [[no_unique_address]] P1 parser_;
    [[no_unique_address]] SeqParser<PN...> inner_;

    template <typename...>
    friend struct SeqParser;

    template <typename T, ParseableBy<P1> I, typename... Ret>
    static constexpr auto call_impl(T&& self, I&& input, Ret&&... ret) {
        auto res = self.parser_(input);
        if constexpr (std::is_void_v<typename std::remove_cvref_t<decltype(res)>::value_type>) {
            if (res) {
                return std::forward<T>(self).inner_.call(res.remaining(), std::forward<Ret>(ret)...);
            } else {
                using res_t = std::remove_cvref_t<decltype(std::forward<T>(self).inner_.call(res.remaining(), std::forward<Ret>(ret)...))>;
                return fail<typename res_t::value_type>(std::forward<I>(input));
            }
        } else {
            if (res) {
                auto rem = res.remaining();
                return std::forward<T>(self).inner_.call(rem, std::forward<Ret>(ret)..., *std::move(res));
            } else {
                using res_t = std::remove_cvref_t<decltype(std::forward<T>(self).inner_.call(res.remaining(), std::forward<Ret>(ret)..., *std::move(res)))>;
                return fail<typename res_t::value_type>(std::forward<I>(input));
            }
        }
    }

    template <typename T, ParseableBy<P1> I>
    static constexpr auto call_first(T&& self, I&& input) {
        auto res = call_impl(std::forward<T>(self), input);
        if (res) {
            return res;
        } else {
            return fail<typename std::remove_cvref_t<decltype(res)>::value_type>(std::forward<I>(input));
        }
    }

    template <ParseableBy<P1> I, typename... Ret>
    constexpr auto call(I&& input, Ret&&... ret) & {
        return call_impl(*this, std::forward<I>(input), std::forward<Ret>(ret)...);
    }

    template <ParseableBy<P1> I, typename... Ret>
    constexpr auto call(I&& input, Ret&&... ret) const& {
        return call_impl(*this, std::forward<I>(input), std::forward<Ret>(ret)...);
    }

    template <ParseableBy<P1> I, typename... Ret>
    constexpr auto call(I&& input, Ret&&... ret) && {
        return call_impl(std::move(*this), std::forward<I>(input), std::forward<Ret>(ret)...);
    }

  public:
    explicit constexpr SeqParser(P1&& parser, PN&&... inner)
        : parser_(std::forward<P1>(parser)),
          inner_(std::forward<PN>(inner)...) {}

    template <ParseableBy<P1> I>
    constexpr auto operator()(I&& input) & {
        return call_first(*this, std::forward<I>(input));
    }

    template <ParseableBy<P1> I>
    constexpr auto operator()(I&& input) const& {
        return call_first(*this, std::forward<I>(input));
    }

    template <ParseableBy<P1> I>
    constexpr auto operator()(I&& input) && {
        return call_first(std::move(*this), std::forward<I>(input));
    }
};

}

struct Seq {
    template <typename... P>
    constexpr auto operator()(P&&... elems) const -> detail::SeqParser<P...> {
        return detail::SeqParser<P...>(std::forward<P>(elems)...);
    }
};

static constexpr Seq seq{};

}

#endif