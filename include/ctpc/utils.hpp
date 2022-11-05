#ifndef CTPC_UTILS_HPP
#define CTPC_UTILS_HPP

#include <functional>
#include <type_traits>
#include <tuple>
#include <utility>

#define CTPC_F(name) \
    [](auto&&... args) -> decltype(auto) { \
        return name(std::forward<decltype(args)>(args)...); \
    }

namespace ctpc::utils {

// type trait to check if a type is a std::tuple
template <typename T>
struct is_tuple : std::false_type {};

template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type {};

template <typename T>
static inline constexpr bool is_tuple_v = is_tuple<T>::value;

// Converts a value into a 1-tuple. If the value is already
// a tuple of any arity, the tuple is returned as is.
template <typename T>
constexpr decltype(auto) into_tuple(T&& value) {
    if constexpr (is_tuple_v<std::remove_cvref_t<T>>) {
        return std::forward<T>(value);
    } else {
        return std::forward_as_tuple<T>(std::forward<T>(value));
    }
}

namespace detail {

template <typename F, typename T, size_t... IDX>
constexpr decltype(auto) invoke_with_tuple_impl(F&& func, T&& tuple, [[maybe_unused]] std::index_sequence<IDX...> idx) {
    return std::invoke(std::forward<F>(func), std::forward<typename std::tuple_element<IDX, std::remove_cvref_t<T>>::type>(std::get<IDX>(tuple))...);
}

}

// Invokes a callable with the elements of a tuple as arguments.
template <typename F, typename T>
constexpr decltype(auto) invoke_with_tuple(F&& func, T&& tuple) {
    return detail::invoke_with_tuple_impl(std::forward<F>(func), std::forward<T>(tuple), std::make_index_sequence<std::tuple_size<std::remove_cvref_t<T>>::value>{});
}

// Invokes a callable with tuple arguments unpacked into
// multiple separate arguments. Unpacking is not done
// recursively, so nested tuples will passed as a tuple.
template <typename F, typename... Args>
constexpr decltype(auto) invoke_unpacked(F&& func, Args&&... args) {
    return invoke_with_tuple(std::forward<F>(func), std::tuple_cat(into_tuple(std::forward<Args>(args))...));
}

}

#endif
