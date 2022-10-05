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
    // test suite with basic tactical puzzles for the engine to solve
    void tacticSuite();

private:
    Position* position;
    Search* search;
    MoveGen* moveGen;

    Move runGetBestMove(std::string fen, int maxElapsed);

    /*
     * here are several positions used to test the engine's tactical awareness.
     * In all of these positions, it needs to be the engine's turn, and
     * these positions are all set to be black's turn to move. therefore,
     * ENGINE_IS_WHITE in Constants.h must be false for these tests to work
     */
    // a pawn fork winning a queen
    const std::string FORK_3 = "4rrk1/1P3ppp/3b4/8/1n6/2p5/PP4B1/1K1QR3 b - - 0 1";
    // a knight fork winning a queen
    const std::string FORK_1 = "1k6/ppp3q1/8/1R6/3n4/5N2/5PPP/2Q3K1 b - - 0 1";
    // a bishop fork winning a queen
    const std::string FORK_2 = "2r3k1/5ppp/b2r4/2P2N2/4Q3/7B/PP6/1K6 b - - 0 1";
    // a rook fork winning a queen
    const std::string FORK_4 = "1k2r3/pbp5/1pRN4/b7/8/8/5PPP/1Q4K1 b - - 0 1";
    // a queen fork winning a rook
    const std::string FORK_5 = "2r1q3/5ppp/5k2/8/8/4N1P1/4BPKP/1R6 b - - 0 1";

    // a bishop pin winning a queen
    const std::string PIN_1 = "1q4k1/3bnppp/8/8/2P1Q3/8/PP6/1KR4R b - - 0 1";
    // a rook pin winning a queen
    const std::string PIN_2 = "1r4k1/2bq1ppp/8/8/5N2/7N/4QPPP/2R1K3 b - - 0 1";
    // a rook pin winning a bishop
    const std::string PIN_3 = "1k5r/p7/1p5b/2p5/2N1B3/8/8/1R2K3 b - - 0 1";
    // a bishop pin winning a rook
    const std::string PIN_4 = "1kr1n3/2N2p1p/6pb/4R3/8/1P6/P1P5/K6B b - - 0 1";
    // a bishop pin winning the exchange
    const std::string PIN_5 = "1k1q4/pppb4/1n6/8/4R3/6P1/4PP1P/5Q1K b - - 0 1";

    // a rook skewer winning a queen
    const std::string SKEWER_1 = "r5k1/5ppp/1b2n3/8/8/3N4/1PP1PR2/2K4Q b - - 0 1";
    // a bishop skewer winning a queen
    const std::string SKEWER_2 = "1k4r1/ppp5/8/3P4/3K2Pb/n3PP2/1Q5P/1R6 b - - 0 1";
    // a rook skewer winning a bishop
    const std::string SKEWER_3 = "5rk1/pp3ppp/8/8/8/4B3/PP2B1PP/1K6 b - - 0 1";
    // a bishop skewer winning the exchange
    const std::string SKEWER_4 = "r2bk3/1p1pppp1/8/7p/3R2P1/4R1P1/P7/5K2 b - - 0 1";
    // a bishop skewer winning a bishop
    const std::string SKEWER_5 = "2r2brk/6pp/8/8/3Q4/6P1/4PB2/7K b - - 0 1";

    // qxr, and the queen cannot be captured because of back rank mate
    const std::string MATE_TACTIC_1 = "3r2k1/3qRppp/pB6/N7/n7/8/5PPP/4Q1K1 b - - 0 1";
    // pinning the queen to a checkmate square
    const std::string MATE_TACTIC_2 = "1r5k/1q3p1p/4p3/6Q1/3b4/4R3/5PPP/5RK1 b - - 0 1";
    // winning the queen with a discovered mate in one threat
    const std::string MATE_TACTIC_3 = "1k4r1/ppp5/2q2n2/6b1/8/7N/1Q3PPP/R4RK1 b - - 0 1";
    // pinning the queen to a back rank mate threat
    const std::string MATE_TACTIC_4 = "1kqr4/p1p5/1p6/8/8/1R2Q3/R2P1PPP/6K1 b - - 0 1";
    // winning the queen with a discovered mate in two threat
    const std::string MATE_TACTIC_5 = "1k2bq2/ppp2n1Q/1b4PP/8/8/1R1P4/5PPP/6K1 b - - 0 1";

    // mate in one instead of capturing a queen
    const std::string MATE_1 = "6k1/1q1N1pbp/4Q1p1/8/8/8/PPP3R1/1K6 b - - 0 1";
    // mate in one instead of a longer mate
    const std::string MATE_2 = "1r4k1/5ppp/8/R7/7B/2q4B/PPP4r/R1K5 b - - 0 1";
    // discovered checkmate in one
    const std::string MATE_3 = "Q3q3/1b3pkp/6p1/2Qr4/8/8/1N2PP1P/7K b - - 0 1";
    // mate in two over instead of capturing either queen
    const std::string MATE_4 = "1Q2r3/Q4pbk/2n3pp/8/7P/1P6/2P5/1K6 b - - 0 1";
    // smothered mate in two
    const std::string MATE_5 = "6k1/b4ppp/1q6/8/1Q6/3n4/6PP/2RR3K b - - 0 1";
    // mate in two with no checks
    const std::string MATE_6 = "7k/1ppp1p1p/2q2npb/8/8/pP6/P1P3Q1/1K1R1B1R b - - 0 1";
    // mate in two with a queen sacrifice
    const std::string MATE_7 = "3r4/2q2pk1/6p1/8/8/8/4nPPP/2QRR2K b - - 0 1";
    // mate in two with a knight sacrifice
    const std::string MATE_8 = "1k5r/1pp5/3q4/4n3/4B3/8/3Q1PPP/1R3RK1 b  - - 0 1";
    // mate in three instead of capturing a queen
    const std::string MATE_9 = "1kr4Q/ppp3q1/2b5/6n1/8/8/2N2PPP/3R2KB b - - 0 1";
    // mate in three with no checks
    const std::string MATE_10 = "5r1k/2r2ppp/8/8/7b/P7/1P5P/6K1 b - - 0 1";

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
    int runPerft(int depth, std::string fen);
    // recursively find the number of leaf positions that exist at a given depth
    void perft(int depth, int& numLeaves);


};


#endif //DEEPENING1_TESTS_H
