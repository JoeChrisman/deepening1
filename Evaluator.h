//
// Created by Joe Chrisman on 9/28/22.
//

#ifndef MAIN_CPP_EVALUATOR_H
#define MAIN_CPP_EVALUATOR_H

#include "MoveGen.h"

const int MAX_EVAL = 32767;
const int MIN_EVAL = -32767;

const int PIECE_SCORES[13] = {
        100, // PLAYER_PAWN
        350, // PLAYER_KNIGHT
        400, // PLAYER_BISHOP
        550, // PLAYER_ROOK
        1000, // PLAYER_QUEEN
        0, // PLAYER_KING
        100, // ENGINE_PAWN
        350, // ENGINE_KNIGHT
        400, // ENGINE_BISHOP
        550, // ENGINE_ROOK
        1000, // ENGINE_QUEEN
        0, // ENGINE_KING
        0 // NONE
};

class Evaluator
{

public:
    Evaluator(Position& position);

    int evaluate();

private:
    Position& position;

    const int ENGINE_KNIGHT_SCORES[64] = {
        -10, -10, -5, -5, -5, -5, -10, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -5, 0, 15, 15, 15, 15, 0, -5,
        -5, 0, 15, 20, 20, 15, 0, -5,
        -5, 0, 15, 20, 20, 15, 0, -5,
        -5, 0, 15, 10, 10, 15, 0, -5,
        -10, 0, 0, 5, 5, 0, 0, -10,
        -10, -10, -5, -5, -5, -5, -10, -10
    };

    const int PLAYER_KNIGHT_SCORES[64] = {
        -10, -10, -5, -5, -5, -5, -10, -10,
        -10, 0, 0, 5, 5, 0, 0, -10,
        -5, 0, 15, 10, 10, 15, 0, -5,
        -5, 0, 15, 20, 20, 15, 0, -5,
        -5, 0, 15, 20, 20, 15, 0, -5,
        -5, 0, 15, 15, 15, 15, 0, -5,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, -10, -5, -5, -5, -5, -10, -10
    };

    const int ENGINE_BISHOP_SCORES[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 2, 2, 2, 2, 0, 0,
        0, 0, 15, 15, 15, 15, 0, 0,
        0, 0, 10, 10, 10, 10, 0, 0,
        0, 5, 15, 5, 5, 15, 5, 0,
        0, 5, 10, 0, 0, 10, 5, 0,
        0, 10, 0, 5, 5, 0, 10, 0,
        -5, -5, -10, -5, -5, -10, -5, -5
    };

    const int PLAYER_BISHOP_SCORES[64] = {
        -5, -5, -10, -5, -5, -10, -5, -5,
        0, 10, 0, 5, 5, 0, 10, 0,
        0, 5, 10, 0, 0, 10, 5, 0,
        0, 5, 15, 5, 5, 15, 5, 0,
        0, 0, 10, 10, 10, 10, 0, 0,
        0, 0, 15, 15, 15, 15, 0, 0,
        0, 0, 2, 2, 2, 2, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    const int ENGINE_PAWN_SCORES[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        8, 8, 15, 15, 15, 15, 8, 8,
        5, 10, 10, 15, 15, 10, 10, 5,
        5,  5, 10, 15, 15, 10,  5,  5,
        0,  0,  5, 20, 20,  5,  0,  0,
        1, -1, -10, 2, 2, -10, -1,  1,
        1, 2, 2, -20, -20, 2, 2,  1,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    const int PLAYER_PAWN_SCORES[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        1, 2, 2, -20, -20, 2, 2,  1,
        1, -1, -10, 2, 2, -10, -1,  1,
        0,  0,  5, 20, 20,  5,  0,  0,
        5,  5, 10, 15, 15, 10,  5,  5,
        5, 10, 10, 15, 15, 10, 10, 5,
        8, 8, 15, 15, 15, 15, 8, 8,
        0, 0, 0, 0, 0, 0, 0, 0,
    };


};


#endif //MAIN_CPP_EVALUATOR_H
