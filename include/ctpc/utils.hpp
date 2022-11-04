#ifndef CTPC_UTILS_HPP
#define CTPC_UTILS_HPP

#define CTPC_F(name) \
    [](auto&&... args) -> decltype(auto) { \
        return name(std::forward<decltype(args)>(args)...); \
    }

#endif
