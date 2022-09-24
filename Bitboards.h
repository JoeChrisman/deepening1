//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_BITBOARDS_H
#define DEEPENING1_BITBOARDS_H

#include <x86intrin.h>
#include "Constants.h"
#include "Magics.h"

typedef unsigned long long int Bitboard;
typedef char Square;

/*
 * The least significant bit represents A1, the most significant bit represents H8.
 * The white player always plays from the A1 side, the black player always plays from the H8 side.
 */

enum Squares
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

const Square NULL_SQUARE = -1;

const Bitboard FULL_BITBOARD = 0xffffffffffffffff;
const Bitboard EMPTY_BITBOARD = 0x0000000000000000;

const Bitboard OUTER_SQUARES = 0xFF818181818181FF;

// these squares must be safe for the king to castle queenside
const Bitboard QUEENSIDE_CASTLE_CHECKS = ENGINE_IS_WHITE ? 0x3000000000000030
                                                         : 0x0c0000000000000c;
// these squares must be empty for the king to castle queenside
const Bitboard QUEENSIDE_CASTLE_EMPTIES = ENGINE_IS_WHITE ? 0x7000000000000070
                                                          : 0x0e0000000000000e;

// these squares must be empty and safe for the king to castle kingside
const Bitboard KINGSIDE_CASTLE_CHECKS = ENGINE_IS_WHITE ? 0x0600000000000006
                                                        : 0x6000000000000060;

const Bitboard RANK_0 = 0x00000000000000ff;
const Bitboard RANK_1 = 0x000000000000ff00;
const Bitboard RANK_2 = 0x0000000000ff0000;
const Bitboard RANK_3 = 0x00000000ff000000;
const Bitboard RANK_4 = 0x000000ff00000000;
const Bitboard RANK_5 = 0x0000ff0000000000;
const Bitboard RANK_6 = 0x00ff000000000000;
const Bitboard RANK_7 = 0xff00000000000000;

const Bitboard FILE_0 = 0x0101010101010101;
const Bitboard FILE_1 = 0x0202020202020202;
const Bitboard FILE_2 = 0x0404040404040404;
const Bitboard FILE_3 = 0x0808080808080808;
const Bitboard FILE_4 = 0x1010101010101010;
const Bitboard FILE_5 = 0x2020202020202020;
const Bitboard FILE_6 = 0x4040404040404040;
const Bitboard FILE_7 = 0x8080808080808080;

inline int countPieces(Bitboard board)
{
    return __builtin_popcountll(board);
}

inline Square toSquare(Bitboard board)
{
    return (Square)_mm_tzcnt_64(board);
}

inline Bitboard toBoard(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return (Bitboard)1 << square;
}

inline int getRank(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return square / 8;
}

inline int getFile(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return square % 8;
}

inline Square getSquare(int rank, int file)
{
    assert(rank >= 0);
    assert(rank <= 7);
    assert(file >= 0);
    assert(file <= 7);
    return (Square)(rank * 8 + file);
}

inline Bitboard north(Bitboard board)
{
    return board << 8;
}

inline Bitboard east(Bitboard board)
{
    return board >> 1;
}

inline Bitboard south(Bitboard board)
{
    return board >> 8;
}

inline Bitboard west(Bitboard board)
{
    return board << 1;
}

inline Square north(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return (Square)(square + 8);
}

inline Square east(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return (Square)(square + 1);
}

inline Square south(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return (Square)(square - 8);
}

inline Square west(Square square)
{
    assert(square >= A1);
    assert(square <= H8);
    return (Square)(square - 1);
}

inline bool isOnBoard(Square square)
{
    return square >= A1 && square <= H8;
}

inline bool isOnBoard(int rank, int file)
{
    return rank < 8 && rank >= 0 && file < 8 && file >= 0;
}

inline Square popFirstPiece(Bitboard& board)
{
    Square piece = toSquare(board);
    board ^= toBoard(piece);
    return piece;
}

inline Bitboard popFirstBitboard(Bitboard& board)
{
    Bitboard piece = board & (~board + 1);
    board ^= piece;
    return piece;
}

inline Bitboard getRandomBitboard()
{
    Bitboard board = EMPTY_BITBOARD;
    for (Square square = A1; square <= H8; square++)
    {
        if (rand() % 2)
        {
            board |= toBoard(square);
        }
    }
    return board;
}

const Bitboard PLAYER_PAWN_CAPTURES[64] = {
    0x0000000000000200,
    0x0000000000000500,
    0x0000000000000a00,
    0x0000000000001400,
    0x0000000000002800,
    0x0000000000005000,
    0x000000000000a000,
    0x0000000000004000,
    0x0000000000020000,
    0x0000000000050000,
    0x00000000000a0000,
    0x0000000000140000,
    0x0000000000280000,
    0x0000000000500000,
    0x0000000000a00000,
    0x0000000000400000,
    0x0000000002000000,
    0x0000000005000000,
    0x000000000a000000,
    0x0000000014000000,
    0x0000000028000000,
    0x0000000050000000,
    0x00000000a0000000,
    0x0000000040000000,
    0x0000000200000000,
    0x0000000500000000,
    0x0000000a00000000,
    0x0000001400000000,
    0x0000002800000000,
    0x0000005000000000,
    0x000000a000000000,
    0x0000004000000000,
    0x0000020000000000,
    0x0000050000000000,
    0x00000a0000000000,
    0x0000140000000000,
    0x0000280000000000,
    0x0000500000000000,
    0x0000a00000000000,
    0x0000400000000000,
    0x0002000000000000,
    0x0005000000000000,
    0x000a000000000000,
    0x0014000000000000,
    0x0028000000000000,
    0x0050000000000000,
    0x00a0000000000000,
    0x0040000000000000,
    0x0200000000000000,
    0x0500000000000000,
    0x0a00000000000000,
    0x1400000000000000,
    0x2800000000000000,
    0x5000000000000000,
    0xa000000000000000,
    0x4000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000
};

