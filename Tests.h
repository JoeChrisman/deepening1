//
// Created by Joe Chrisman on 9/26/22.
//

#ifndef DEEPENING1_TESTS_H
#define DEEPENING1_TESTS_H

#include <iostream>
#include <ctime>
#include "Search.h"

class Tests
{
public:
    Tests();

    // test suite for move generation testing
    void perftSuite();

private:
    Position* position;
    Search* search;
    MoveGen* moveGen;

    /*
     * all these positions are specially designed to find move generation bugs.
     * notice they all are from white's perspective. because of this, ENGINE_IS_WHITE
     * in Constants.h must be set to false for the tests to be accurate.
     * these test positions can be found here:
     * https://www.chessprogramming.org/Perft_Results
     */
    // initial position
    const std::string POS_1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // Kiwipete by Peter McKenzie
    const std::string POS_2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    // catches horizontal en passant pin
    const std::string POS_3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    // catches diagonal en passant pin
    const std::string POS_4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    // catches castling issues
    const std::string POS_5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    // complex position
    const std::string POS_6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

    /*
     * count the number of leaf positions that arise after a given depth for a given position.
     * it is important that the position given must be from the player's perspective,
     * meaning the color of the player must be opposite of the ENGINE_IS_WHITE constant.
     * ENGINE_IS_WHITE must be false for the perft test positions
     */
    int runPerft(int depth, const std::string& fen);
    // recursively find the number of leaf positions that exist at a given depth
    void perft(int depth, int& numLeaves);

};


#endif //DEEPENING1_TESTS_H