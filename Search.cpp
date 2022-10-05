//
// Created by Joe Chrisman on 9/17/22.
//

#include "Search.h"

Search::Search(Position& position) :
moveGen(position),
position(position),
evaluator(position)
{
}


int Search::negamax(int ply, int goalPly, int alpha, int beta)
{
    nodesSearched++;

    bool isEngineMove = position.isEngineMove;
    if (repeated() || position.rights.halfMoveClock >= 50)
    {
        return isEngineMove ? CONTEMPT : -CONTEMPT;
    }
    if (ply >= goalPly)
    {
        return isEngineMove ? evaluator.evaluate() : -evaluator.evaluate();
    }
    isEngineMove ? moveGen.genEngineMoves() : moveGen.genPlayerMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if there are no legal moves, it is either checkmate or stalemate
    if (moveList.empty())
    {
        // if the king is safe
        if (position.pieces[isEngineMove ? ENGINE_KING : PLAYER_KING] & moveGen.safeSquares)
        {
            // stalemate
            return isEngineMove ? CONTEMPT : -CONTEMPT;;
        }
        // checkmate
        return isEngineMove ? MIN_EVAL + ply : MAX_EVAL - ply;
    }

    int bestScore = MIN_EVAL;
    int moveIndex = 0;
    while (selectMove(moveList, moveIndex))
    {
        Move& move = moveList[moveIndex++];

        // make the move
        PositionRights rights = position.rights;
        if (isEngineMove)
        {
            position.makeMove<true>(move);
        }
        else
        {
            position.makeMove<false>(move);
        }

        repetitions.push_back(position.hash);
        int score = -negamax(ply + 1, goalPly, -beta, -alpha);
        repetitions.pop_back();

        // unmake the move
        if (isEngineMove)
        {
            position.unMakeMove<true>(move, rights);
        }
        else
        {
            position.unMakeMove<false>(move, rights);
        }

        if (score > bestScore)
        {
            bestScore = score;
        }
        if (bestScore > alpha)
        {
            alpha = bestScore;
        }
        if (alpha >= beta)
        {
            return alpha;
        }
    }
    return bestScore;
}

Move Search::getBestMove()
{
    Move bestMove{};
    startTime = std::clock() * 1000 / CLOCKS_PER_SEC;
    // while we still have time to search
    for (int depth = 0; depth <= MAX_DEPTH; depth++)
    {
        Move move = iterate(depth);
        // if we ran out of time
        if (move.moved == NONE)
        {
            break;
        }
        bestMove = move;
    }
    return bestMove;
}

Move Search::iterate(int depth)
{
    nodesSearched = 0;
    std::cout << "initializing depth " << depth << " search.";
    moveGen.genEngineMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if the engine is in checkmate or stalemate
    if (moveList.empty())
    {
        // deal with this stuff later
        assert(false);
    }
    Move bestMove{};
    int moveIndex = 0;
    int bestScore = MIN_EVAL;
    while (selectMove(moveList, moveIndex))
    {
        Move move = moveList[moveIndex++];
        // if we ran out of time during iterative deepening
        if (clock() * 1000 / CLOCKS_PER_SEC - startTime > MAX_ELAPSED)
        {
            // return a null move we can check for
            Move nullMove;
            nullMove.moved = NONE;
            std::cout << "\ndepth " << depth << " ran out of time.\n";
            return nullMove;
        }

        // make the move
        PositionRights rights = position.rights;
        position.makeMove<true>(move);

        repetitions.push_back(position.hash);
        int score = -negamax(0, depth, MIN_EVAL, MAX_EVAL);

        repetitions.pop_back();

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }

        // unmake the move
        position.unMakeMove<true>(move, rights);
    }
    std::cout << "\ndepth " << depth << " complete.";
    std::cout << "\nmove = " << moves::toNotation(bestMove);
    std::cout << "\nscore = " << bestScore;
    std::cout << "\nnodes = " << nodesSearched;
    std::cout << "\nelapsed = " << double(std::clock() * 1000 / CLOCKS_PER_SEC - startTime) / 1000;
    std::cout << "\n\n";
    return bestMove;
}
