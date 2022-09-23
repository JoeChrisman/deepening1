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

    // generate quiet moves as well as captures
    void genEngineMoves();
    void genPlayerMoves();
    // generate only captures - for use in a quiescence search
    void genEngineCaptures();
    void genPlayerCaptures();

private:

    // squares where the king is safe to move to
    Bitboard safeSquares;
    template<bool isEngine>
    void updateSafeSquares();

    // squares we can move a piece to without leaving the king in check
    Bitboard resolverSquares;
    template<bool isEngine>
    void updateResolverSquares();

    // squares along all ordinal pins
    Bitboard cardinalPins;
    // squares along all cardinal pins
    Bitboard ordinalPins;
    template<bool isEngine, bool isCardinal>
    void updatePins();

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
