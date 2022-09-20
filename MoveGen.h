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

    Bitboard empties; // empty squares
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

    template<bool isEngine, bool quiets>
    void genPawnMoves();

    template<bool isEngine, bool quiets>
    void genKnightMoves();

    template<bool isEngine, bool quiets>
    void genKingMoves();

    // get an attack set for a sliding rook given a bitboard of blocking pieces
    // and a boolean deciding whether to include the first blocker we encounter
    Bitboard getCardinalAttacks(Square from, Bitboard blockers, bool captures);
    // get an attack set for a sliding bishop given a bitboard of blocking pieces
    // and a boolean deciding whether to include the first blocker we encounter
    Bitboard getOrdinalAttacks(Square from, Bitboard blockers, bool captures);



};


#endif //DEEPENING1_MOVEGEN_H
