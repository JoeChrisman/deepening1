//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_CONSTANTS_H
#define DEEPENING1_CONSTANTS_H

const int WINDOW_SIZE = 800;
const int SQUARES_WIDE = 8;
const int SQUARE_SIZE = WINDOW_SIZE / SQUARES_WIDE;

const bool ENGINE_IS_WHITE = true;

/*
 * FEN strings for the initial board state.
 */
const std::string ENGINE_BLACK_FEN = "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR";
const std::string ENGINE_WHITE_FEN = "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr";

enum Piece
{
    PLAYER_PAWN,
    PLAYER_KNIGHT,
    PLAYER_BISHOP,
    PLAYER_ROOK,
    PLAYER_QUEEN,
    PLAYER_KING,
    ENGINE_PAWN,
    ENGINE_KNIGHT,
    ENGINE_BISHOP,
    ENGINE_ROOK,
    ENGINE_QUEEN,
    ENGINE_KING,
    NONE
};

const int DARK_SQUARE_COLOR = 0x0;
const int LIGHT_SQUARE_COLOR = 0xffffff;
const int CHECKING_SQUARE_COLOR = 0x880808;
const int MOVE_OPTION_COLOR = 0x89cff0;
const int PREVIOUS_MOVE_COLOR = 0x50C878;

#endif //DEEPENING1_CONSTANTS_H
