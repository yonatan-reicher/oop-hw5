#ifndef CODE_MOVEVEHICLE_H
#define CODE_MOVEVEHICLE_H

#include "CellType.h"
#include "Direction.h"
#include "GameBoard.h"
#include "Utilities.h"
#include <iostream>
#include "Printer.h"
#include "TransposeList.h"


template <CellType Type, Direction Dir, int Amount>
struct Move {
    static constexpr CellType type = Type;
    static constexpr Direction dir = Dir;
    static constexpr int amount = Amount;

    static_assert(type != CellType::EMPTY, "Tried moving an empty cell!");
};

template <int Row, int Col, Direction Dir, int Amount>
struct OffsetCell {
    static_assert(Amount > 0, "Amount must be positive!");

    static constexpr int row =
        Row + (Dir == Direction::DOWN ? Amount : 0) - (Dir == Direction::UP ? Amount : 0);
    static constexpr int col = 
        Col + (Dir == Direction::RIGHT ? Amount : 0) - (Dir == Direction::LEFT ? Amount : 0);
};

template <typename B, int Row, int Col, Direction Dir, int Amount>
struct BoundedOffsetCell {
    using offset = OffsetCell<Row, Col, Dir, Amount>;
    static constexpr int row = std::max(0, std::min(offset::row, GameBoard<B>::width - 1));
    static constexpr int col = std::max(0, std::min(offset::col, GameBoard<B>::height - 1));
};

template <typename B, int Row, int Col>
struct GetCell {
    static_assert(0 <= Row && Row < GameBoard<B>::width, "Tried getting a cell outside the board!");
    static_assert(0 <= Col && Col < GameBoard<B>::height, "Tried getting a cell outside the board!");

    using cell = typename GetAtIndex<Col, typename GetAtIndex<Row, B>::value>::value;
};


// Takes a cell, a direction and an path length. Checks that all cells in the
// path are empty.
template <typename B, int Row, int Col, Direction Dir, int Amount>
struct AssertPathEmpty {

    using next = OffsetCell<Row, Col, Dir, 1>;
    using next_cell = typename GetCell<B, next::row, next::col>::cell;

    static_assert(
        next_cell::type == CellType::EMPTY,
        "Tried moving to a non-empty cell!"
    );

    AssertPathEmpty<
        B,
        next::row,
        next::col,
        Dir,
        Amount - 1
    > recurse;
};

template <typename B, int Row, int Col, typename BoardCell>
struct SetCell {
    static_assert(
        0 <= Row && Row < GameBoard<B>::width,
        "Tried setting a cell outside the board!"
    );
    static_assert(
        0 <= Col && Col < GameBoard<B>::height,
        "Tried setting a cell outside the board!"
    );

    using row = typename GetAtIndex<Row, B>::value;
    using board = typename SetAtIndex<
        Col,
        typename SetAtIndex<
            Row,
            BoardCell,
            row
        >::value,
        B
    >::value;
};

template <int Row, int Col>
struct RowCol {};

template <
    typename B, int Row, int Col,
    typename Cell = typename GetCell<B, Row, Col>::cell,
    typename ThisCell = typename GetCell<B, Row, Col>::cell,
    bool Valid =
        // Only do the deletion if the indices are valid...
        0 <= Row && Row < GameBoard<B>::width
        && 0 <= Col && Col < GameBoard<B>::height
        // And the cell at the index (ThisCell) matches the cell of the car we
        // are deleting (Cell).
        && Cell::type == ThisCell::type
        && Cell::direction == ThisCell::direction
        && Cell::amount == ThisCell::amount
>
struct DeleteCar;

// Valid case! Do the deletion.
template < typename B, int Row, int Col, typename Cell, typename ThisCell>
struct DeleteCar<B, Row, Col, Cell, ThisCell, true> {
    using right = typename BoundedOffsetCell<B, Row, Col, Direction::RIGHT, 1>::cell;
    using left = typename BoundedOffsetCell<B, Row, Col, Direction::LEFT, 1>::cell;
    using up = typename BoundedOffsetCell<B, Row, Col, Direction::UP, 1>::cell;
    using down = typename BoundedOffsetCell<B, Row, Col, Direction::DOWN, 1>::cell;

