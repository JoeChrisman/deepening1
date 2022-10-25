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

typedef unsigned int Move;

/*
 * a move is encoded as a 32 bit unsigned integer.
 * the first 23 bits are used, and last 9 bits are unused.
 * this is the encoding scheme:
 *
 * 0b
 * 111 (MoveType 1->7)
 * 1111 (PieceType moved 1->13)
 * 1111 (PieceType captured 1->13)
 * 111111 (Square from 0->63)
 * 111111 (Square to 0->63)
 * 000000000 (Unused bits)
 *
 * if a move is zero, that move can be considered a null move
 */

inline Move makeMove(MoveType moveType, PieceType moved, PieceType captured, Square from, Square to)
{
    int move = moveType << 29;
    move |= moved << 25;
    move |= captured << 21;
    move |= from << 15;
    move |= to << 9;
    return move;
}

inline MoveType getMoveType(Move move)
{
    return (MoveType)((move & 0b11100000000000000000000000000000) >> 29);
}

inline PieceType getPieceMoved(Move move)
{
    return (PieceType)((move & 0b00011110000000000000000000000000) >> 25);
}

inline PieceType getPieceCaptured(Move move)
{
    return (PieceType)((move & 0b00000001111000000000000000000000) >> 21);
}

inline Square getSquareFrom(Move move)
{
    return (Square)((move & 0b00000000000111111000000000000000) >> 15);
}

inline Square getSquareTo(Move move)
{
    return (Square)((move & 0b00000000000000000111111000000000) >> 9);
}

namespace moves
{
    std::string toNotation(Move move);
    bool isIrreversible(Move move);
}

#endif //MAIN_CPP_MOVES_H
