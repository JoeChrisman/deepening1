#include "ChessGame.h"
//#include "Tests.h"

int main()
{
    Position position(ENGINE_IS_WHITE ? "RNBKQBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbkqbnr w KQkq -"
                                      : "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");


    //Position position("3k4/3q4/8/8/8/8/8/4K3 w - - 0 1"); // to test a KQ vs K endgame
    //Position position("k3q3/pp3P2/8/3Q4/3P4/8/1R4P1/7K w - - 0 1"); // to make sure the engine will force a draw if necessary
    ChessGame game(position);

    //Tests tests;

    return 0;
}
