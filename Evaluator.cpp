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
    int score = 0;

    Bitboard enginePawns = position.pieces[ENGINE_PAWN];
    Bitboard engineKnights = position.pieces[ENGINE_KNIGHT];
    Bitboard engineBishops = position.pieces[ENGINE_BISHOP];
    while (enginePawns)
    {
        score += ENGINE_PAWN_SCORES[popFirstPiece(enginePawns)];
        score += PIECE_SCORES[ENGINE_PAWN];
    }
    while (engineKnights)
    {
        score += ENGINE_KNIGHT_SCORES[popFirstPiece(engineKnights)];
        score += PIECE_SCORES[ENGINE_KNIGHT];
    }
    while (engineBishops)
    {
        score += ENGINE_BISHOP_SCORES[popFirstPiece(engineBishops)];
        score += PIECE_SCORES[ENGINE_BISHOP];
    }

    Bitboard playerPawns = position.pieces[PLAYER_PAWN];
    Bitboard playerKnights = position.pieces[PLAYER_KNIGHT];
    Bitboard playerBishops = position.pieces[PLAYER_BISHOP];
    while (playerPawns)
    {
        score -= PLAYER_PAWN_SCORES[popFirstPiece(playerPawns)];
        score -= PIECE_SCORES[PLAYER_PAWN];
    }
    while (playerKnights)
    {
        score -= PLAYER_KNIGHT_SCORES[popFirstPiece(playerKnights)];
        score -= PIECE_SCORES[PLAYER_KNIGHT];
    }
    while (playerBishops)
    {
        score -= PLAYER_BISHOP_SCORES[popFirstPiece(playerBishops)];
        score -= PIECE_SCORES[PLAYER_BISHOP];
    }

    score += countPieces(position.pieces[ENGINE_ROOK]) * PIECE_SCORES[ENGINE_ROOK];
    score += countPieces(position.pieces[ENGINE_QUEEN]) * PIECE_SCORES[ENGINE_QUEEN];

    score -= countPieces(position.pieces[PLAYER_ROOK]) * PIECE_SCORES[PLAYER_ROOK];
    score -= countPieces(position.pieces[PLAYER_QUEEN]) * PIECE_SCORES[PLAYER_QUEEN];

    return score;
}
