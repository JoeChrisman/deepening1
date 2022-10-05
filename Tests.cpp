//
// Created by Joe Chrisman on 9/26/22.
//

#include "Tests.h"

Tests::Tests()
{
    tacticSuite();
}

void Tests::tacticSuite()
{
    std::cout << "* tactical suite run initialized\n";
    // fork tactics winning material
    assert(runGetBestMove(FORK_1, 500).to == E2);
    assert(runGetBestMove(FORK_2, 500).to == D3);
    assert(runGetBestMove(FORK_3, 500).to == C2);
    assert(runGetBestMove(FORK_4, 1500).to == E1);
    assert(runGetBestMove(FORK_5, 500).to == E4);

    // pin tactics winning material
    assert(runGetBestMove(PIN_1, 500).to == F5);
    assert(runGetBestMove(PIN_2, 500).to == E8);
    assert(runGetBestMove(PIN_3, 500).to == E8);
    assert(runGetBestMove(PIN_4, 500).to == G7);
    assert(runGetBestMove(PIN_5, 500).to == C6);

    // skewer tactics winning material
    assert(runGetBestMove(SKEWER_1, 500).to == A1);
    assert(runGetBestMove(SKEWER_2, 500).to == F6);
    assert(runGetBestMove(SKEWER_3, 500).to == E8);
    assert(runGetBestMove(SKEWER_4, 500).to == B6);
    assert(runGetBestMove(SKEWER_5, 1500).to == C5);

    // tactics relying on checkmates that win material
    assert(runGetBestMove(MATE_TACTIC_1, 500).to == E7);
    assert(runGetBestMove(MATE_TACTIC_2, 1000).to == G8);
    assert(runGetBestMove(MATE_TACTIC_3, 500).to == C1);
    assert(runGetBestMove(MATE_TACTIC_4, 1000).to == E8);
    assert(runGetBestMove(MATE_TACTIC_5, 500).to == G5);

    // forced checkmates
    assert(runGetBestMove(MATE_1, 500).to == B2);
    assert(runGetBestMove(MATE_2, 500).to == C2);
    assert(runGetBestMove(MATE_3, 500).to == D1);
    assert(runGetBestMove(MATE_4, 500).to == E1);
    assert(runGetBestMove(MATE_5, 500).to == G1);
    assert(runGetBestMove(MATE_6, 500).to == C3);
    assert(runGetBestMove(MATE_7, 1500).to == H2);
    assert(runGetBestMove(MATE_8, 1000).to == F3);
    assert(runGetBestMove(MATE_9, 2500).to == F3);
    assert(runGetBestMove(MATE_10, 1000).to == C2);

    std::cout << "* tactical suite run terminated.\n";
}

void Tests::perftSuite()
{
    double start = clock();
    std::cout << "* perft suite run initialized\n";
    std::cout << "* running perft test for position 1: \"" << POS_1 << "\"\n";
    assert(runPerft(1, POS_1) == 20);
    assert(runPerft(2, POS_1) == 400);
    assert(runPerft(3, POS_1) == 8902);
    assert(runPerft(4, POS_1) == 197281);
    assert(runPerft(5, POS_1) == 4865609);
    assert(runPerft(6, POS_1) == 119060324); // ~14 secs

    std::cout << "* running perft test for position 2: \"" << POS_2 << "\"\n";
    assert(runPerft(1, POS_2) == 48);
    assert(runPerft(2, POS_2) == 2039);
    assert(runPerft(3, POS_2) == 97862);
    assert(runPerft(4, POS_2) == 4085603);
    assert(runPerft(5, POS_2) == 193690690); // ~16 secs

    std::cout << "* running perft test for position 3: \"" << POS_3 << "\"\n";
    assert(runPerft(1, POS_3) == 14);
    assert(runPerft(2, POS_3) == 191);
    assert(runPerft(3, POS_3) == 2812);
    assert(runPerft(4, POS_3) == 43238);
    assert(runPerft(5, POS_3) == 674624);
    assert(runPerft(6, POS_3) == 11030083);
    assert(runPerft(7, POS_3) == 178633661); // ~18.5 secs

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
    assert(runPerft(5, POS_5) == 89941194); // ~9.5 secs

    std::cout << "* running perft test for position 6: \"" << POS_6 << "\"\n";
    assert(runPerft(1, POS_6) == 46);
    assert(runPerft(2, POS_6) == 2079);
    assert(runPerft(3, POS_6) == 89890);
    assert(runPerft(4, POS_6) == 3894594);
    assert(runPerft(5, POS_6) == 164075551); // ~13.5 secs

    // ~75 secs
    std::cout << "* perft suite run terminated.\n";
    std::cout << "* " << (clock() - start) / CLOCKS_PER_SEC << " seconds elapsed.\n";

}

Move Tests::runGetBestMove(std::string fen, int maxElapsed)
{
    std::cout << "* running tactical test for position FEN: \"" << fen << "\"\n";
    position = new Position(fen);
    search = new Search(*(position));
    moveGen = &search->moveGen;

    Move bestMove = search->getBestMove(maxElapsed);
    std::cout << "*\t move found: " << moves::toNotation(bestMove) << std::endl;
    return search->getBestMove(maxElapsed);
}

// runs a complete perft test for a given depth and position.
int Tests::runPerft(int depth, std::string fen)
{
    position = new Position(fen);
    search = new Search(*(position));
    moveGen = &search->moveGen;

    int numLeaves = 0;

    double start = std::clock();
    Zobrist hashBefore = position->hash;
    perft(depth, numLeaves);
    double elapsed = (std::clock() - start) / CLOCKS_PER_SEC;

    std::cout << "*\t depth " << depth << ":\n";
    std::cout << "*\t\t seconds elapsed ---> " << elapsed << std::endl;
    std::cout << "*\t\t leaf nodes      ---> " << numLeaves << std::endl;
    std::cout << "*\t\t hash            ---> " << position->hash << std::endl;
    assert(position->hash == hashBefore);
    return numLeaves;
}

/*
 * this is the recursive portion of the perft test.
 * it generates all legal moves from each position until a certain depth is reached.
 * when that depth is hit, we add the number of leaf nodes to a sum.
 * we can then compare the number of leaf nodes to a known value to assert complete move generation accuracy.
 * notice we do not count stalemate or checkmate nodes as leaf nodes. this is because this follows the
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
    std::vector<Move> moveList = moveGen->moveList;
    for (Move& move : moveList)
    {
        PositionRights rights = position->rights;
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
            position->unMakeMove<false>(move, rights);
        }
        else
        {
            position->unMakeMove<true>(move, rights);
        }
    }
}