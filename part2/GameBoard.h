#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "List.h"
#include "Utilities.h"
#include "BoardCell.h"

template <typename Board>
struct GameBoard {
    using board = Board;

    static constexpr int width = 
        // Here, if we did not use AppendList::list, we could not call head.
        AppendList<List<>, board>::list::head::size;
    static constexpr int height = Board::size;

};

// Check that empty board compiles. Just in case.
template class GameBoard<List<>>;

#endif
