//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_POSITION_H
#define DEEPENING1_POSITION_H

#include <vector>
#include <string>
#include "Bitboards.h"
#include "Constants.h"

struct Move
{
    Square from;
    Square to;
    Piece moved;
    Piece captured;
};

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

    // some extra information about the position
    Bitboard empties;
    Bitboard occupied;
    Bitboard enginePieces;
    Bitboard playerPieces;
    Bitboard playerMovable; // engine pieces or empty squares
    Bitboard engineMovable; // player pieces or empty squares
    // update the additional information
    void updateBitboards();

    void makeMove(Move& move);

private:


};


#endif //DEEPENING1_POSITION_H
