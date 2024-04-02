#ifndef UTILITIES_H
#define UTILITIES_H

template <bool B, typename T, typename F>
struct Conditional;

template <typename T, typename F>
struct Conditional<true, T, F> {
    using value = T;
};

template <typename T, typename F>
struct Conditional<false, T, F> {
    using value = F;
};


template <bool B, int T, int F>
struct ConditionalInteger;

template <int T, int F>
struct ConditionalInteger<true, T, F> {
    static constexpr int value = T;
};

template <int T, int F>
struct ConditionalInteger<false, T, F> {
    static constexpr int value = F;
};

#endif
