//
// Created by Joe Chrisman on 9/17/22.
//

#ifndef DEEPENING1_SEARCH_H
#define DEEPENING1_SEARCH_H

#include "Evaluator.h"

class Search {
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
    int min(int depth, int alpha, int beta);
    int max(int depth, int alpha, int beta);

};


#endif //DEEPENING1_SEARCH_H
