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
 * search the current position recursively depth first, using the
 * negamax algorithm with alpha beta pruning and a transposition table
 */
int Search::negamax(int depth, int alpha, int beta)
{
    nodesSearched++;
    bool isEngineMove = position.isEngineMove;

    /*
     * if the position is a draw by threefold repetition, or by the fifty move rule,
     * return the contempt value immediately. We do this before probing the transposition
     * table because we want to make sure it is not possible for a drawn position to end
     * up in the table, because the zobrist hash does not know about draw by threefold
     * repetition or by the fifty move rule. The zobrist hash knows nothing about the position's
     * history other than the en passant square and castling rights. Accidentally adding
     * these types of positions to the hash table may result in search instability.
     * https://www.chessprogramming.org/Search_Instability
     */
    if (repeated() || position.rights.halfMoveClock >= 50)
    {
        /*
         * the contempt value is equal to the amount of evaluation the engine
         * must have in order to want a draw, and the amount of evaluation the
         * player must have in order to not want a draw.
         */
        return -CONTEMPT;
    }

    // look up the current position in the transposition table
    // https://www.chessprogramming.org/Transposition_Table
    Node& currentNode = transpositions[int(position.hash % MAX_TRANSPOSITIONS)];
    /*
     * we always want to overwrite the current node with new information.
     * it does not matter if it is empty or occupied, index collisions are common,
     * and the easiest way to fix them is to simply always overwrite the node
     */
    if (currentNode.hash != position.hash)
    {
        /*
         * overwrite the transposition node with blank information, prepare it to be filled.
         * We might be overwriting a valid node because of an index collision,
         * but that is okay because this program uses the "always replace" scheme
         * https://www.chessprogramming.org/Transposition_Table#Collisions
         * https://www.chessprogramming.org/Transposition_Table#Always_Replace
         */
        currentNode.hash = position.hash;
        currentNode.bestMove = NULL_MOVE;
        currentNode.isLowerBound = false;
        currentNode.isUpperBound = false;
        currentNode.isExact = false;
        currentNode.depth = (short)depth;
        currentNode.evaluation = 0;
    }
    /*
     * We can only use what we learned about the position if the depth
     * it was searched to last time is greater than or equal to the current
     * depth we are searching to. If we have already searched this node to the
     * required depth, we can use the evaluation we calculated last time, which
     * has a good chance of pruning large branches off of the search graph
     */
    else if (currentNode.depth >= depth)
    {
        transpositionHits++;
        // if this node is a PV node or has been statically evaluated
        if (currentNode.isExact)
        {
            // we don't need to search it again
            return currentNode.evaluation;
        }
        // if the best move we found last time was lower than the old alpha,
        // but is greater than the current alpha
        else if (currentNode.isLowerBound && currentNode.evaluation > alpha)
        {
            // increase the lower bound
            alpha = currentNode.evaluation;
        }
        // if the best move we found last time was higher than the old beta,
        // but is less than the current beta
        else if (currentNode.isUpperBound && currentNode.evaluation < beta)
        {
            // decrease the upper bound
            beta = currentNode.evaluation;
        }
        // if what we learned last time caused the window to collapse
        if (alpha >= beta)
        {
            // return the exact bound we calculated last time
            return currentNode.evaluation;
        }
    }

    // if the current node is a leaf node
    if (!depth)
    {
        // if the current node has not already been statically evaluated
        if (!currentNode.isExact)
        {
            nodesEvaluated++;
            // statically evaluate the position and save it to the transposition table
            currentNode.isExact = true;
            currentNode.evaluation = (short)(isEngineMove ? evaluator.evaluate() : -evaluator.evaluate());
        }
        // depth is zero, so no more searching
        return currentNode.evaluation;
    }

    isEngineMove ? moveGen.genEngineMoves() : moveGen.genPlayerMoves();
    std::vector<Move> moveList = moveGen.moveList;
    /*
     * if there are no legal moves, it is either checkmate or stalemate.
     * Remember to save this evaluation to the transposition table. It won't
     * save very much search effort, because this node is a leaf node, but
     * we might as well let the table fill up as much as we can.
     */
    if (moveList.empty())
    {
        // if the king is safe
        if (position.pieces[isEngineMove ? ENGINE_KING : PLAYER_KING] & moveGen.safeSquares)
        {
            // stalemate
            currentNode.evaluation = (short)-CONTEMPT;
        }
        else
        {
            // checkmate
            currentNode.evaluation = (short)(MIN_EVAL + MAX_DEPTH - depth);
        }
        return currentNode.evaluation;
    }
    /*
     * remember the best move we find, so we can add it to this node in the transposition table.
     * the next time we arrive at this position, even if it has not been searched to the required
     * depth, we can still use the best move we learned about last time to improve move ordering
     */
    Move bestMove = NULL_MOVE;
    int bestScore = MIN_EVAL;
    // select each move we generated according to a predefined order based on heuristics about chess
    int moveIndex = 0;
    while (selectMove(currentNode.bestMove, moveList, moveIndex))
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
        int score = -negamax(depth - 1, -beta, -alpha);
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

        // if we found a better score than the best one so far
        if (score > bestScore)
        {
            // remember the best score
            bestScore = score;
            // remember the best move
            bestMove = move;
        }
        // if we found a move greater than our lower bound
        if (score > alpha)
        {
            // update the lower bound for future searches
            alpha = score;
        }
        // if our lower bound exceeded our upper bound
        if (alpha >= beta)
        {
            // no need to search any more moves
            break;
        }
    }
    // figure out the node type to save in the transposition table.
    // later we can use the node type to restrict the search window, pruning the tree
    if (bestScore <= alpha)
    {
        currentNode.isUpperBound = true;
        currentNode.bestMove = bestMove;
    }
    else if (bestScore >= beta)
    {
        currentNode.isLowerBound = true;
    }
    else
    {
        currentNode.isExact = true;
        currentNode.bestMove = bestMove;
    }
    currentNode.evaluation = (short)bestScore;
    return bestScore;
}

