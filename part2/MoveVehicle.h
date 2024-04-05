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

template <typename B, int Row, int Col, Direction Dir>
struct AssertPathEmpty<B, Row, Col, Dir, 0> {
    // End of recursion!
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
        >::list,
        B
    >::list;
};

template <int Row, int Col>
struct RowCol {
    static constexpr int row = Row;
    static constexpr int col = Col;
};

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
        && Cell::length == ThisCell::length
>
struct DeleteCar;

// Valid case! Do the deletion.
template < typename B, int Row, int Col, typename Cell, typename ThisCell>
struct DeleteCar<B, Row, Col, Cell, ThisCell, true> {
    using right_offset = BoundedOffsetCell<B, Row, Col, Direction::RIGHT, 1>;
    using left_offset = BoundedOffsetCell<B, Row, Col, Direction::LEFT, 1>;
    using up_offset = BoundedOffsetCell<B, Row, Col, Direction::UP, 1>;
    using down_offset = BoundedOffsetCell<B, Row, Col, Direction::DOWN, 1>;
    using right = typename GetCell<B, right_offset::row, right_offset::col>::cell;
    using left = typename GetCell<B, left_offset::row, left_offset::col>::cell;
    using up = typename GetCell<B, up_offset::row, up_offset::col>::cell;
    using down = typename GetCell<B, down_offset::row, down_offset::col>::cell;

    using board0 = SetCell<
        B,
        Row, Col,
        BoardCell<CellType::EMPTY, Direction::RIGHT, 0>
    >;
    using board1 = DeleteCar<typename board0::board, up_offset::row, up_offset::col, Cell>;
    using board2 = DeleteCar<typename board1::board, down_offset::row, down_offset::col, Cell>;
    using board3 = DeleteCar<typename board2::board, left_offset::row, left_offset::col, Cell>;
    using board4 = DeleteCar<typename board3::board, right_offset::row, right_offset::col, Cell>;

    using board = typename board4::board;
    using deleted = typename Concat<
        typename board1::deleted,
        typename Concat<
            typename board2::deleted,
            typename Concat<
                typename board3::deleted,
                typename Concat<
                    typename board4::deleted,
                    List<RowCol<Row, Col>>
                >::list
            >::list
        >::list
    >::list;
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
struct MoveVehicle;

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
    AssertPathEmpty<typename delete_car::board, Row, Col, Dir, Amount> assert_path_empty;
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