const Bitboard ENGINE_PAWN_CAPTURES[64] = {
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000000,
    0x0000000000000002,
    0x0000000000000005,
    0x000000000000000a,
    0x0000000000000014,
    0x0000000000000028,
    0x0000000000000050,
    0x00000000000000a0,
    0x0000000000000040,
    0x0000000000000200,
    0x0000000000000500,
    0x0000000000000a00,
    0x0000000000001400,
    0x0000000000002800,
    0x0000000000005000,
    0x000000000000a000,
    0x0000000000004000,
    0x0000000000020000,
    0x0000000000050000,
    0x00000000000a0000,
    0x0000000000140000,
    0x0000000000280000,
    0x0000000000500000,
    0x0000000000a00000,
    0x0000000000400000,
    0x0000000002000000,
    0x0000000005000000,
    0x000000000a000000,
    0x0000000014000000,
    0x0000000028000000,
    0x0000000050000000,
    0x00000000a0000000,
    0x0000000040000000,
    0x0000000200000000,
    0x0000000500000000,
    0x0000000a00000000,
    0x0000001400000000,
    0x0000002800000000,
    0x0000005000000000,
    0x000000a000000000,
    0x0000004000000000,
    0x0000020000000000,
    0x0000050000000000,
    0x00000a0000000000,
    0x0000140000000000,
    0x0000280000000000,
    0x0000500000000000,
    0x0000a00000000000,
    0x0000400000000000,
    0x0002000000000000,
    0x0005000000000000,
    0x000a000000000000,
    0x0014000000000000,
    0x0028000000000000,
    0x0050000000000000,
    0x00a0000000000000,
    0x0040000000000000,
};

const Bitboard KNIGHT_MOVES[64] = {
    0x0000000000020400,
    0x0000000000050800,
    0x00000000000a1100,
    0x0000000000142200,
    0x0000000000284400,
    0x0000000000508800,
    0x0000000000a01000,
    0x0000000000402000,
    0x0000000002040004,
    0x0000000005080008,
    0x000000000a110011,
    0x0000000014220022,
    0x0000000028440044,
    0x0000000050880088,
    0x00000000a0100010,
    0x0000000040200020,
    0x0000000204000402,
    0x0000000508000805,
    0x0000000a1100110a,
    0x0000001422002214,
    0x0000002844004428,
    0x0000005088008850,
    0x000000a0100010a0,
    0x0000004020002040,
    0x0000020400040200,
    0x0000050800080500,
    0x00000a1100110a00,
    0x0000142200221400,
    0x0000284400442800,
    0x0000508800885000,
    0x0000a0100010a000,
    0x0000402000204000,
    0x0002040004020000,
    0x0005080008050000,
    0x000a1100110a0000,
    0x0014220022140000,
    0x0028440044280000,
    0x0050880088500000,
    0x00a0100010a00000,
    0x0040200020400000,
    0x0204000402000000,
    0x0508000805000000,
    0x0a1100110a000000,
    0x1422002214000000,
    0x2844004428000000,
    0x5088008850000000,
    0xa0100010a0000000,
    0x4020002040000000,
    0x0400040200000000,
    0x0800080500000000,
    0x1100110a00000000,
    0x2200221400000000,
    0x4400442800000000,
    0x8800885000000000,
    0x100010a000000000,
    0x2000204000000000,
    0x0004020000000000,
    0x0008050000000000,
    0x00110a0000000000,
    0x0022140000000000,
    0x0044280000000000,
    0x0088500000000000,
    0x0010a00000000000,
    0x0020400000000000,
};

const Bitboard KING_MOVES[64] = {
    0x0000000000000302,
    0x0000000000000705,
    0x0000000000000e0a,
    0x0000000000001c14,
    0x0000000000003828,
    0x0000000000007050,
    0x000000000000e0a0,
    0x000000000000c040,
    0x0000000000030203,
    0x0000000000070507,
    0x00000000000e0a0e,
    0x00000000001c141c,
    0x0000000000382838,
    0x0000000000705070,
    0x0000000000e0a0e0,
    0x0000000000c040c0,
    0x0000000003020300,
    0x0000000007050700,
    0x000000000e0a0e00,
    0x000000001c141c00,
    0x0000000038283800,
    0x0000000070507000,
    0x00000000e0a0e000,
    0x00000000c040c000,
    0x0000000302030000,
    0x0000000705070000,
    0x0000000e0a0e0000,
    0x0000001c141c0000,
    0x0000003828380000,
    0x0000007050700000,
    0x000000e0a0e00000,
    0x000000c040c00000,
    0x0000030203000000,
    0x0000070507000000,
    0x00000e0a0e000000,
    0x00001c141c000000,
    0x0000382838000000,
    0x0000705070000000,
    0x0000e0a0e0000000,
    0x0000c040c0000000,
    0x0003020300000000,
    0x0007050700000000,
    0x000e0a0e00000000,
    0x001c141c00000000,
    0x0038283800000000,
    0x0070507000000000,
    0x00e0a0e000000000,
    0x00c040c000000000,
    0x0302030000000000,
    0x0705070000000000,
    0x0e0a0e0000000000,
    0x1c141c0000000000,
    0x3828380000000000,
    0x7050700000000000,
    0xe0a0e00000000000,
    0xc040c00000000000,
    0x0203000000000000,
    0x0507000000000000,
    0x0a0e000000000000,
    0x141c000000000000,
    0x2838000000000000,
    0x5070000000000000,
    0xa0e0000000000000,
    0x40c0000000000000,
};

#endif //DEEPENING1_BITBOARDS_H
