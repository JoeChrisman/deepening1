//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_POSITION_H
#define DEEPENING1_POSITION_H

#include <vector>
#include <string>
#include "Bitboards.h"
#include "Constants.h"

class Position
{

public:
    // create a chess game from a FEN string
    Position(const std::string& fen);

    Piece getPiece(Square square);

    /*
     * a vector of bitboards of pieces. there is one bitboard for each piece type.
     * the vector is indexed using the Piece enumeration in Constants.h
     */
    std::vector<Bitboard> pieces;

private:


};


#endif //DEEPENING1_POSITION_H
