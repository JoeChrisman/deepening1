//
// Created by Joe Chrisman on 9/17/22.
//

#ifndef DEEPENING1_MOVEGEN_H
#define DEEPENING1_MOVEGEN_H

#include "Position.h"

class MoveGen {
public:
    MoveGen();

    Position position;

    std::vector<Move> moveList;

    Bitboard empties;
    Bitboard occupied;
    Bitboard enginePieces;
    Bitboard playerPieces;
    Bitboard playerMovable; // engine pieces or empty squares
    Bitboard engineMovable; // player pieces or empty squares
    // update the bitboards that tell us where we can move - the five bitboards above
    void updateBitboards();

    // generate quiet moves as well as captures
    void genEngineMoves();
    void genPlayerMoves();
    // generate only captures - for use in a quiescence search
    void genEngineCaptures();
    void genPlayerCaptures();

private:

    // squares we can move a piece to without leaving the king in check
    Bitboard safeSquares;

    template<bool isEngine>
    void updateSafeSquares();

    template<bool isEngine, bool quiets>
    void genPawnMoves();

    template<bool isEngine, bool quiets>
    void genKnightMoves();

    template<bool isEngine, bool quiets>
    void genKingMoves();

    template<bool isEngine, bool quiets>
    void genRookMoves();

    template<bool isEngine, bool quiets>
    void genBishopMoves();

    template<bool isEngine, bool quiets>
    void genQueenMoves();

    template<bool isCardinal>
    Bitboard getSlidingMoves(Square from);
};


#endif //DEEPENING1_MOVEGEN_H
