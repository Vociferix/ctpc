#ifndef CTPC_UTF_HPP
#define CTPC_UTF_HPP

#include <cstdint>
#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>

#include "input.hpp"
#include "parse_result.hpp"

namespace ctpc {

namespace utils {

template <typename T>
struct is_text_char : std::false_type {};

template <>
struct is_text_char<char> : std::true_type {};

template <>
struct is_text_char<char8_t> : std::true_type {};

template <>
struct is_text_char<char16_t> : std::true_type {};

template <>
struct is_text_char<char32_t> : std::true_type {};

template <>
struct is_text_char<wchar_t> : std::true_type {};

template <typename T>
static inline constexpr bool is_text_char_v = is_text_char<T>::value;

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = char32_t>
struct Utf8ToUtf32 {
  private:
    std::ranges::subrange<I, S> range_;
    std::optional<O> curr_{};

  public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    constexpr Utf8ToUtf32() = default;

    constexpr Utf8ToUtf32(I first, S last)
        : range_(first, last) {
        operator++();
    }

    explicit constexpr Utf8ToUtf32(std::ranges::subrange<I, S> range)
        : range_(range) {
        operator++();
    }

    constexpr O operator*() const {
        return *curr_;
    }

    constexpr Utf8ToUtf32& operator++() {
        if (range_.empty()) {
            curr_.reset();
            return *this;
        }

        auto c = static_cast<uint32_t>(range_.front());
        range_ = range_.next();

        size_t count = 0;
        if ((c & 0b1000'0000) == 0b0000'0000) {
            count = 0;
        } else if ((c & 0b1110'0000) == 0b1100'0000) {
            count = 1;
            c &= 0b0001'1111;
        } else if ((c & 0b1111'0000) == 0b1110'0000) {
            count = 2;
            c &= 0b0000'1111;
        } else if ((c & 0b1111'1000) == 0b1111'0000) {
            count = 3;
            c &= 0b0000'0111;
        } else if ((c & 0b1111'1100) == 0b1111'1000) {
            count = 4;
            c &= 0b0000'0011;
        } else if ((c & 0b1111'1110) == 0b1111'1100) {
            count = 5;
            c &= 0b0000'0001;
        } else {
            c = 0xFFFD;
        }

        while (count != 0) {
            if (range_.empty()) {
                c = 0xFFFD;
                break;
            }
            auto tmp = static_cast<uint32_t>(range_.front());
            if ((tmp & 0b1100'0000) != 0b1000'0000) {
                c = 0xFFFD;
                break;
            }
            c = (c << 6) | tmp;
            range_ = range_.next();
            --count;
        }

        curr_.emplace(c);
        return *this;
    }

    constexpr Utf8ToUtf32 operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }

    constexpr std::optional<O> get() const {
        return curr_;
    }

    friend constexpr bool operator==(const Utf8ToUtf32& lhs, const Utf8ToUtf32& rhs) {
        if (lhs.range_.empty()) {
            return rhs.range_.empty();
        } else {
            return !rhs.range_.empty() && lhs.range_.begin() == rhs.range_.begin();
        }
    }

    friend constexpr bool operator==(const Utf8ToUtf32& lhs, [[maybe_unused]] std::default_sentinel_t rhs) {
        return !lhs.curr_.has_value();
    }

    friend constexpr bool operator==(std::default_sentinel_t lhs, const Utf8ToUtf32& rhs) {
        return operator==(rhs, lhs);
    }
};

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = char8_t>
struct Utf32ToUtf8 {
  private:
    std::ranges::subrange<I, S> range_;
    std::array<char8_t, 5> cache_{};
    size_t pos_{5};
    std::optional<char8_t> curr_{std::nullopt};

  public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    constexpr Utf32ToUtf8() = default;

    constexpr Utf32ToUtf8(I first, S last)
        : range_(first, last) {
        operator++();
    }

    explicit constexpr Utf32ToUtf8(std::ranges::subrange<I, S> range)
        : range_(range) {
        operator++();
    }

    constexpr O operator*() const {
        return *curr_;
    }

    constexpr Utf32ToUtf8& operator++() {
        if (pos_ < 5) {
            curr_.emplace(cache_[pos_++]);
            return *this;
        } else if (range_.empty()) {
            curr_.reset();
            return *this;
        }

        auto c = static_cast<uint32_t>(range_.front());
        range_ = range_.next();

        if (c < 0x80) {
            curr_.emplace(static_cast<O>(c));
        } else if (c < 0x800) {
            curr_.emplace(static_cast<O>((c >> 6) | 0b1100'0000));
            cache_[4] = static_cast<O>((c & 0b0011'1111) | 0b1000'0000);
            pos_ = 4;
        } else if (c < 0x10000) {
            curr_.emplace(static_cast<O>((c >> 12) | 0b1110'0000));
            cache_[3] = static_cast<O>(((c >> 6) & 0b0011'1111) | 0b1000'0000);
            cache_[4] = static_cast<O>((c & 0b0011'1111) | 0b1000'0000);
            pos_ = 3;
        } else if (c < 0x200000) {
            curr_.emplace(static_cast<O>((c >> 18) | 0b1111'0000));
            cache_[2] = static_cast<O>(((c >> 12) & 0b0011'1111) | 0b1000'0000);
            cache_[3] = static_cast<O>(((c >> 6) & 0b0011'1111) | 0b1000'0000);
            cache_[4] = static_cast<O>((c & 0b0011'1111) | 0b1000'0000);
            pos_ = 2;
        } else if (c < 0x4000000) {
            curr_.emplace(static_cast<O>((c >> 24) | 0b1111'1000));
            cache_[1] = static_cast<O>(((c >> 18) & 0b0011'1111) | 0b1000'0000);
            cache_[2] = static_cast<O>(((c >> 12) & 0b0011'1111) | 0b1000'0000);
            cache_[3] = static_cast<O>(((c >> 6) & 0b0011'1111) | 0b1000'0000);
            cache_[4] = static_cast<O>((c & 0b0011'1111) | 0b1000'0000);
            pos_ = 1;
        } else {
            curr_.emplace(static_cast<O>((c >> 30) | 0b1111'1100));
            cache_[0] = static_cast<O>(((c >> 24) & 0b0011'1111) | 0b1000'0000);
            cache_[1] = static_cast<O>(((c >> 18) & 0b0011'1111) | 0b1000'0000);
            cache_[2] = static_cast<O>(((c >> 12) & 0b0011'1111) | 0b1000'0000);
            cache_[3] = static_cast<O>(((c >> 6) & 0b0011'1111) | 0b1000'0000);
            cache_[4] = static_cast<O>((c & 0b0011'1111) | 0b1000'0000);
            pos_ = 0;
        }

        return *this;
    }

    constexpr Utf32ToUtf8 operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }

    friend constexpr bool operator==(const Utf32ToUtf8& lhs, const Utf32ToUtf8& rhs) {
        if (lhs.pos_ == rhs.pos_) {
            if (lhs.range_.empty()) {
                return rhs.range_.empty();
            } else {
                return !rhs.range_.empty() && lhs.range_.begin() == rhs.range_.begin();
            }
        }
        return false;
    }

    friend constexpr bool operator==(const Utf32ToUtf8& lhs, [[maybe_unused]] std::default_sentinel_t rhs) {
        return !lhs.curr_.has_value();
    }

    friend constexpr bool operator==(std::default_sentinel_t lhs, const Utf32ToUtf8& rhs) {
        return operator==(rhs, lhs);
    }
};

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = char32_t>
struct Utf16ToUtf32 {
  private:
    std::ranges::subrange<I, S> range_;
    std::optional<O> curr_{std::nullopt};

  public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    constexpr Utf16ToUtf32() = default;

    constexpr Utf16ToUtf32(I first, S last)
        : range_(first, last) {
        operator++();
    }

    explicit constexpr Utf16ToUtf32(std::ranges::subrange<I, S> range)
        : range_(range) {
        operator++();
    }

    constexpr O operator*() const {
        return *curr_;
    }

    constexpr Utf16ToUtf32& operator++() {
        if (range_.empty()) {
            curr_.reset();
            return *this;
        }

        auto c = static_cast<uint32_t>(range_.front());
        range_ = range_.next();

        if ((c & 0b1111'1100'0000'0000) == 0b1101'1000'0000'0000) {
            if (range_.empty()) {
                curr_.emplace(static_cast<O>(0xFFFD));
                return *this;
            }

            auto d = static_cast<uint32_t>(range_.front());
            range_ = range_.next();
            if ((d & 0b1111'1100'0000'0000) != 0b1101'1100'0000'0000) {
                curr_.emplace(static_cast<O>(0xFFFD));
                return *this;
            }

            c = (c & 0b0000'0011'1111'1111) << 10;
            d = d & 0b0000'0011'1111'1111;
            curr_.emplace(static_cast<O>((c | d) + 0x10000));
        } else {
            curr_.emplace(static_cast<O>(c));
        }

        return *this;
    }

    constexpr Utf16ToUtf32 operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }

    friend constexpr bool operator==(const Utf16ToUtf32& lhs, const Utf16ToUtf32& rhs) {
        if (lhs.range_.empty()) {
            return rhs.range_.empty();
        } else {
            return !rhs.range_.empty() && lhs.range_.begin() == rhs.range_.begin();
        }
    }

    friend constexpr bool operator==(const Utf16ToUtf32& lhs, [[maybe_unused]] std::default_sentinel_t rhs) {
        return !lhs.curr_.has_value();
    }

    friend constexpr bool operator==(std::default_sentinel_t lhs, const Utf16ToUtf32& rhs) {
        return operator==(rhs, lhs);
    }
};

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = char16_t>
struct Utf32ToUtf16 {
  private:
    std::ranges::subrange<I, S> range_;
    std::optional<O> next_{std::nullopt};
    std::optional<O> curr_{std::nullopt};

  public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    constexpr Utf32ToUtf16() = default;

    constexpr Utf32ToUtf16(I first, S last)
        : range_(first, last) {
        operator++();
    }

    explicit constexpr Utf32ToUtf16(std::ranges::subrange<I, S> range)
        : range_(range) {
        operator++();
    }

    constexpr O operator*() const {
        return *curr_;
    }

    constexpr Utf32ToUtf16& operator++() {
        if (next_.has_value()) {
            curr_.emplace(*next_);
            next_.reset();
            return *this;
        } else if (range_.empty()) {
            curr_.reset();
            return *this;
        }

        auto c = static_cast<uint32_t>(range_.front());
        range_ = range_.next();

        if (c < 0x10000) {
            if (c >= 0xD800 && c < 0xE000) {
                c = 0xFFFD;
            }
            curr_.emplace(static_cast<O>(c));
        } else {
            c -= 0x10000;
            next_.emplace(static_cast<O>((c & 0b0000'0011'1111'1111) | 0xDC00));
            curr_.emplace(static_cast<O>((c >> 10) | 0xD800));
        }

        return *this;
    }

    constexpr Utf32ToUtf16 operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }

    friend constexpr bool operator==(const Utf32ToUtf16& lhs, const Utf32ToUtf16& rhs) {
        if (lhs.next_.has_value() == rhs.next_.has_value()) {
            if (lhs.range_.empty()) {
                return rhs.range_.empty();
            } else {
                return !rhs.range_.empty() && lhs.range_.begin() == rhs.range_.begin();
            }
        }
        return false;
    }

    friend constexpr bool operator==(const Utf32ToUtf16& lhs, [[maybe_unused]] std::default_sentinel_t rhs) {
        return !lhs.curr_.has_value();
    }

    friend constexpr bool operator==(std::default_sentinel_t lhs, const Utf32ToUtf16& rhs) {
        return operator==(rhs, lhs);
    }
};

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = char16_t>
struct Utf8ToUtf16 : Utf32ToUtf16<Utf8ToUtf32<I, S>, std::default_sentinel_t, O> {
    constexpr Utf8ToUtf16(I first, S last)
        : Utf32ToUtf16<Utf8ToUtf32<I, S>, std::default_sentinel_t, O>(
                Utf8ToUtf32<I, S>(first, last),
                std::default_sentinel) {}

    constexpr Utf8ToUtf16(std::ranges::subrange<I, S> range)
        : Utf32ToUtf16<Utf8ToUtf32<I, S>, std::default_sentinel_t, O>(
                Utf8ToUtf32<I, S>(range),
                std::default_sentinel) {}

    constexpr Utf8ToUtf16& operator++() {
        Utf32ToUtf16<Utf8ToUtf32<I, S>, std::default_sentinel_t, O>::operator++();
        return *this;
    }

    constexpr Utf8ToUtf16 operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }
};

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = char8_t>
struct Utf16ToUtf8 : Utf32ToUtf8<Utf16ToUtf32<I, S>, std::default_sentinel_t, O> {
    constexpr Utf16ToUtf8(I first, S last)
        : Utf32ToUtf8<Utf16ToUtf32<I, S>, std::default_sentinel_t, O>(
                Utf16ToUtf32<I, S>(first, last),
                std::default_sentinel) {}

    constexpr Utf16ToUtf8(std::ranges::subrange<I, S> range)
        : Utf32ToUtf8<Utf16ToUtf32<I, S>, std::default_sentinel_t, O>(
                Utf16ToUtf32<I, S>(range),
                std::default_sentinel) {}

    constexpr Utf16ToUtf8& operator++() {
        Utf32ToUtf8<Utf16ToUtf32<I, S>, std::default_sentinel_t, O>::operator++();
        return *this;
    }

    constexpr Utf16ToUtf8 operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }
};

template <std::forward_iterator I, std::sentinel_for<I> S = I, typename O = std::remove_cvref_t<std::iter_value_t<I>>>
struct UtfIdentity {
  private:
    std::ranges::subrange<I, S> range_;

  public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    constexpr UtfIdentity() = default;

    constexpr UtfIdentity(I first, S last)
        : range_(first, last) {}

    explicit constexpr UtfIdentity(std::ranges::subrange<I, S> range)
        : range_(range) {}

    constexpr O operator*() const {
        return static_cast<O>(range_.front());
    }

    constexpr UtfIdentity& operator++() {
        range_ = range_.next();
        return *this;
    }

    constexpr UtfIdentity operator++(int) {
        auto ret = *this;
        operator++();
        return ret;
    }

    friend constexpr bool operator==(const UtfIdentity& lhs, const UtfIdentity& rhs) {
        if (lhs.range_.empty()) {
            return rhs.range_.empty();
        } else {
            return !rhs.range_.empty() && lhs.range_.begin() == rhs.range_.begin();
        }
    }

    friend constexpr bool operator==(const UtfIdentity& lhs, [[maybe_unused]] std::default_sentinel_t rhs) {
        return lhs.range_.empty();
    }

    friend constexpr bool operator==(std::default_sentinel_t lhs, const UtfIdentity& rhs) {
        return operator==(rhs, lhs);
    }
};

namespace detail {

template <template<typename, typename, typename> class U, typename To, typename Range>
constexpr auto utf_convert_impl(Range&& range) {
    using I = std::ranges::iterator_t<Range>;
    using S = std::ranges::sentinel_t<Range>;
    return std::ranges::subrange<U<I, S, To>, std::default_sentinel_t>(
        U<I, S, To>{std::forward<Range>(range)},
        std::default_sentinel
    );
}

}

/// @brief Lazily converts one UTF encoding into another UTF encoding
///
/// @details
/// This function takes a UTF encoded range and returns a new view of
/// the same range lazily converted to a UTF encoding matching the type
/// `To`. The output encoding and the assumed input encoding depend on
/// the byte width of the input range character type and the `To` type,
/// respectively. For each, given character type `T`:
///   * `sizeof(T) == 1`: UTF-8 (e.g. `char8_t`)
///   * `sizeof(T) == 2`: UTF-16 (e.g. `char16_t`)
///   * `sizeof(T) == 4`: UTF-32 (e.g. `char32_t`)
///
/// The output view lazily converts characters from the input range in
/// order when they are fetched, such as during iteration.
template <typename To, typename Range>
constexpr auto utf_convert(Range&& range) {
    using from_t = std::remove_cvref_t<std::ranges::range_value_t<Range>>;
    using detail::utf_convert_impl;

    if constexpr (sizeof(from_t) == sizeof(char8_t)) {
        if constexpr (sizeof(To) == sizeof(char8_t)) {
            return utf_convert_impl<UtfIdentity, To>(std::forward<Range>(range));
        } else if constexpr (sizeof(To) == sizeof(char16_t)) {
            return utf_convert_impl<Utf8ToUtf16, To>(std::forward<Range>(range));
        } else if constexpr (sizeof(To) == sizeof(char32_t)) {
            return utf_convert_impl<Utf8ToUtf32, To>(std::forward<Range>(range));
        }
    } else if constexpr (sizeof(from_t) == sizeof(char16_t)) {
        if constexpr (sizeof(To) == sizeof(char8_t)) {
            return utf_convert_impl<Utf16ToUtf8, To>(std::forward<Range>(range));
        } else if constexpr (sizeof(To) == sizeof(char16_t)) {
            return utf_convert_impl<UtfIdentity, To>(std::forward<Range>(range));
        } else if constexpr (sizeof(To) == sizeof(char32_t)) {
            return utf_convert_impl<Utf16ToUtf32, To>(std::forward<Range>(range));
        }
    } else if constexpr (sizeof(from_t) == sizeof(char32_t)) {
        if constexpr (sizeof(To) == sizeof(char8_t)) {
            return utf_convert_impl<Utf32ToUtf8, To>(std::forward<Range>(range));
        } else if constexpr (sizeof(To) == sizeof(char16_t)) {
            return utf_convert_impl<Utf32ToUtf16, To>(std::forward<Range>(range));
        } else if constexpr (sizeof(To) == sizeof(char32_t)) {
            return utf_convert_impl<UtfIdentity, To>(std::forward<Range>(range));
        }
    }
}

}

template <typename Char>
struct Utf {
    template <Input I>
    constexpr auto operator()(I input) {
        return pass<void>(utils::utf_convert<Char>(input));
    }
};

/// @ingroup ctpc_parsers
template <typename Char>
static constexpr Utf<Char> utf{};

/// @ingroup ctpc_parsers
static constexpr const auto& utf8 = utf<char8_t>;

/// @ingroup ctpc_parsers
static constexpr const auto& utf16 = utf<char16_t>;

/// @ingroup ctpc_parsers
static constexpr const auto& utf32 = utf<char32_t>;

/// @ingroup ctpc_parsers
static constexpr const auto& wutf = utf<wchar_t>;

/// @ingroup ctpc_parsers
static constexpr const auto& sutf = utf<char>;

/// @ingroup ctpc_parsers
static constexpr const auto& uutf = utf<unsigned char>;

/// @ingroup ctpc_parsers
static constexpr const auto& butf = utf<std::byte>;

}

#endif
