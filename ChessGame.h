//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_CHESSGAME_H
#define DEEPENING1_CHESSGAME_H

#include <SDL2/SDL.h>
#include "Search.h"

class ChessGame
{
public:
    ChessGame(Position& position);
    ~ChessGame();

private:

    // a square on the graphical chess board
    struct SquareUI
    {
        PieceType piece;
        SDL_Rect bounds;

        bool isLight; // is this a light or dark square?
        bool isMoveOption; // is this square a move option?
        bool isPreviousMove; // did we just move a piece to or from this square?
        bool isChecking; // is a piece on this square attacking the enemy king?
    };

    SDL_Window* window;
    SDL_Renderer* renderer;

    Search search;
    Position& position;

    // a vector to represent our graphical board
    std::vector<SquareUI> board;
    // 12 preloaded piece textures plus one nullptr for a NONE piece
    std::vector<SDL_Texture*> pieceTextures;

    // the piece the user is dragging
    SquareUI dragging;
    Square draggingFrom;
    Square draggingTo;

    // if true, we want to display promotion choices
    bool isPromoting;
    std::vector<SquareUI> promotionOptions;
    PieceType promotionChoice;

    // initialize graphical board with square bounds and colors
    void createBoard();
    // update graphical board with piece locations and clear highlights
    void updateBoard();
    // clear all types of highlighting
    void clearHighlights();
    // clear highlighting from move option squares
    void clearMoveOptions();

    // get whatever square the cursor is over. will return
    // NULL_SQUARE if the cursor is not over a square
    Square getSquareHovering(SDL_Point* mouse);

    /*
     * create a move based on what piece the player moved,
     * where the player moved it from, and where he moved it to.
     * then we can call position.makeMove(move) to move the pieces around
     */
    Move getMove();

    SDL_Texture* loadPieceTexture(PieceType piece);
    // render the chess board
    void render();
    // event loop
    void run();

    void onMousePressed(SDL_Point& mouse);
    void onMouseReleased(SDL_Point& mouse);
};


#endif //DEEPENING1_CHESSGAME_H
