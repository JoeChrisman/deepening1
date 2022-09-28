//
// Created by Joe Chrisman on 9/26/22.
//

#include "Tests.h"

Tests::Tests()
{
    perftSuite();
}

void Tests::perftSuite()
{
    double start = clock();
    std::cout << "* perft suite run initialized" << std::endl;
    std::cout << "* running perft test for position 1: \"" << POS_1 << "\"\n";
    assert(runPerft(1, POS_1) == 20);
    assert(runPerft(2, POS_1) == 400);
    assert(runPerft(3, POS_1) == 8902);
    assert(runPerft(4, POS_1) == 197281);
    assert(runPerft(5, POS_1) == 4865609);
    assert(runPerft(6, POS_1) == 119060324); // ~11.5 secs


    std::cout << "* running perft test for position 2: \"" << POS_2 << "\"\n";
    assert(runPerft(1, POS_2) == 48);
    assert(runPerft(2, POS_2) == 2039);
    assert(runPerft(3, POS_2) == 97862);
    assert(runPerft(4, POS_2) == 4085603);
    assert(runPerft(5, POS_2) == 193690690); // ~17.5 secs

    std::cout << "* running perft test for position 3: \"" << POS_3 << "\"\n";
    assert(runPerft(1, POS_3) == 14);
    assert(runPerft(2, POS_3) == 191);
    assert(runPerft(3, POS_3) == 2812);
    assert(runPerft(4, POS_3) == 43238);
    assert(runPerft(5, POS_3) == 674624);
    assert(runPerft(6, POS_3) == 11030083);
    assert(runPerft(7, POS_3) == 178633661); // ~19.5 secs

    std::cout << "* running perft test for position 4: \"" << POS_4 << "\"\n";
    assert(runPerft(1, POS_4) == 6);
    assert(runPerft(2, POS_4) == 264);
    assert(runPerft(3, POS_4) == 9467);
    assert(runPerft(4, POS_4) == 422333);
    assert(runPerft(5, POS_4) == 15833292); // ~1.5 secs

    std::cout << "* running perft test for position 5: \"" << POS_5 << "\"\n";
    assert(runPerft(1, POS_5) == 44);
    assert(runPerft(2, POS_5) == 1486);
    assert(runPerft(3, POS_5) == 62379);
    assert(runPerft(4, POS_5) == 2103487);
    assert(runPerft(5, POS_5) == 89941194); // ~8.5 secs

    std::cout << "* running perft test for position 6: \"" << POS_6 << "\"\n";
    assert(runPerft(1, POS_6) == 46);
    assert(runPerft(2, POS_6) == 2079);
    assert(runPerft(3, POS_6) == 89890);
    assert(runPerft(4, POS_6) == 3894594);
    assert(runPerft(5, POS_6) == 164075551); // ~15.5 secs

    // ~75 secs
    std::cout << "* perft suite run terminated." << std::endl;
    std::cout << "* " << (clock() - start) / CLOCKS_PER_SEC << " seconds elapsed." << std::endl;

}

// this function will run a complete perft test for a given depth and position.
int Tests::runPerft(int depth, const std::string& fen)
{
    position = new Position(fen);
    search = new Search(*(position));
    moveGen = &search->moveGen;

    int numLeaves = 0;

    double start = std::clock();
    perft(depth, numLeaves);
    double elapsed = (std::clock() - start) / CLOCKS_PER_SEC;

    std::cout << "*\t depth " << depth << ":\n";
    std::cout << "*\t\t seconds elapsed ---> " << elapsed << std::endl;
    std::cout << "*\t\t leaf nodes      ---> " << numLeaves << std::endl;

    return numLeaves;
}

/*
 * this is the recursive portion of the perft test.
 * it generates all legal moves from each position until a certain depth is reached.
 * when that depth is hit, we add the number of leaf nodes to a sum.
 * we can then compare the number of leaf nodes to a known value to assert complete move generation accuracy.
 * notice, we do not count stalemate or checkmate nodes as leaf nodes. this is because this follows the
 * definition of what a perft test is.
 * https://www.chessprogramming.org/Perft
 */
void Tests::perft(int depth, int& numLeafNodes)
{
    if (position->isEngineMove)
    {
        moveGen->genEngineMoves();
    }
    else
    {
        moveGen->genPlayerMoves();
    }

    // if we are one depth higher than the leaf node depth
    if (depth == 1)
    {
        // add to the total number of leaf nodes
        numLeafNodes += moveGen->moveList.size();
        return;
    }

    std::vector<Move> copy = moveGen->moveList;
    for (Move& move : copy)
    {
        bool playerCastleKingside = position->playerCastleKingside;
        bool playerCastleQueenside = position->playerCastleQueenside;
        bool engineCastleKingside = position->engineCastleKingside;
        bool engineCastleQueenside = position->engineCastleQueenside;
        Bitboard enPassantCapture = position->enPassantCapture;

        // make move
        if (position->isEngineMove)
        {
            position->makeMove<true>(move);
        }
        else
        {
            position->makeMove<false>(move);
        }
        perft(depth - 1, numLeafNodes);

        // unmake move
        if (position->isEngineMove)
        {
            position->unMakeMove<false>(move);
        }
        else
        {
            position->unMakeMove<true>(move);
        }
        position->enPassantCapture = enPassantCapture;
        position->playerCastleKingside = playerCastleKingside;
        position->playerCastleQueenside = playerCastleQueenside;
        position->engineCastleKingside = engineCastleKingside;
        position->engineCastleQueenside = engineCastleQueenside;
    }
}