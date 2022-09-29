//
// Created by Joe Chrisman on 9/28/22.
//

#ifndef MAIN_CPP_EVALUATOR_H
#define MAIN_CPP_EVALUATOR_H

#include "MoveGen.h"

const int MAX_EVAL = ~(1 << 31); // signed 32 bit positive integer limit
const int MIN_EVAL = -MAX_EVAL; // signed 32 bit negative integer limit

class Evaluator
{

public:
    Evaluator(Position& position);

private:
    Position& position;


};


#endif //MAIN_CPP_EVALUATOR_H
