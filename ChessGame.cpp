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

    search = Search();
    enginesTurn = ENGINE_IS_WHITE;

    draggingFrom = NULL_SQUARE;
    draggingTo = NULL_SQUARE;
    dragging.piece = NONE;

    createBoard();
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
    board = std::vector<SquareUI>(64);
    pieceTextures = std::vector<SDL_Texture*>(12);

    for (int piece = PLAYER_PAWN; piece < NONE; piece++)
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
        board[square].isLight = rank % 2 != file % 2;
    }
}

// bring our graphical chess board back to the state of the bitboards
void ChessGame::updateBoard()
{
    assert(pieceTextures.size() == 12);
    assert(board.size() == 64);

    for (Square squareNum = A1; squareNum <= H8; squareNum++)
    {
        board[squareNum].piece = search.moveGen.position.getPiece(squareNum);
    }
}

/*
 * get a texture for a piece type. this should only be done once for each piece type
 * during the entire lifetime of the program. we store the textures in a
 * vector to avoid reloading them every frame and making things slow.
*/
SDL_Texture* ChessGame::loadPieceTexture(Piece piece)
{
    assert(piece != NONE);

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

        default: assert(false);
    }
    imagePath += ".bmp";
    SDL_Surface *surface = SDL_LoadBMP(imagePath.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

Square ChessGame::getSquareHovering(SDL_Point mouse)
{
    for (Square square = A1; square <= H8; square++)
    {
        SquareUI& squareUi = board[square];
        if (SDL_PointInRect(
                new SDL_Point{mouse.x, mouse.y},
                &squareUi.bounds))
        {
            return square;
        }
    }
    return NULL_SQUARE;
}

void ChessGame::render()
{
    assert(renderer);
    assert(window);

    SDL_RenderClear(renderer);

    for (SquareUI square : board)
    {
        /*
         * because of the way these if/else statements are laid out,
         * some highlights can have precedence over others. for example,
         * a previous move highlight can be overridden by a move option highlight.
         * same thing with a previous move highlight being overridden by a checking highlight.
         * a square can have multiple highlights at once, but the correct highlight gets chosen here.
         */
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
            color = PREVIOUS_MOVE_COLOR;
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

        // render the square and its piece if it has one
        SDL_RenderFillRect(renderer, &square.bounds);
        if (square.piece != NONE)
        {
            SDL_RenderCopy(
                    renderer,
                    pieceTextures[square.piece],
                    nullptr,
                    &square.bounds);
        }
    }

    // render the piece we are dragging
    if (dragging.piece != NONE)
    {
        SDL_RenderCopy(
                renderer,
                pieceTextures[dragging.piece],
                nullptr,
                &dragging.bounds);
    }

    SDL_RenderPresent(renderer);
}

Move ChessGame::getMove()
{
    assert(dragging.piece != NONE);
    assert(draggingFrom >= A1);
    assert(draggingFrom <= H8);
    assert(draggingTo >= A1);
    assert(draggingTo <= H8);

    MoveType type = NORMAL;

    // if we are moving a king
    if (dragging.piece == ENGINE_KING || dragging.piece == PLAYER_KING)
    {
        // if the king is moving along a rank
        if (getRank(draggingFrom) == getRank(draggingTo))
        {
            // if the king moved more than one square
            if (abs(draggingTo - draggingFrom) > 1)
            {
                // we castled
                type = CASTLE;
            }
        }

    }

    // this will have to be changed later, when we include special moves
    // like castling, en passant, and promotions.
    return Move{
        type,
        draggingFrom,
        draggingTo,
        dragging.piece,
        search.moveGen.position.getPiece(draggingTo)
    };
}

void ChessGame::run()
{
    render();
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            // if the user wants to close the window
            if (event.type == SDL_QUIT)
            {
                // get out of the event loop
                break;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                assert(dragging.piece == NONE);
                assert(draggingFrom == NULL_SQUARE);
                assert(draggingTo == NULL_SQUARE);

                Square clicked = getSquareHovering(
                        SDL_Point{
                            event.button.x,
                            event.button.y});

                if (clicked != NULL_SQUARE)
                {
                    SquareUI& square = board[clicked];
                    if (square.piece != NONE)
                    {
                        dragging.piece = square.piece;
                        draggingFrom = clicked;
                        dragging.bounds = square.bounds;
                        dragging.bounds.x = event.button.x - SQUARE_SIZE / 2;
                        dragging.bounds.y = event.button.y - SQUARE_SIZE / 2;

                        // generate legal moves
                        if (enginesTurn)
                        {
                            search.moveGen.genEngineMoves();
                        }
                        else
                        {
                            search.moveGen.genPlayerMoves();
                        }
                        for (Move& move : search.moveGen.moveList)
                        {
                            if (move.from == clicked)
                            {
                                // highlight move options
                                board[move.to].isMoveOption = true;
                            }
                        }

                        // remove the piece from its square
                        square.piece = NONE;
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (dragging.piece != NONE)
                {
                    draggingTo = getSquareHovering(SDL_Point{
                        event.button.x,
                        event.button.y
                    });

                    if (draggingTo != NULL_SQUARE && board[draggingTo].isMoveOption)
                    {
                        clearHighlights();
                        // figure out what move the player made
                        Move move = getMove();
                        if (enginesTurn)
                        {
                            search.moveGen.position.makeMove<true>(move);
                        }
                        else
                        {
                            search.moveGen.position.makeMove<false>(move);

                        }
                        board[draggingTo].isPreviousMove = true;
                        board[draggingFrom].isPreviousMove = true;

                        enginesTurn = !enginesTurn;
                    }
                }

                draggingFrom = NULL_SQUARE;
                draggingTo = NULL_SQUARE;
                dragging.piece = NONE;
                clearMoveOptions();

                // bring graphics up to date with the position
                updateBoard();

            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                if (dragging.piece != NONE)
                {
                    dragging.bounds.x = event.motion.x - SQUARE_SIZE / 2;
                    dragging.bounds.y = event.motion.y - SQUARE_SIZE / 2;
                }
            }
            // render the player's move
            render();
            /*
            if (enginesTurn)
            {
                // spend potentially many seconds calculating a move...
                Move engineMove = search.getBestMove();
                // play the move
                search.moveGen.position.makeMove(engineMove);
                clearHighlights();
                updateBoard();
                board[engineMove.to].isPreviousMove = true;
                board[engineMove.from].isPreviousMove = true;
                // render the engine's move
                render();
                enginesTurn = false;
            }*/
        }
    }
}
