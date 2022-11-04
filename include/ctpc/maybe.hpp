#ifndef CTPC_MAYBE_HPP
#define CTPC_MAYBE_HPP

#include <initializer_list>
#include <optional>
#include <type_traits>
#include <utility>

namespace ctpc::utils {

struct none_t {};

static inline constexpr none_t none{};

template <typename T>
class Maybe {
  private:
    std::optional<T> value_{std::nullopt};

  public:
    using value_type = T;

    constexpr Maybe() = default;

    constexpr Maybe([[maybe_unused]] none_t none) {}

    template <typename U>
    explicit(!std::is_convertible_v<U, T>) constexpr Maybe(U&& value)
        requires(!std::is_same_v<std::remove_cvref_t<U>, Maybe>)
        : value_(std::forward<U>(value)) {}

    template <typename... Args>
    explicit(sizeof...(Args) == 0) constexpr Maybe(std::in_place_t in_place, Args&&... args)
        : value_(in_place, std::forward<Args>(args)...) {}

    template <typename U, typename... Args>
    constexpr Maybe(std::in_place_t in_place, std::initializer_list<U> ilist, Args&&... args)
        : value_(in_place, ilist, std::forward<Args>(args)...) {}

    template <typename U>
    explicit(!std::is_convertible_v<U, T>) constexpr Maybe(const Maybe<U>& other) {
        if (other.has_value()) {
            value_.emplace(*other);
        }
    }

    template <typename U>
    explicit(!std::is_convertible_v<U, T>) constexpr Maybe(Maybe<U>&& other) {
        if (other.has_value()) {
            value_.emplace(*std::move(other));
        }
    }

    constexpr bool has_value() const noexcept {
        return value_.has_value();
    }

    constexpr operator bool() const noexcept {
        return has_value();
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

    constexpr decltype(auto) operator*() & noexcept {
        return *value_;
    }

    constexpr decltype(auto) operator*() const& noexcept {
        return *value_;
    }

    constexpr decltype(auto) operator*() && {
        return *std::move(value_);
    }

    constexpr decltype(auto) operator->() noexcept {
        return value_.operator->();
    }

    constexpr decltype(auto) operator->() const noexcept {
        return value_.operator->();
    }
};

template <typename T>
class Maybe<T&> {
  private:
    T* value_{nullptr};

  public:
    using value_type = T&;

    constexpr Maybe() = default;

    constexpr Maybe([[maybe_unused]] none_t none) {}

    template <typename U>
    constexpr Maybe(U& value)
        requires(!std::is_same_v<std::remove_cvref_t<U>, Maybe>)
        : value_(&value) {}

    template <typename U>
    constexpr Maybe([[maybe_unused]] std::in_place_t in_place, U& value)
        : value_(&value) {}

    template <typename U>
    constexpr Maybe(const Maybe<U>& other) {
        if (other.has_value()) {
            value_ = other.operator->();
        }
    }

    template <typename U>
    constexpr Maybe(Maybe<U&>&& other) {
        if (other.has_value()) {
            value_ = other.operator->();
        }
    }

    constexpr bool has_value() const noexcept {
        return value_ != nullptr;
    }

    constexpr operator bool() const noexcept {
        return has_value();
    }

    constexpr decltype(auto) value() {
        if (value_ == nullptr) {
            throw std::bad_optional_access();
        }
        return *value_;
    }

    constexpr decltype(auto) value() const {
        if (value_ == nullptr) {
            throw std::bad_optional_access();
        }
        return *value_;
    }

    constexpr decltype(auto) operator*() noexcept {
        return *value_;
    }

    constexpr decltype(auto) operator*() const noexcept {
        return *value_;
    }

    constexpr decltype(auto) operator->() noexcept {
        return value_;
    }

    constexpr decltype(auto) operator->() const noexcept {
        return value_;
    }
};

template <typename T>
class Maybe<T&&> : public Maybe<T> {
  public:
    using value_type = T&&;

    using Maybe<T>::Maybe;
};

template <>
class Maybe<void> {
  private:
    bool has_value_{false};

  public:
    using value_type = void;

    constexpr Maybe() = default;

    constexpr Maybe([[maybe_unused]] none_t none) {}

    constexpr Maybe([[maybe_unused]] std::in_place_t in_place)
        : has_value_(true) {}

    constexpr bool has_value() const noexcept {
        return has_value_;
    }

    constexpr operator bool() const noexcept {
        return has_value();
    }

    constexpr void value() const {
        if (!has_value_) {
            throw std::bad_optional_access();
        }
    }

    constexpr void operator*() const noexcept {}
};

}

#endif
