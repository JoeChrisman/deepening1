//
// Created by Joe Chrisman on 9/28/22.
//

#ifndef BITBOARDS_H_TYPES_H
#define BITBOARDS_H_TYPES_H

typedef unsigned long long int Bitboard;
typedef int Square;

enum Squares
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NULL_SQUARE
};

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


#endif //BITBOARDS_H_TYPES_H
