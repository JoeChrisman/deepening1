//
// Created by Joe Chrisman on 9/17/22.
//

#ifndef DEEPENING1_SEARCH_H
#define DEEPENING1_SEARCH_H

#include "MoveGen.h"

class Search {
public:
    Search(Position& _position);

    MoveGen moveGen;
    Position& position;

    Move getBestMove();

private:
};


#endif //DEEPENING1_SEARCH_H
