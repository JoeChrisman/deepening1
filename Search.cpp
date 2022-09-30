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
        bool playerCastleKingside = position.playerCastleKingside;
        bool playerCastleQueenside = position.playerCastleQueenside;
        bool engineCastleKingside = position.engineCastleKingside;
        bool engineCastleQueenside = position.engineCastleQueenside;
        Bitboard enPassantCapture = position.enPassantCapture;
        position.makeMove<false>(move);

        int score = max(ply + 1, maxDepth, alpha, beta);

        // unmake the move
        position.enPassantCapture = enPassantCapture;
        position.playerCastleKingside = playerCastleKingside;
        position.playerCastleQueenside = playerCastleQueenside;
        position.engineCastleKingside = engineCastleKingside;
        position.engineCastleQueenside = engineCastleQueenside;
        position.unMakeMove<false>(move);

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
        bool playerCastleKingside = position.playerCastleKingside;
        bool playerCastleQueenside = position.playerCastleQueenside;
        bool engineCastleKingside = position.engineCastleKingside;
        bool engineCastleQueenside = position.engineCastleQueenside;
        Bitboard enPassantCapture = position.enPassantCapture;
        position.makeMove<true>(move);

        int score = min(ply + 1, maxDepth, alpha, beta);

        // unmake the move
        position.enPassantCapture = enPassantCapture;
        position.playerCastleKingside = playerCastleKingside;
        position.playerCastleQueenside = playerCastleQueenside;
        position.engineCastleKingside = engineCastleKingside;
        position.engineCastleQueenside = engineCastleQueenside;
        position.unMakeMove<true>(move);

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
    startTime = std::clock() / CLOCKS_PER_SEC;
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
        if (clock() / CLOCKS_PER_SEC - startTime > MAX_ELAPSED)
        {
            // return a null move we can check for
            Move nullMove;
            nullMove.moved = NONE;
            return nullMove;
        }

        // make the move
        bool playerCastleKingside = position.playerCastleKingside;
        bool playerCastleQueenside = position.playerCastleQueenside;
        bool engineCastleKingside = position.engineCastleKingside;
        bool engineCastleQueenside = position.engineCastleQueenside;
        Bitboard enPassantCapture = position.enPassantCapture;
        position.makeMove<true>(move);

        int score = min(1, depth, MIN_EVAL, MAX_EVAL);
        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }

        // unmake the move
        position.enPassantCapture = enPassantCapture;
        position.playerCastleKingside = playerCastleKingside;
        position.playerCastleQueenside = playerCastleQueenside;
        position.engineCastleKingside = engineCastleKingside;
        position.engineCastleQueenside = engineCastleQueenside;
        position.unMakeMove<true>(move);
    }
    std::cout << "depth " << depth << " complete.\n";
    std::cout << "move = " << squares::toNotation(bestMove) << std::endl;
    std::cout << "score = " << bestScore << std::endl;
    std::cout << "elapsed = " << std::clock() / CLOCKS_PER_SEC - startTime << std::endl;
    std::cout << std::endl;
    return bestMove;
}
