//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_BITBOARDS_H
#define DEEPENING1_BITBOARDS_H

#include <x86intrin.h>

typedef unsigned long long int Bitboard;
typedef char Square;

#define countPieces(board) __builtin_popcountll(board)
#define toSquare(board) _mm_tzcnt_64(board)
#define toBoard(square) (Bitboard)1 << (square)

#define getRank(square) ((square) / 8)
#define getFile(square) ((square) % 8)
#define getSquare(rank, file) ((rank) * 8 + (file))

#define north(square) ((square) + 8)
#define south(square) ((square) - 8)
#define east(square) ((square) - 1)
#define west(square) ((square) + 1)

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

inline Square popFirstPiece(Bitboard& board)
{
    Square leastSquare = toSquare(board);
    board ^= toBoard(leastSquare);
    return leastSquare;
}

inline Bitboard popFirstBitboard(Bitboard& board)
{
    Bitboard leastBoard = board & (~board + 1);
    board ^= leastBoard;
    return leastBoard;
}

#endif //DEEPENING1_BITBOARDS_H
