#ifndef CTPC_FLAT_MAP_HPP
#define CTPC_FLAT_MAP_HPP

#include "parser.hpp"
#include "input.hpp"
#include "parse_result.hpp"
#include "utils.hpp"

namespace ctpc {

namespace detail {

template <typename Parser, typename Combinator>
struct FlatMapParser {
  private:
    [[no_unique_address]] Parser parser_;
    [[no_unique_address]] Combinator combinator_;

  public:
    constexpr FlatMapParser(Parser&& parser, Combinator&& combinator)
        : parser_(std::forward<Parser>(parser)),
          combinator_(std::forward<Combinator>(combinator)) {}

    template <ParseableBy<Parser> I>
    constexpr auto operator()(I input) const {
        auto tmp = parser_(input);
        if (tmp) {
            auto rem = tmp.remaining();
            return utils::invoke_unpacked(combinator_, *std::move(tmp))(rem);
        }
        return fail<std::remove_cvref_t<decltype(utils::invoke_unpacked(combinator_, *std::move(tmp))(tmp.remaining()))>::value_type>(input);
    }
};

}

struct FlatMap {
    template <typename Parser, typename Combinator>
    constexpr auto operator()(Parser&& parser, Combinator&& combinator) -> detail::FlatMapParser<Parser, Combinator> {
        return {std::forward<Parser>(parser), std::forward<Combinator>(combinator)};
    }
};

static constexpr FlatMap flat_map{};

}

#endif
