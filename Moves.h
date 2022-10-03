//
// Created by Joe Chrisman on 9/30/22.
//

#ifndef MAIN_CPP_MOVES_H
#define MAIN_CPP_MOVES_H

#include "Squares.h"

enum PieceType
{
    PLAYER_PAWN,
    PLAYER_KNIGHT,
    PLAYER_BISHOP,
    PLAYER_ROOK,
    PLAYER_QUEEN,
    PLAYER_KING,
    ENGINE_PAWN,
    ENGINE_KNIGHT,
    ENGINE_BISHOP,
    ENGINE_ROOK,
    ENGINE_QUEEN,
    ENGINE_KING,
    NONE
};

enum MoveType
{
    NORMAL,
    CASTLE,
    EN_PASSANT,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION
};

struct Move
{
    MoveType type;
    Square from;
    Square to;
    PieceType moved;
    PieceType captured;
};

namespace moves
{
    std::string toNotation(Move move);
    bool isIrreversible(Move move);
}

#endif //MAIN_CPP_MOVES_H
