#ifndef CTPC_INTEGER_HPP
#define CTPC_INTEGER_HPP

#include <algorithm>
#include <bit>
#include <cstdint>
#include <type_traits>

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "byte.hpp"
#include "static_count.hpp"

namespace ctpc {

template <typename T>
concept Integral = std::is_integral_v<T> && !std::is_same_v<T, bool>;

template <Integral T, std::endian ENDIANNESS = std::endian::native>
struct Integer {
    template <ByteInput I>
    constexpr auto operator()(I input) const -> ParseResultOf<T, I> {
        if constexpr (sizeof(T) == 1) {
            return byte(input).map([] (auto b) -> T { return static_cast<T>(b); });
        } else {
            return static_count<sizeof(T)>(byte).map([] (const auto& bytes) {
                if constexpr (ENDIANNESS == std::endian::native) {
                    return std::bit_cast<T>(bytes);
                } else {
                    std::array<std::byte, sizeof(T)> tmp{};
                    std::ranges::reverse_copy(bytes, tmp);
                    return std::bit_cast<T>(bytes);
                }
            });
        }
    }
};

/// @ingroup ctpc_parsers
template <Integral T, std::endian ENDIANNESS = std::endian::native>
static constexpr Integer<T, ENDIANNESS> integer{};

/// @ingroup ctpc_parsers
static constexpr auto& uint8 = integer<uint_least8_t>;

/// @ingroup ctpc_parsers
template <std::endian ENDIANNESS = std::endian::native>
static constexpr auto& uint16 = integer<uint_least16_t, ENDIANNESS>;

/// @ingroup ctpc_parsers
static constexpr auto& uint16_be = uint16<std::endian::big>;

/// @ingroup ctpc_parsers
static constexpr auto& uint16_le = uint16<std::endian::little>;

/// @ingroup ctpc_parsers
template <std::endian ENDIANNESS = std::endian::native>
static constexpr auto& uint32 = integer<uint_least32_t, ENDIANNESS>;

/// @ingroup ctpc_parsers
static constexpr auto& uint32_be = uint32<std::endian::big>;

/// @ingroup ctpc_parsers
static constexpr auto& uint32_le = uint32<std::endian::little>;

/// @ingroup ctpc_parsers
template <std::endian ENDIANNESS = std::endian::native>
static constexpr auto& uint64 = integer<uint_least64_t, ENDIANNESS>;

/// @ingroup ctpc_parsers
static constexpr auto& uint64_be = uint64<std::endian::big>;

/// @ingroup ctpc_parsers
static constexpr auto& uint64_le = uint64<std::endian::little>;

/// @ingroup ctpc_parsers
static constexpr auto& int8 = integer<int_least8_t>;

/// @ingroup ctpc_parsers
template <std::endian ENDIANNESS = std::endian::native>
static constexpr auto& int16 = integer<int_least16_t, ENDIANNESS>;

/// @ingroup ctpc_parsers
static constexpr auto& int16_be = int16<std::endian::big>;

/// @ingroup ctpc_parsers
static constexpr auto& int16_le = int16<std::endian::little>;

/// @ingroup ctpc_parsers
template <std::endian ENDIANNESS = std::endian::native>
static constexpr auto& int32 = integer<int_least32_t, ENDIANNESS>;

/// @ingroup ctpc_parsers
static constexpr auto& int32_be = int32<std::endian::big>;

/// @ingroup ctpc_parsers
static constexpr auto& int32_le = int32<std::endian::little>;

/// @ingroup ctpc_parsers
template <std::endian ENDIANNESS = std::endian::native>
static constexpr auto& int64 = integer<int_least64_t, ENDIANNESS>;

/// @ingroup ctpc_parsers
static constexpr auto& int64_be = int64<std::endian::big>;

/// @ingroup ctpc_parsers
static constexpr auto& int64_le = int64<std::endian::little>;

}

#endif
