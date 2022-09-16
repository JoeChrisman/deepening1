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

#endif //DEEPENING1_DEBUG_H
