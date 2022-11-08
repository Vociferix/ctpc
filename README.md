# ctpc - Compile Time Parser Combinator

ctpc stands for "Compile Time Parser Combinator". This library is a
collection of composable functions to build parsers of both text and
binary input in C++. Such parsers can be executed at runtime or in a
`constexpr` (compile-time) context.

Compared to parser generators, such YACC and Bison, ctpc has no
inherent concept of grammars, productions, or lexing. Parsers built
with ctpc are more akin to recursive descent parsers and will simply
parse exactly as instructed. This means that an "incorrect" parser is
not necessarily a compile error, and may result in undesired behavior,
such as unbounded recursion ending in a stack overflow. But this does
also allow for grammars that might not be accepted by an LR, LALR, or
other kind of parser generator.

ctpc was inspired by the `nom` library for the Rust programming
language. Credit to the `nom` developers for their ingenuity in API
design and parser architecture. See the
[`nom` library](https://github.com/Geal/nom) for more information on
`nom`.

## Defining a Parser
There are two main methods for defining a single parser. Which of the
two methods you use can usually be determined by whether the parser
will need to recurse, directly or indirectly.

The first and simplest method is as follows:
```cpp
// assume that `number` and `plus` are previously defined parsers
static constexpr auto sum_expr = map(
    seq(number, plus, number),
    [](auto lhs, auto rhs) { return lhs + rhs; }
);
```

In this first method, the parser `sum_expr` is defined in line as the
parser returned by the expression to the right of the `=`, which is a
parser that succeeds on the pattern of a `number`, followed by a
`plus`, followed by another `number`, where `number` and `plus` are
themselves parsers (that presumably parse some sort of number string
and add operator, respectively). Additionally, this parser takes the
two parsed `number`s, and maps them to to a single sum value as the
result value of `sum_expr`.

The second, more flexible but verobse, method is as follows:
```cpp
// forward declaration
template <Input I>
constexpr ParseResultOf<int, I> sum_expr_(I input);
static constexpr auto sum_expr = CTPC_F(sum_expr_);

// definition
template <Input I>
constexpr ParseResultOf<int, I> sum_expr_(I input) {
    return alt(
        map(seq(number, plus, sum_expr),
            [](auto lhs, auto rhs) { return lhs + rhs; }),
        number
    );
}
```

In this second method, we have the same parser as in the first method,
except that it is now recursive. It will parse multiple `numbers`
separated by a `plus`, and add them all together. A single `number`
will return the `number` value as is, and the pattern '`number` `plus`
`number` `plus` `number`' will also be parsed and return the values of
all the `number`s summed together.

Of these two methods, the first is preferred when it can be used,
simply for it's brevity and clarity. But most parsers will require at
least some uses of the second method in order to provide a recursively
defined parser. When using the second method, the best practice is to
forward declare all such parsers before any definitions, otherwise it
quickly becomes easy to run into compiler errors about using an
identifier before it has been declared. Such is the nature of
recursion in C++.

Looking at our method 2 example, we the use the `CTPC_F(...)` macro
in the forward declaration, and the actual parser function is
postfixed with an underscore, only to be renamed immediately after. In
C++, function templates cannot be passed to another function that
needs to deduce the type of the function template. Passing `sum_expr_`
directly to any of the ctpc combinators will result in a compile error
as a result. `CTPC_F(...)` converts the template into a
"function object", which has a single known type, but a generic call
operator. C++ is able to make sense of this, so `CTPC_F(...)` is
provided to solve this problem. Note that this also requires that
`sum_expr_` has an explicitly declared return type in order to avoid
further compile errors about calling a function before `auto` return
type can be deduced. `CTPC_F(...)` is merely a convenience. The macro
expands to a generic C++ lambda that internally calls the function
template.