    using board0 = SetCell<
        B,
        Row, Col,
        BoardCell<CellType::EMPTY, Direction::RIGHT, 0>
    >;
    using board1 = DeleteCar<typename board0::board, up::row, up::col, Cell>;
    using board2 = DeleteCar<typename board1::board, down::row, down::col, Cell>;
    using board3 = DeleteCar<typename board2::board, left::row, left::col, Cell>;
    using board4 = DeleteCar<typename board3::board, right::row, right::col, Cell>;

    using board = typename board4::board;
    using deleted = Concat<
        typename board1::deleted,
        Concat<
            typename board2::deleted,
            Concat<
                typename board3::deleted,
                Concat<
                    typename board4::deleted,
                    List<RowCol<Row, Col>>
                >
            >
        >
    >;
};

// This cell is invalid! Do nothing.
template < typename B, int Row, int Col, typename Cell, typename ThisCell>
struct DeleteCar<B, Row, Col, Cell, ThisCell, false> {
    using board = B;
    using deleted = List<>;
};


template <typename B, typename Positions, typename CellToSet>
struct SetCells;

template <typename B, typename CellToSet>
struct SetCells<B, List<>, CellToSet> {
    using board = B;
};

template <typename B, typename Head, typename... Tail, typename CellToSet>
struct SetCells<B, List<Head, Tail...>, CellToSet> {
    // TODO: Assert that this spot is empty!
    using board = typename SetCell<
        typename SetCells<B, List<Tail...>, CellToSet>::board,
        Head::row,
        Head::col,
        CellToSet
    >::board;
};


constexpr Direction opposite(Direction dir) {
    switch (dir) {
        case Direction::UP:
            return Direction::DOWN;
        case Direction::DOWN:
            return Direction::UP;
        case Direction::LEFT:
            return Direction::RIGHT;
        case Direction::RIGHT:
            return Direction::LEFT;
    }
    return Direction::UP;
}


template <typename GameBoard, int Row, int Col, Direction Dir, int Amount>
struct MoveVehicle {
    static_assert(false, "First argument to MoveVehicle must be a GameBoard!");
};

template <typename B, int Row, int Col, Direction Dir, int Amount>
struct MoveVehicle<GameBoard<B>, Row, Col, Dir, Amount> {
    static constexpr int width = GameBoard<B>::width;
    static constexpr int height = GameBoard<B>::height;

    using cell = typename GetAtIndex<Col, typename GetAtIndex<Row, B>::value>::value;

    static_assert(0 <= Row && Row < width, "Tried moving at a cell outside the board!");
    static_assert(0 <= Col && Col < height, "Tried moving at a cell outside the board!");
    static_assert(cell::type != CellType::EMPTY, "Tried moving an empty cell!");
    static_assert(cell::direction == Dir || cell::direction == opposite(Dir), "Tried moving a car in the wrong direction!");

    // This member holds the board after the execution of the move.
    using delete_car = DeleteCar<B, Row, Col>;
    AssertPathEmpty<B, Row, Col, Dir, Amount> assert_path_empty;
    using board = typename SetCells<
        typename delete_car::board,
        typename delete_car::deleted,
        cell
    >::board;
};


// Try to instantiate it:
using cell = BoardCell<CellType::X, Direction::RIGHT, 1>;
using empty_cell = BoardCell<CellType::EMPTY, Direction::RIGHT, 0>;
using board = GameBoard<List<List<cell, empty_cell>, List<empty_cell, empty_cell>>>;

template class MoveVehicle<board, 0, 0, Direction::RIGHT, 1>;


#endif //CODE_MOVEVEHICLE_H
