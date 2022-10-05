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

    // some extra debug info
    int nodesSearched;

    MoveGen moveGen;
    Position& position;

    /*
     * a vector of zobrist hashes used for detecting draw by repetition.
     * this vector can be cleared out whenever an irreversible move is made.
     * when the engine or the player makes a move in the GUI (not in the search),
     * we check if the move is irreversible, if it is, we can clear the repetitions vector.
     * In the search, we push and pop positions from the repetitions vector, but never clear it
     */
    std::vector<Zobrist> repetitions;
    Move getBestMove();

    // return true if we repeated a position three times
    inline bool repeated()
    {
        if (!repetitions.empty())
        {
            // start at one, because we already have one instance of the current position
            int numRepetitions = 1;
            Zobrist repetition = repetitions[repetitions.size() - 1];
            for (int i = 0; i < repetitions.size() - 1; i++)
            {
                if (repetitions[i] == repetition)
                {
                    if (++numRepetitions >= 3)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:

    Evaluator evaluator;

    // keep track of how many milliseconds we spend doing an iterative deepening search.
    const int MAX_ELAPSED = 5000;
    int startTime;

    // the engine should be disadvantaged by 4 or more pawns in evaluation to want a draw
    const int CONTEMPT = -PIECE_SCORES[ENGINE_PAWN] * 4;

    const int MAX_DEPTH = 100;

    // a vector containing the best line during the previous iterative deepening search.
    // this vector should be cleared when initializing iterative deepening for a new position
    std::vector<Move> principalVariation;

    /*
     * depth first negamax search with alpha beta pruning.
     * negamax is a simplified version of the minimax algorithm,
     * that takes advantage of the fact that max(a, b) = -min(-a, -b)
     * we play all moves for the engine, and then play all moves for the player,
     * all the way until ply reaches the given goalPly.
     * along the way, we keep track of a lower bound (alpha), and an upper bound (beta).
     * we can use these bounds to prune large parts of the search tree, because we
     * don't have to look for stronger refutations than a refutation we already found.
     * https://www.chessprogramming.org/Negamax
     * https://www.chessprogramming.org/Minimax
     * https://www.chessprogramming.org/Alpha-Beta
     */
    int negamax(int ply, int goalPly, int alpha, int beta);

    /*
     * iterative deepening search.
     * we search the tree depth first repeatedly, starting at depth 0
     * and increasing, until a time given time restraint has been breached
     * https://www.chessprogramming.org/Iterative_Deepening
     * so far, there are no move ordering techniques implemented here, so this method
     * will not provide an efficiency increase. later, we will add a transposition
     * table so we can actually benefit from researching the top of the tree
     */
    Move iterate(int depth);

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
