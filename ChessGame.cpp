//
// Created by Joe Chrisman on 9/15/22.
//

#include "ChessGame.h"

ChessGame::ChessGame()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_CreateWindowAndRenderer(
            WINDOW_SIZE,
            WINDOW_SIZE,
            0,
            &window,
            &renderer);

    createBoard();
    position = new Position(ENGINE_IS_WHITE ?
                                ENGINE_WHITE_FEN :
                                ENGINE_BLACK_FEN);
    updateBoard();

    run();
}

ChessGame::~ChessGame()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void ChessGame::clearMoveOptions()
{
    for (SquareUI& square : board)
    {
        square.isMoveOption = false;
    }
}

void ChessGame::clearHighlights()
{
    clearMoveOptions();
    for (SquareUI& square : board)
    {
        square.isPreviousMove = false;
        square.isChecking = false;
    }
}

void ChessGame::createBoard()
{
    // create a vector to hold each graphical square
    board = std::vector<SquareUI>(64);
    // create a vector of textures to hold piece textures
    // 1-12 correspond to piece types, 13 corresponds to the NONE piece. it holds a nullptr.
    pieceTextures = std::vector<SDL_Texture*>(13);
    for (int piece = PLAYER_PAWN; piece <= NONE; piece++)
    {
        pieceTextures[piece] = loadPieceTexture((Piece)piece);
    }
    for (Square square = A1; square <= H8; square++)
    {
        int rank = getRank(square);
        int file = getFile(square);

        board[square].bounds = SDL_Rect{
            file * SQUARE_SIZE,
            (7 - rank) * SQUARE_SIZE,
            SQUARE_SIZE,
            SQUARE_SIZE
        };
        board[square].isLight = rank % 2 == file % 2;
    }
}

// bring our visual chess board back to the state of the bitboards
void ChessGame::updateBoard()
{
    clearHighlights();
    for (Square squareNum = A1; squareNum <= H8; squareNum++)
    {
        board[squareNum].piece = position->getPiece(squareNum);
    }
}

/*
 * get a texture for a piece type. this should only be done once for each piece type
 * during the entire duration of the program running. we store the textures in a
 * vector to avoid reloading them every frame and making things slow.
*/
SDL_Texture* ChessGame::loadPieceTexture(Piece piece)
{
    char engineColor = ENGINE_IS_WHITE ? 'w' : 'b';
    char playerColor = ENGINE_IS_WHITE ? 'b' : 'w';
    std::string imagePath = "Images/";
    switch (piece)
    {
        case ENGINE_PAWN: imagePath += engineColor; imagePath += 'p'; break;
        case ENGINE_KNIGHT: imagePath += engineColor; imagePath += 'n'; break;
        case ENGINE_BISHOP: imagePath += engineColor; imagePath += 'b'; break;
        case ENGINE_ROOK: imagePath += engineColor; imagePath += 'r'; break;
        case ENGINE_QUEEN: imagePath += engineColor; imagePath += 'q'; break;
        case ENGINE_KING: imagePath += engineColor; imagePath += 'k'; break;

        case PLAYER_PAWN: imagePath += playerColor; imagePath += 'p'; break;
        case PLAYER_KNIGHT: imagePath += playerColor; imagePath += 'n'; break;
        case PLAYER_BISHOP: imagePath += playerColor; imagePath += 'b'; break;
        case PLAYER_ROOK: imagePath += playerColor; imagePath += 'r'; break;
        case PLAYER_QUEEN: imagePath += playerColor; imagePath += 'q'; break;
        case PLAYER_KING: imagePath += playerColor; imagePath += 'k'; break;

        default: return nullptr;
    }
    imagePath += ".bmp";
    SDL_Surface *surface = SDL_LoadBMP(imagePath.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void ChessGame::render()
{
    SDL_RenderClear(renderer);

    for (SquareUI square : board)
    {
        // because of the way these if/else statements are laid out,
        // some highlights can have precedence over others. for example,
        // a previous move highlight can be overridden by a move option highlight.
        // same thing with a previous move highlight being overridden by a checking highlight.
        // a square can have multiple highlights at once, but the correct highlight gets chosen here.
        int color;
        if (square.isMoveOption)
        {
            color = MOVE_OPTION_COLOR;
        }
        else if (square.isChecking)
        {
            color = CHECKING_SQUARE_COLOR;
        }
        else if (square.isPreviousMove)
        {
            color = MOVE_HIGHLIGHT_COLOR;
        }
        else if (square.isLight)
        {
            color = LIGHT_SQUARE_COLOR;
        }
        else
        {
            color = DARK_SQUARE_COLOR;
        }
        SDL_SetRenderDrawColor(
                renderer,
                color >> 16 & 0xff,
                color >> 8 & 0xff,
                color & 0xff,
                255);

        SDL_RenderFillRect(renderer, &square.bounds);
        SDL_RenderCopy(
                renderer,
                pieceTextures[square.piece],
                nullptr,
                &square.bounds);

    }
    SDL_RenderPresent(renderer);
}

void ChessGame::run()
{
    render();
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            // we only need to rerender when there is a user driven event... for now...
            render();
            // if the user wants to close the window
            if (event.type == SDL_QUIT)
            {
                break;
            }
        }
    }
}
