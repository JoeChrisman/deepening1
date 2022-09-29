//
// Created by Joe Chrisman on 9/17/22.
//

#ifndef DEEPENING1_SEARCH_H
#define DEEPENING1_SEARCH_H

#include "Evaluator.h"
#include <iostream>

class Search
{
public:

    Search(Position& position);

    MoveGen moveGen;
    Position& position;

    Move getBestMove();

private:
    Evaluator evaluator;

    /*
     * depth first search using minimax algorithm with alpha beta pruning
     * https://www.chessprogramming.org/Minimax
     * https://www.chessprogramming.org/Alpha-Beta
     */
    int min(int ply, int maxDepth, int alpha, int beta);
    int max(int ply, int maxDepth, int alpha, int beta);

    /*
     * swap the best move in the move list that occurs after the given index
     * with the move found at the given index. used for move ordering as an
     * alpha beta search optimization. it orders moves based on difference
     * between the score of material captured and material captured with.
     * this function returns true while there is still a move to be selected
     *
     * the sorting works like this:
     * 1) winning captures (PxQ)
     * 2) losing captures (QxP)
     * 3) quiet moves
     */
    inline bool selectMove(std::vector<Move>& moveList, int index)
    {
        if (index >= moveList.size())
        {
            return false;
        }
        Move move = moveList[index];
        int bestIndex = index;
        int bestScore;
        if (move.captured != NONE)
        {
            // sort winning captures before losing captures (PxQ before QxP),
            // the score will always be at least the value of a pawn
            bestScore = PIECE_SCORES[ENGINE_QUEEN] +
                        PIECE_SCORES[move.captured] -
                        PIECE_SCORES[move.moved];
        }
        else
        {
            bestScore = -1;
        }


        // find the index of the best capture after the given index
        for (int i = index + 1; i < moveList.size(); i++)
        {
            move = moveList[i];

            int score;
            if (move.captured != NONE)
            {
                score = PIECE_SCORES[ENGINE_QUEEN] +
                            PIECE_SCORES[move.captured] -
                            PIECE_SCORES[move.moved];
            }
            else
            {
                score = -1;
            }

            if (score > bestScore)
            {
                bestScore = score;
                bestIndex = i;
            }
        }
        // we found the index of the strongest capture.
        // swap the best capture with the index we started with
        move = moveList[bestIndex];
        moveList[bestIndex] = moveList[index];
        moveList[index] = move;

        return true;
    }

};


#endif //DEEPENING1_SEARCH_H
