//
// Created by Joe Chrisman on 9/28/22.
//

#include "Squares.h"


std::string squares::toNotation(Square square)
{
    std::string notation = "--";
    int rank = getRank(square);
    int file = getFile(square);
    notation[0] = 'a' + (ENGINE_IS_WHITE ? 8 - file : file);
    notation[1] = '1' + (ENGINE_IS_WHITE ? 8 - rank : rank);
    return notation;
}

/*
 * convert a move into algebraic notation.
 * this is not fully implemented yet, so far it
 * has only been used for debugging purposes
 */
std::string squares::toNotation(const Move& move)
{
    if (move.type == CASTLE)
    {
        return "castle";
    }
    std::string to = toNotation(move.to);
    if (move.moved == PLAYER_PAWN || move.moved == ENGINE_PAWN)
    {
        std::string notation = to;
        if (move.captured != NONE)
        {
            std::string notation = toNotation(move.from).substr(0, 1) + "x" + to.substr(0, 1);
        }
        if (move.type == KNIGHT_PROMOTION)
        {
            notation += "=n";
        }
        else if (move.type == BISHOP_PROMOTION)
        {
            notation += "=b";
        }
        else if (move.type == ROOK_PROMOTION)
        {
            notation += "=r";
        }
        else if (move.type == QUEEN_PROMOTION)
        {
            notation += "=q";
        }
        return notation;
    }
    else
    {
        std::string notation;
        if (move.moved == ENGINE_KNIGHT || move.moved == PLAYER_KNIGHT)
        {
            notation = "n";
        }
        else if (move.moved == ENGINE_BISHOP || move.moved == PLAYER_BISHOP)
        {
            notation = "b";
        }
        else if (move.moved == ENGINE_ROOK || move.moved == PLAYER_ROOK)
        {
            notation = "r";
        }
        else if (move.moved == ENGINE_QUEEN || move.moved == PLAYER_QUEEN)
        {
            notation = "q";
        }
        else if (move.moved == ENGINE_KING || move.moved == PLAYER_KING)
        {
            notation = "k";
        }
        if (move.captured != NONE)
        {
            notation += "x";
        }
        return notation + to;
    }
}

Square squares::toSquare(const std::string& notation)
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