Move Search::getBestMove(int maxElapsed)
{
    int depthSearched = 0;
    Move bestMove;
    int startTime = std::clock() * 1000 / CLOCKS_PER_SEC;
    // while we still have time to search
    for (int depth = 1; depth <= MAX_DEPTH; depth++)
    {
        Move move = iterate(depth, startTime, maxElapsed);

        // if we ran out of time
        if (move == NULL_MOVE)
        {
            break;
        }
        depthSearched = depth;
        bestMove = move;
    }
    if (moves::isIrreversible(bestMove))
    {
        // clear the repetitions list. we can never get the old position again
        repetitions.clear();
    }

    int endTime = std::clock() * 1000 / CLOCKS_PER_SEC;
    std::cout << "depth = " << depthSearched << std::endl;
    std::cout << "ms elapsed = " << endTime - startTime << std::endl;
    std::cout << "nodes searched = " << nodesSearched << std::endl;
    std::cout << "nodes evaluated = " << nodesEvaluated << std::endl;
    std::cout << "transposition hits = " << transpositionHits << std::endl;
    std::cout << std::endl;
    return bestMove;
}

Move Search::iterate(int depth, int startTime, int maxElapsed)
{
    moveGen.genEngineMoves();
    std::vector<Move> moveList = moveGen.moveList;
    // if the engine is in checkmate or stalemate
    if (moveList.empty())
    {
        // deal with this stuff later
        assert(false);
    }
    int moveIndex = 0;
    int bestScore = MIN_EVAL;
    Move bestMove = NULL_MOVE;
    while (selectMove(NULL_MOVE, moveList, moveIndex))
    {
        Move move = moveList[moveIndex++];
        // if we ran out of time during iterative deepening
        if (std::clock() * 1000 / CLOCKS_PER_SEC - startTime > maxElapsed)
        {
            return NULL_MOVE;
        }

        // make the move
        PositionRights rights = position.rights;
        position.makeMove<true>(move);

        repetitions.push_back(position.hash);
        int score = -negamax(depth, MIN_EVAL, MAX_EVAL);
        repetitions.pop_back();

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }

        // unmake the move
        position.unMakeMove<true>(move, rights);
    }
    nodesSearched = 0;
    nodesEvaluated = 0;
    transpositionHits = 0;
    return bestMove;
}
