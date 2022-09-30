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
    moveGen.genEngineMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if the engine is in checkmate or stalemate
    if (moveList.empty())
    {
        assert(false);
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
    int moveIndex = 0;
    while (selectMove(moveList, moveIndex))
    {
        Move move = moveList[moveIndex++];
        // make the move
        bool playerCastleKingside = position.playerCastleKingside;
        bool playerCastleQueenside = position.playerCastleQueenside;
        bool engineCastleKingside = position.engineCastleKingside;
        bool engineCastleQueenside = position.engineCastleQueenside;
        Bitboard enPassantCapture = position.enPassantCapture;
        position.makeMove<true>(move);

        int score = min(1, 5, MIN_EVAL, MAX_EVAL);
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