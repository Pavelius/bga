#pragma once

template<typename... Ts>
constexpr unsigned fg(unsigned p1, Ts... args) {
    if constexpr(!sizeof...(args))
        return 1 << p1;
    else
        return (1 << p1) | fg(args...);
}
