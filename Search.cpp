//
// Created by Joe Chrisman on 9/17/22.
//

#include "Search.h"

Search::Search()
{
    moveGen = MoveGen();
}

Move Search::getBestMove()
{
    moveGen.genEngineMoves();
    return moveGen.moveList[rand() % (int)moveGen.moveList.size()];
}