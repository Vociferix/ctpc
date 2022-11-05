#include <ctpc/ctpc.hpp>
#include <cstdint>
#include <iostream>
#include <string_view>

using namespace ctpc;

using namespace std::string_view_literals;

static constexpr auto ws = regex_match<"\\s*">;

constexpr auto ignore_ws(auto&& parser) {
    return delimited(ws, std::forward<decltype(parser)>(parser), ws);
}

static constexpr auto plus = ignore_ws(verbatim<"+">);
static constexpr auto minus = ignore_ws(verbatim<"-">);
static constexpr auto star = ignore_ws(verbatim<"*">);
static constexpr auto slash = ignore_ws(verbatim<"/">);
static constexpr auto lparen = ignore_ws(verbatim<"(">);
static constexpr auto rparen = ignore_ws(verbatim<")">);

static constexpr auto number = map(ignore_ws(regex_match<"\\d+">), [](Input auto&& input) {
    int64_t value = 0;
    for (auto c : input) {
        value = (value * 10) + (static_cast<int64_t>(c) - '0');
    }
    return value;
});

static constexpr auto num_expr = alt(
    map(preceded(minus, number), [](auto value) { return -value; }),
    number
);

template <Input I>
constexpr ParseResultOf<int64_t, I> expr_(I input);
static constexpr auto expr = CTPC_F(expr_);

template <Input I>
constexpr ParseResultOf<int64_t, I> term_(I input);
static constexpr auto term = CTPC_F(term_);

template <Input I>
constexpr ParseResultOf<int64_t, I> unary_(I input);
static constexpr auto unary = CTPC_F(unary_);

template <Input I>
constexpr ParseResultOf<int64_t, I> primary_(I input);
static constexpr auto primary = CTPC_F(primary_);

template <Input I>
constexpr ParseResultOf<int64_t, I> primary_(I input) {
    return alt(
        number,
        delimited(lparen, expr, rparen)
    )(input);
}

template <Input I>
constexpr ParseResultOf<int64_t, I> unary_(I input) {
    return alt(
        map(preceded(minus, unary), [](auto value) { return -value; }),
        primary
    )(input);
}

template <Input I>
constexpr ParseResultOf<int64_t, I> term_(I input) {
    return alt(
        map(seq(unary, ignore(star), term), [](auto lhs, auto rhs) { return lhs * rhs; }),
        map(seq(unary, ignore(slash), term), [](auto lhs, auto rhs) { return lhs / rhs; }),
        unary
    )(input);
}

template <Input I>
constexpr ParseResultOf<int64_t, I> expr_(I input) {
    return alt(
        map(seq(term, ignore(plus), expr), [](auto lhs, auto rhs) { return lhs + rhs; }),
        map(seq(term, ignore(minus), expr), [](auto lhs, auto rhs) { return lhs - rhs; }),
        term
    )(input);
}

int main() {
    static constexpr auto expression = "1 + (2 + 3) * -(1 + 1)"sv;
    static_assert(*expr(expression) == -9, "test failure!");

    auto res = expr(expression);
    if (res) {
        std::cout << expression << " = " << *res << '\n';
    } else {
        std::cout << "failed to parse \"" << expression << "\"\n";
        return 1;
    }

    return 0;
}
