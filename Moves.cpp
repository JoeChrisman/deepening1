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
    return move.captured != NONE ||
           move.moved == ENGINE_PAWN ||
           move.moved == PLAYER_PAWN;
}

/*
 * convert a move into algebraic notation.
 * this is not fully implemented yet, so far it
 * has only been used for debugging purposes
 */
std::string moves::toNotation(Move move)
{
    if (move.type == CASTLE)
    {
        return "castle";
    }
    std::string to = squares::toNotation(move.to);
    if (move.moved == PLAYER_PAWN || move.moved == ENGINE_PAWN)
    {
        std::string notation = to;
        if (move.captured != NONE)
        {
            notation = squares::toNotation(move.from).substr(0, 1) + "x" + to.substr(0, 1);
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