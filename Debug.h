//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_DEBUG_H
#define DEEPENING1_DEBUG_H

#include <iostream>
#include "Bitboards.h"

void printBitboard(Bitboard board)
{
    std::cout << "--------------\n";
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file <= 7; file++)
        {
            Bitboard square = toBoard(getSquare(rank, file));
            std::cout << (square & board ? "1 " : "0 ");
        }
        std::cout << std::endl;
    }
}

void printEnginePawnMovement()
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Square pawn = getSquare(rank, file);
            Bitboard captures = 0;
            if (rank != 0 && rank != 7)
            {
                if (file != 0)
                {
                    captures |= toBoard(west(south(pawn)));
                }
                if (file != 7)
                {
                    captures  |= toBoard(east(south(pawn)));
                }
            }
            printf("0x%016llx,\n", captures);
        }
    }
}

void printPlayerPawnMovement()
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Square pawn = getSquare(rank, file);
            Bitboard captures = 0;
            if (rank != 0 && rank != 7)
            {
                if (file != 0)
                {
                    captures |= toBoard(west(north(pawn)));
                }
                if (file != 7)
                {
                    captures  |= toBoard(east(north(pawn)));
                }
            }
            printf("0x%016llx,\n", captures);
        }
    }
}

void printKnightMovement()
{
    int offsets[8][2] = {
            {-2, -1},
            {-1, -2},
            {1, 2},
            {2, 1},
            {-1, 2},
            {1, -2},
            {-2, 1},
            {2, -1}
    };
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            Bitboard moves = 0;
            for (int i = 0; i < 8; i++)
            {
                int r = rank + offsets[i][0];
                int f = file + offsets[i][1];
                if (r <= 7 && r >= 0 && f <= 7 && f >= 0)
                {
                    moves |= toBoard(getSquare(r, f));
                }
            }
            printf("0x%016llx,\n", moves);
        }
    }

}

#endif //DEEPENING1_DEBUG_H
