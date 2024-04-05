#ifndef RUSHHOUR_H
#define RUSHHOUR_H

#include "List.h"
#include "Utilities.h"
#include "CellType.h"
#include "Direction.h"
#include "BoardCell.h"
#include "GameBoard.h"
#include "MoveVehicle.h"


template <typename RowList>
struct AllEmptyOrX;

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
struct CheckWin;

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

template <typename GB, typename MoveList>
struct CheckSolution;

template <typename B>
struct CheckSolution<GameBoard<B>, List<>> {
    static constexpr bool result = CheckWin<GameBoard<B>>::result;
};

template <typename B, typename Move0, typename ... MovesRest>
struct CheckSolution<GameBoard<B>, List<Move0, MovesRest...>> {
    // TODO: Rename
    using GBAfterMove0 = typename MoveVehicle<
        GameBoard<B>,
        Move0::row, Move0::col, Move0::dir, Move0::amount
    >::board;
    using Rec = CheckSolution<GBAfterMove0, List<MovesRest...>>;

    // TODO:
    static constexpr bool result = Rec::result;
};

#endif
