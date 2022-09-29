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


/*
 * when we minimise, we are making moves for the player.
 * when the player is winning, we have a negative evaluation.
 * so find the move with the lowest evaluation
 */
int Search::min(int depth, int alpha, int beta)
{
    moveGen.genPlayerMoves();
    std::vector<Move> moveList = moveGen.moveList;

    // if there are no legal moves, it is either checkmate or stalemate
    if (moveList.empty())
    {
        // just assume stalemate for now
        return 0;
    }

    if (depth == 0)
    {
        return position.materialScore;
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

        int score = max(depth - 1, alpha, beta);

        // unmake the move
        position.enPassantCapture = enPassantCapture;
        position.playerCastleKingside = playerCastleKingside;
        position.playerCastleQueenside = playerCastleQueenside;
        position.engineCastleKingside = engineCastleKingside;
        position.engineCastleQueenside = engineCastleQueenside;
        position.unMakeMove<false>(move);

        if (score <= alpha)
        {
            return alpha;
        }
        if (score < beta)
        {
            beta = score;
        }
        if (score < bestScore)
        {
            bestScore = score;
        }
    }
    return beta;
}

/*
 * when we maximise, we are making moves for the engine.
 * when the engine is winning, we have a positive evaluation.
 * so find the move with the highest evaluation
 */
int Search::max(int depth, int alpha, int beta)
{
    moveGen.genEngineMoves();
    std::vector<Move> moveList = moveGen.moveList;

    // if there are no legal moves, it is either checkmate or stalemate
    if (moveList.empty())
    {
        // just assume stalemate for now
        return 0;
    }
    if (depth == 0)
    {
        return position.materialScore;
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

        int score = min(depth - 1, alpha, beta);

        // unmake the move
        position.enPassantCapture = enPassantCapture;
        position.playerCastleKingside = playerCastleKingside;
        position.playerCastleQueenside = playerCastleQueenside;
        position.engineCastleKingside = engineCastleKingside;
        position.engineCastleQueenside = engineCastleQueenside;
        position.unMakeMove<true>(move);

        if (score >= beta)
        {
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
        }
        if (score > bestScore)
        {
            bestScore = score;
        }
    }
    return alpha;

}

Move Search::getBestMove()
{
    moveGen.genEngineMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if the engine is in checkmate or stalemate
    if (moveList.empty())
    {
        // return an empty move
        return Move{
            NORMAL,
            NULL_SQUARE,
            NULL_SQUARE,
            NONE,
            NONE
        };
    }

    Move bestMove{};
    int bestScore = MIN_EVAL;
    for (Move& move : moveList)
    {
        // make the move
        bool playerCastleKingside = position.playerCastleKingside;
        bool playerCastleQueenside = position.playerCastleQueenside;
        bool engineCastleKingside = position.engineCastleKingside;
        bool engineCastleQueenside = position.engineCastleQueenside;
        Bitboard enPassantCapture = position.enPassantCapture;
        position.makeMove<true>(move);

        int score = min(4, MIN_EVAL, MAX_EVAL);
        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }

        std::cout << squares::toNotation(move) << ": " << score << std::endl;

        // unmake the move
        position.enPassantCapture = enPassantCapture;
        position.playerCastleKingside = playerCastleKingside;
        position.playerCastleQueenside = playerCastleQueenside;
        position.engineCastleKingside = engineCastleKingside;
        position.engineCastleQueenside = engineCastleQueenside;
        position.unMakeMove<true>(move);
    }
    return bestMove;
}