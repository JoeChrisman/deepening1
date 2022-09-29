//
// Created by Joe Chrisman on 9/28/22.
//

#include "Evaluator.h"

Evaluator::Evaluator(Position& position) :
position(position)
{
}

int Evaluator::evaluate()
{
    int score = position.materialScore;

    // if the engine has the bishop pair, that is good
    if (countPieces(position.pieces[ENGINE_BISHOP]) > 1)
    {
        score += PIECE_SCORES[PLAYER_PAWN] * 1.5;
    }
    // if the engine has the bishop pair, that is bad
    if (countPieces(position.pieces[PLAYER_BISHOP] > 1))
    {
        score -= PIECE_SCORES[ENGINE_PAWN] * 1.5;
    }

    // add positional advantages for piece placement
    Bitboard enginePawns = position.pieces[ENGINE_PAWN];
    Bitboard engineKnights = position.pieces[ENGINE_KNIGHT];
    Bitboard engineBishops = position.pieces[ENGINE_BISHOP];

    while (enginePawns)
    {
        score += ENGINE_PAWN_SCORES[popFirstPiece(enginePawns)];
    }
    while (engineKnights)
    {
        score += ENGINE_KNIGHT_SCORES[popFirstPiece(engineKnights)];
    }
    while (engineBishops)
    {
        score += ENGINE_BISHOP_SCORES[popFirstPiece(engineBishops)];
    }

    Bitboard playerPawns = position.pieces[PLAYER_PAWN];
    Bitboard playerKnights = position.pieces[PLAYER_KNIGHT];
    Bitboard playerBishops = position.pieces[PLAYER_BISHOP];

    while (playerPawns)
    {
        score -= PLAYER_PAWN_SCORES[popFirstPiece(playerPawns)];
    }
    while (playerKnights)
    {
        score -= PLAYER_KNIGHT_SCORES[popFirstPiece(playerKnights)];
    }
    while (playerBishops)
    {
        score -= PLAYER_BISHOP_SCORES[popFirstPiece(playerBishops)];
    }
    return score;
}
