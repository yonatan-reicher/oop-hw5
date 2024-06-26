#ifndef LIST_H
#define LIST_H

template <typename ... T>
struct List;

// Define list by cases:

// Base case: empty list.
// Does not have head or tail.
template <>
struct List<> {
    static constexpr int size = 0;
};

// Recursive case: non-empty list.
template <typename Head, typename ... Tail>
struct List<Head, Tail...> {
    using head = Head;
    // Accidentally thought the requirement was to have a field called tail,
    // now I don't feel like changing it so we have both.
    using tail = List<Tail...>;
    using next = List<Tail...>;

    static constexpr int size = 1 + tail::size;
};


template <typename Head, typename Rest>
struct PrependList;

// PrependList is defined exactly the same!

// only + []
template <typename Only>
struct PrependList<Only, List<>> {
    using list = List<Only>;
};

// [head] + [old_head, tail...]
template <typename Head, typename OldHead, typename ... Tail>
struct PrependList<Head, List<OldHead, Tail...>> {
    using list = List<Head, OldHead, Tail...>;
};


// This is just like prepend, but from the back! Useful in some cases.
template <typename Head, typename Rest>
struct AppendList;

// only + []
template <typename Only>
struct AppendList<Only, List<>> {
    using list = List<Only>;
};

// [head, tail...] + [X] = [head] + ([tail...] + [X])
template <typename Head, typename ... Tail, typename X>
struct AppendList<X, List<Head, Tail...>> {
    using list = typename PrependList<Head, typename AppendList<X, List<Tail...>>::list>::list;
};


template <int N, typename List>
struct GetAtIndex;

// list[0]
template <typename Head, typename ... Tail>
struct GetAtIndex<0, List<Head, Tail...>> {
    // The value at index 0 is the head of the list.
    // value, just like head and tail, is also actually a type.
    using value = Head;
};

// list[N] = list[1:][N - 1]
template <int N, typename Head, typename ... Tail>
struct GetAtIndex<N, List<Head, Tail...>> {
    using value = typename GetAtIndex<N - 1, List<Tail...>>::value;
};

template <int N, typename X, typename List>
struct SetAtIndex;

// [X] + list[1:]   
template <typename X, typename Head, typename ... Tail>
struct SetAtIndex<0, X, List<Head, Tail...>> {
    using list = List<X, Tail...>;
};

// [head] + SetAtIndex(N - 1, X, [tail...])
template <int N, typename X, typename Head, typename ... Tail>
struct SetAtIndex<N, X, List<Head, Tail...>> {
    using rec = typename SetAtIndex<N - 1, X, List<Tail...>>::list;
    using list = typename PrependList<Head, rec>::list;
};


template <typename List1, typename List2>
struct Concat;

template <typename ... T2>
struct Concat<List<>, List<T2...>> {
    using list = List<T2...>;
};

template <typename H, typename ... T1, typename ... T2>
struct Concat<List<H, T1...>, List<T2...>> {
    using list = typename PrependList<H, typename Concat<List<T1...>, List<T2...>>::list>::list;
};


#endif
