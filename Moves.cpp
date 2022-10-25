//
// Created by Joe Chrisman on 9/30/22.
//

#include "Moves.h"

/*
 * when we encounter an irreversible move, we can do special things,
 * such as clearing all positions from the repetitions vector.
 * An irreversible move is a move that causes the game to not legally
 * be able to reach any positions that arose before the move was made.
 */
bool moves::isIrreversible(Move move)
{
    return getPieceCaptured(move) != NONE ||
           getPieceMoved(move) == ENGINE_PAWN ||
           getPieceMoved(move) == PLAYER_PAWN;
}

/*
 * convert a move into algebraic notation.
 * this is not fully implemented yet, so far it
 * has only been used for debugging purposes
 */
std::string moves::toNotation(Move move)
{
    MoveType moveType = getMoveType(move);
    PieceType pieceMoved = getPieceMoved(move);
    PieceType pieceCaptured = getPieceCaptured(move);

    if (moveType == CASTLE)
    {
        return "castle";
    }
    std::string to = squares::toNotation(getSquareTo(move));
    if (pieceMoved == PLAYER_PAWN || pieceMoved == ENGINE_PAWN)
    {
        std::string notation = to;
        if (pieceCaptured != NONE)
        {
            notation = squares::toNotation(getSquareFrom(move)).substr(0, 1) + "x" + to.substr(0, 1);
        }
        if (moveType == KNIGHT_PROMOTION)
        {
            notation += "=n";
        }
        else if (moveType == BISHOP_PROMOTION)
        {
            notation += "=b";
        }
        else if (moveType == ROOK_PROMOTION)
        {
            notation += "=r";
        }
        else if (moveType == QUEEN_PROMOTION)
        {
            notation += "=q";
        }
        return notation;
    }
    else
    {
        std::string notation;
        if (pieceMoved == ENGINE_KNIGHT || pieceMoved == PLAYER_KNIGHT)
        {
            notation = "n";
        }
        else if (pieceMoved == ENGINE_BISHOP || pieceMoved == PLAYER_BISHOP)
        {
            notation = "b";
        }
        else if (pieceMoved == ENGINE_ROOK || pieceMoved == PLAYER_ROOK)
        {
            notation = "r";
        }
        else if (pieceMoved == ENGINE_QUEEN || pieceMoved == PLAYER_QUEEN)
        {
            notation = "q";
        }
        else if (pieceMoved == ENGINE_KING || pieceMoved == PLAYER_KING)
        {
            notation = "k";
        }
        if (pieceCaptured != NONE)
        {
            notation += "x";
        }
        return notation + to;
    }
}