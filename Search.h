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

    /*
     * a vector of zobrist hashes used for detecting draw by repetition.
     * this vector can be cleared out whenever an irreversible move is made.
     * when the engine or the player makes a move in the game (not in the search),
     * we check if the move is irreversible, if it is, we can clear the repetitions vector.
     * In the search, we push and pop positions from the repetitions vector, but never clear it
     */
    std::vector<Zobrist> repetitions;
    Move getBestMove(int maxElapsed);

    /*
     * a struct that represents a position we already evaluated.
     * by remembering what we already calculated, we can prune huge subtrees.
     */
    struct Node
    {
        Zobrist hash; // to help detect hash collisions (two different zobrist keys hashing to the same index)
        Move bestMove; // to help improve move ordering by using the last best move we calculated for this node
        short depth; // to make sure we don't use the evaluation of a more shallowly searched node than the current node
        short evaluation;
        bool isLowerBound; // we could not find a move greater than alpha
        bool isUpperBound; // we found a move greater than beta
        bool isExact; // the evaluation for this node does not belong to a bound
    };

    /*
     * a hash table, indexed by position zobrist hash modulo table size, that
     * holds information about previously evaluated nodes.
     * https://www.chessprogramming.org/Transposition_Table
     */
    const int MAX_TRANSPOSITIONS = 16777213; // 2^24 - 3, the greatest prime number smaller than 2^24
    std::vector<Node> transpositions = std::vector<Node>(MAX_TRANSPOSITIONS);


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

    // the engine should be disadvantaged by 4 or more pawns in evaluation to want a draw
    const int CONTEMPT = -PIECE_SCORES[ENGINE_PAWN] * 4;

    const int MAX_DEPTH = 100;

    int nodesSearched = 0;
    int nodesEvaluated = 0;
    int transpositionHits = 0;

    /*
     * depth first negamax search with alpha beta pruning.
     * negamax is a simplified version of the minimax algorithm,
     * that takes advantage of the fact that max(a, b) = -min(-a, -b)
     * https://www.chessprogramming.org/Negamax
     * https://www.chessprogramming.org/Minimax
     * https://www.chessprogramming.org/Alpha-Beta
     */
    int negamax(int depth, int alpha, int beta);

    /*
     * iterative deepening search.
     * we search the tree depth first repeatedly, starting at depth 1
     * and increasing, until a time given time restraint has been breached
     * https://www.chessprogramming.org/Iterative_Deepening
     */
    Move iterate(int depth, int startTime, int maxElapsed);

    /*
     * swap the best move in the move list that occurs after the given index
     * with the move found at the given index. used for move ordering as an
     * alpha beta search optimization. it orders moves based on difference
     * between the score of material captured and material captured with.
     * this function returns true while there is still a move to be selected
     *
     * the sorting works like this:
     * 1) best move last time we searched this node
     * 2) winning captures (PxQ)
     * 3) losing captures (QxP)
     * 4) quiet moves
     */
    inline bool selectMove(Move previousBest, std::vector<Move>& moveList, int index)
    {
        if (index >= moveList.size())
        {
            return false;
        }
        Move move = moveList[index];
        PieceType pieceCaptured = getPieceCaptured(move);

        int bestIndex = index;
        int bestScore;
        if (move == previousBest)
        {
            bestScore = 3 * PIECE_SCORES[ENGINE_QUEEN];
        }
        else if (pieceCaptured != NONE)
        {
            // sort winning captures before losing captures (PxQ before QxP),
            // the score will always be at least the value of a pawn
            bestScore = PIECE_SCORES[ENGINE_QUEEN] +
                        PIECE_SCORES[pieceCaptured] -
                        PIECE_SCORES[getPieceMoved(move)];
        }
        else
        {
            bestScore = -1;
        }
        // find the index of the best capture after the given index
        for (int i = index + 1; i < moveList.size(); i++)
        {
            move = moveList[i];
            pieceCaptured = getPieceCaptured(move);

            int score;
            if (move == previousBest)
            {
                score = 3 * PIECE_SCORES[ENGINE_QUEEN];
            }
            else if (pieceCaptured != NONE)
            {
                score = PIECE_SCORES[ENGINE_QUEEN] +
                            PIECE_SCORES[pieceCaptured] -
                            PIECE_SCORES[getPieceMoved(move)];
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
        // we found the index of the strongest move.
        // swap the best move with the index we started with
        move = moveList[bestIndex];
        moveList[bestIndex] = moveList[index];
        moveList[index] = move;

        return true;
    }
};

#endif //DEEPENING1_SEARCH_H
