//
// Created by Joe Chrisman on 9/28/22.
//

#include "Squares.h"


std::string squares::toNotation(Square square)
{
    std::string notation = "--";
    int rank = getRank(square);
    int file = getFile(square);
    notation[0] = 'a' + (ENGINE_IS_WHITE ? 7 - file : file);
    notation[1] = '1' + (ENGINE_IS_WHITE ? 7 - rank : rank);
    return notation;
}

Square squares::toSquare(std::string notation)
{
    if (notation[0] >= 'a' && notation[0] <= 'h' &&
        notation[1] >= '1' && notation[1] <= '8')
    {
        int file = (ENGINE_IS_WHITE ? 'h' - notation[0] : notation[0] - 'a');
        int rank = (ENGINE_IS_WHITE ? '8' - notation[1] : notation[1] - '1');
        return getSquare(rank, file);
    }
    return NULL_SQUARE;
}
