#ifndef CTPC_INPUT_HPP
#define CTPC_INPUT_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <type_traits>

namespace ctpc {

template <typename T>
concept Input = std::ranges::forward_range<T> &&
                std::ranges::borrowed_range<T> &&
                !std::is_array_v<std::remove_cvref_t<T>> &&
                !std::is_pointer_v<std::remove_cvref_t<T>>;

template <typename T, typename U>
concept InputOf = Input<T> && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<T>>, U>;

template <typename T>
concept TextInput =
    InputOf<T, char> ||
    InputOf<T, char8_t> ||
    InputOf<T, wchar_t> ||
    InputOf<T, char16_t> ||
    InputOf<T, char32_t>;

template <typename T>
concept ByteInput = InputOf<T, unsigned char> || InputOf<T, uint8_t> || InputOf<T, std::byte>;

}

#endif
