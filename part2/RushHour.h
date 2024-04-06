#ifndef RUSHHOUR_H
#define RUSHHOUR_H

#include "List.h"
#include "Utilities.h"
#include "CellType.h"
#include "Direction.h"
#include "BoardCell.h"
#include "GameBoard.h"
#include "MoveVehicle.h"

// Circular dependency! Weird, but okay.
template <typename B, int Row, int Col>
struct GetCell;

template <typename GameBoard, int Row, int Col, Direction Dir, int Amount>
struct MoveVehicle;


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
        || CheckWin<GameBoard<List<List<Tail...>>>>::result;
};

// CheckWin([head, tail...]) = CheckWin([head]) || CheckWin([tail...])

template <typename Head, typename ... Tail>
struct CheckWin<GameBoard<List<Head, Tail...>>> {
    using first_row = CheckWin<GameBoard<List<Head>>>;
    using rest = CheckWin<GameBoard<List<Tail...>>>;
    static constexpr bool result = first_row::result || rest::result;
};

template <
    typename B, typename Move0,
    int Row = 0, int Col = 0,
    bool BoundReached =
        Row >= GameBoard<B>::height || Col >= GameBoard<B>::width
>
struct FindRowCol;

template <typename B, typename Move0, int Row, int Col>
struct FindRowCol<B, Move0, Row, Col, false> {
    using cell = typename GetCell<B, Row, Col>::cell;

    using RecDown = FindRowCol<B, Move0, Row + 1, Col>;
    using RecRight = FindRowCol<B, Move0, Row, Col + 1>;

    static constexpr bool found_here =
        cell::type == Move0::type
        && (cell::direction == Move0::direction || cell::direction == opposite(Move0::direction));
    static constexpr bool found = found_here || RecDown::found || RecRight::found;

    static constexpr int row =
        found_here ? Row :
        RecDown::found ? RecDown::row :
        RecRight::row;
    static constexpr int col =
        found_here ? Col :
        RecDown::found ? RecDown::col :
        RecRight::col;
};

template <typename B, typename Move0, int Row, int Col>
struct FindRowCol<B, Move0, Row, Col, true> {
    static constexpr bool found = false;
    static constexpr int row = -1;
    static constexpr int col = -1;
};

template <typename GB, typename MoveList>
struct CheckSolution;

template <typename B>
struct CheckSolution<GameBoard<B>, List<>> {
    static constexpr bool result = CheckWin<GameBoard<B>>::result;
};

template <typename B, typename Move0, typename ... MovesRest>
struct CheckSolution<GameBoard<B>, List<Move0, MovesRest...>> {
    using find = FindRowCol<B, Move0>;
    using GBAfterMove0 = typename MoveVehicle<
        GameBoard<B>,
        find::row, find::col, Move0::direction, Move0::amount
    >::board;
    using Rec = CheckSolution<GBAfterMove0, List<MovesRest...>>;

    static constexpr bool result = Rec::result;
};

/*
typedef GameBoard< List<
        List < BoardCell< EMPTY , RIGHT , 0 >, BoardCell< EMPTY , RIGHT , 0 >, BoardCell< EMPTY , RIGHT , 0>, BoardCell< X , RIGHT , 1 >, BoardCell< EMPTY , RIGHT , 0>, BoardCell< EMPTY , RIGHT , 0>, BoardCell< EMPTY , RIGHT , 0>>
>> solvedBoard;

template class CheckSolution<solvedBoard, List<Move < X, RIGHT, 1 >>>;
*/


#endif
