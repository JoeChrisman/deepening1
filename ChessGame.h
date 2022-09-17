//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_CHESSGAME_H
#define DEEPENING1_CHESSGAME_H

#include <SDL2/SDL.h>
#include "Position.h"

// a square on the graphical chess board
struct SquareUI
{
    Piece piece;
    SDL_Rect bounds;

    bool isLight; // is this a light or dark square?
    bool isMoveOption; // is this square a move option?
    bool isPreviousMove; // did we just move a piece to or from this square?
    bool isChecking; // is a piece on this square attacking the enemy king?
};

class ChessGame
{
public:
    ChessGame();
    ~ChessGame();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Position* position;

    // a vector to represent our graphical board
    std::vector<SquareUI> board;
    // 12 preloaded piece textures plus one nullptr for a NONE piece
    std::vector<SDL_Texture*> pieceTextures;
    // the piece the user is dragging
    SquareUI dragging;

    // initialize graphical board with square bounds and colors
    void createBoard();
    // update graphical board with piece locations and clear highlights
    void updateBoard();
    // clear all types of highlighting
    void clearHighlights();
    // clear highlighting from move option squares
    void clearMoveOptions();

    SDL_Texture* loadPieceTexture(Piece piece);
    // render the chess board
    void render();
    // event loop
    void run();
};


#endif //DEEPENING1_CHESSGAME_H
