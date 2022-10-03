//
// Created by Joe Chrisman on 9/17/22.
//

#include "Search.h"

Search::Search(Position& position) :
moveGen(position),
position(position),
evaluator(position)
{
    startTime = 0;
}

/*
 * when we minimise, we are making moves for the player.
 * when the player is winning, we have a negative evaluation.
 * so find the move with the lowest evaluation
 */
int Search::min(int ply, int maxDepth, int alpha, int beta)
{
    if (repeated())
    {
        return CONTEMPT;
    }
    if (ply > maxDepth)
    {
        return evaluator.evaluate();
    }

    moveGen.genPlayerMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if there are no legal moves, it is either checkmate or stalemate
    if (moveList.empty())
    {
        // if the king is safe
        if (position.pieces[PLAYER_KING] & moveGen.safeSquares)
        {
            // stalemate
            return 0;
        }
        // checkmate
        return MAX_EVAL - ply;
    }

    int bestScore = MAX_EVAL;
    int moveIndex = 0;
    while (selectMove(moveList, moveIndex))
    {
        Move& move = moveList[moveIndex++];

        // make the move
        PositionRights rights = position.rights;
        position.makeMove<false>(move);

        repetitions.push_back(position.hash);
        int score = max(ply + 1, maxDepth, alpha, beta);
        repetitions.pop_back();

        // unmake the move
        position.unMakeMove<false>(move, rights);

        if (score < bestScore)
        {
            bestScore = score;
        }
        if (bestScore < beta)
        {
            beta = bestScore;
        }
        if (beta <= alpha)
        {
            break;
        }
    }
    return bestScore;
}

/*
 * when we maximise, we are making moves for the engine.
 * when the engine is winning, we have a positive evaluation.
 * so find the move with the highest evaluation
 */
int Search::max(int ply, int maxDepth, int alpha, int beta)
{
    if (repeated())
    {
        return CONTEMPT;
    }
    if (ply > maxDepth)
    {
        return evaluator.evaluate();
    }

    moveGen.genEngineMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if there are no legal moves, it is either checkmate or stalemate
    if (moveList.empty())
    {
        // if the king is safe
        if (position.pieces[ENGINE_KING] & moveGen.safeSquares)
        {
            // stalemate
            return 0;
        }
        // checkmate
        return MIN_EVAL + ply;
    }

    int bestScore = MIN_EVAL;
    int moveIndex = 0;
    while (selectMove(moveList, moveIndex))
    {
        Move& move = moveList[moveIndex++];
        // make the move
        PositionRights rights = position.rights;
        position.makeMove<true>(move);

        repetitions.push_back(position.hash);
        int score = min(ply + 1, maxDepth, alpha, beta);
        repetitions.pop_back();

        // unmake the move
        position.unMakeMove<true>(move, rights);

        if (score > bestScore)
        {
            bestScore = score;
        }
        if (bestScore > alpha)
        {
            alpha = bestScore;
        }
        if (beta <= alpha)
        {
            break;
        }
    }
    return bestScore;
}

Move Search::getBestMove()
{
    Move bestMove{};
    startTime = std::clock() * 1000 / CLOCKS_PER_SEC;
    // while we still have time to search
    for (int depth = 0; depth <= 100; depth++)
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
            return nullMove;
        }

        // make the move
        PositionRights rights = position.rights;
        position.makeMove<true>(move);

        repetitions.push_back(position.hash);
        int score = min(1, depth, MIN_EVAL, MAX_EVAL);
        repetitions.pop_back();

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }

        // unmake the move
        position.unMakeMove<true>(move, rights);
    }
    std::cout << "depth " << depth << " complete.";
    std::cout << "\nmove = " << moves::toNotation(bestMove);
    std::cout << "\nscore = " << bestScore;
    std::cout << "\nelapsed = " << double(std::clock() * 1000 / CLOCKS_PER_SEC - startTime) / 1000;
    std::cout << "\n\n";
    return bestMove;
}
