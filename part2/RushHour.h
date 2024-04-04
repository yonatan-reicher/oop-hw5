#ifndef RUSHHOUR_H
#define RUSHHOUR_H

#include "List.h"
#include "Utilities.h"
#include "CellType.h"
#include "Direction.h"
#include "BoardCell.h"
#include "GameBoard.h"


template <typename RowList>
struct AllEmptyOrX {
    static_assert(false, "RowList was not a list of cells!");
};

template <>
struct AllEmptyOrX<List<>> {
    static constexpr bool result = true;
};

template <typename Head, typename ... Tail>
struct AllEmptyOrX<List<Head, Tail...>> {
    static constexpr bool result =
        (Head::type == CellType::EMPTY || Head::type == CellType::X)
        && AllEmptyOrX<List<Tail...>>::result;
};


template <typename GameBoard>
struct CheckWin {
    static_assert(false, "GameBoard was not a gameboard!");
};

// CheckWin([]) = false
template <>
struct CheckWin<GameBoard<List<>>> {
    static constexpr bool result = false;
};

// CheckWin([[]]) = false
template <>
struct CheckWin<GameBoard<List<List<>>>> {
    static constexpr bool result = false;
};

// CheckWin([ [head] + [tail...] ]) = ...
template <typename Head, typename ... Tail>
struct CheckWin<GameBoard<List<List<Head, Tail...>>>> {
    static constexpr bool result =
        (Head::type == CellType::X && AllEmptyOrX<List<Tail...>>::result)
        || CheckWin<GameBoard<List<Tail...>>>::result;
};

// CheckWin([head, tail...]) = CheckWin([head]) || CheckWin([tail...])

template <typename Head, typename ... Tail>
struct CheckWin<GameBoard<List<Head, Tail...>>> {
    using first_row = CheckWin<List<Head>>;
    using rest = CheckWin<GameBoard<List<Tail...>>>;
    static constexpr bool result = first_row::result || rest::result;
};

#endif
