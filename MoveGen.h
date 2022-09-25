//
// Created by Joe Chrisman on 9/17/22.
//

#ifndef DEEPENING1_MOVEGEN_H
#define DEEPENING1_MOVEGEN_H

#include "Position.h"

class MoveGen
{

public:
    MoveGen(Position& _position);

    Position& position;

    std::vector<Move> moveList;

    // generate quiet moves as well as captures
    void genEngineMoves();
    void genPlayerMoves();
    // generate only captures - for use in a quiescence search
    void genEngineCaptures();
    void genPlayerCaptures();


    /*
     * get a bitboard of pieces that are attacking the king.
     * used in ChessGame.cpp to highlight checking pieces
     */
    template<bool isEngine>
    Bitboard getCheckers()
    {
        Square king = toSquare(position.pieces[isEngine ? ENGINE_KING : PLAYER_KING]);
        Bitboard checkers = EMPTY_BITBOARD;

        Bitboard knights = KNIGHT_MOVES[king];
        knights &= position.pieces[isEngine ? PLAYER_KNIGHT : ENGINE_KNIGHT];

        Bitboard pawns = isEngine ? PLAYER_PAWN_CAPTURES[king] : ENGINE_PAWN_CAPTURES[king];
        pawns &= position.pieces[isEngine ? PLAYER_PAWN : ENGINE_PAWN];

        Bitboard bishops = getSlidingMoves<false>(king);
        Bitboard rooks = getSlidingMoves<true>(king);
        Bitboard queens = (bishops | rooks);

        queens &= position.pieces[isEngine ? PLAYER_QUEEN : ENGINE_QUEEN];
        bishops &= position.pieces[isEngine ? PLAYER_BISHOP : ENGINE_BISHOP];
        rooks &= position.pieces[isEngine ? PLAYER_ROOK : ENGINE_ROOK];

        return pawns | knights | bishops | rooks | queens;
    }

private:

    template<bool isEngine>
    void genPromotions(Square from, Square to, Piece captured);

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
