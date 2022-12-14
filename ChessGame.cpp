//
// Created by Joe Chrisman on 9/15/22.
//

#include "ChessGame.h"

ChessGame::ChessGame(Position& position) :
search(position),
position(position)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_CreateWindowAndRenderer(
            WINDOW_SIZE,
            WINDOW_SIZE,
            0,
            &window,
            &renderer);

    // we are not dragging anything at the moment
    draggingFrom = NULL_SQUARE;
    draggingTo = NULL_SQUARE;
    dragging.piece = NONE;

    // create the graphics for the game
    createBoard();
    updateBoard();

    // remember the initial position for repetitions
    search.repetitions.push_back(position.hash);

    // we are not deciding what piece to promote to
    isPromoting = false;
    // we have not decided what piece to promote to
    promotionChoice = NONE;
    // graphics that show the user some squares with promotion options
    promotionOptions = std::vector<SquareUI>(4);
    promotionOptions[0].bounds = board[C5].bounds;
    promotionOptions[0].piece = PLAYER_KNIGHT;
    promotionOptions[1].bounds = board[D5].bounds;
    promotionOptions[1].piece = PLAYER_BISHOP;
    promotionOptions[2].bounds = board[E5].bounds;
    promotionOptions[2].piece = PLAYER_ROOK;
    promotionOptions[3].bounds = board[F5].bounds;
    promotionOptions[3].piece = PLAYER_QUEEN;

    run();
}

ChessGame::~ChessGame()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

// clear move option highlights.
// this should be called when the user stops dragging a piece
void ChessGame::clearMoveOptions()
{
    for (SquareUI& square : board)
    {
        square.isMoveOption = false;
    }
}

// clear all kinds of square highlighting.
// this should be called before a move is made
void ChessGame::clearHighlights()
{
    clearMoveOptions();
    for (SquareUI& square : board)
    {
        square.isPreviousMove = false;
        square.isChecking = false;
    }
}

// initialize the chessboard with 64 squares.
// also initialize vector of 12 preloaded piece textures
void ChessGame::createBoard()
{
    board = std::vector<SquareUI>(64);
    pieceTextures = std::vector<SDL_Texture*>(12);

    for (int piece = PLAYER_PAWN; piece < NONE; piece++)
    {
        pieceTextures[piece] = loadPieceTexture((PieceType)piece);
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
        board[squareNum].piece = position.getPiece(squareNum);
    }
}

/*
 * get a texture for a piece type. this should only be done once for each piece type
 * during the entire lifetime of the program. we store the textures in a
 * vector to avoid reloading them every frame and making things slow.
*/
SDL_Texture* ChessGame::loadPieceTexture(PieceType piece)
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

Square ChessGame::getSquareHovering(SDL_Point* mouse)
{
    for (Square square = A1; square <= H8; square++)
    {
        SquareUI& squareUi = board[square];
        if (SDL_PointInRect(
                mouse,
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

    for (SquareUI& square : board)
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

/*
 * get an initialized Move struct representing whatever move the player made.
 * this function supports en passant, promotion, and castling.
 * if the player promotes, this function raises the isPromoting flag,
 * and the move that gets returned is not made. once the player decides
 * what piece to promote to, the promotion is made on the board
 */
Move ChessGame::getMove()
{
    MoveType moveType = NORMAL;
    PieceType captured = position.getPiece<true>(draggingTo);

    // if we are moving a king
    if (dragging.piece == PLAYER_KING)
    {
        // if the king is moving along a rank
        if (getRank(draggingFrom) == getRank(draggingTo))
        {
            // if the king moved more than one square
            if (abs(draggingTo - draggingFrom) > 1)
            {
                // we castled
                moveType = CASTLE;
            }
        }
    }
    // if we are moving a pawn
    if (dragging.piece == PLAYER_PAWN)
    {
        // if we are capturing en passant
        if (getFile(draggingTo) != getFile(draggingFrom) && board[draggingTo].piece == NONE)
        {
            moveType = EN_PASSANT;
            captured = (dragging.piece == ENGINE_PAWN) ? PLAYER_PAWN : ENGINE_PAWN;
        }
        // if we are promoting a pawn
        if (getRank(draggingTo) == 7)
        {
            // if we have already made the promotion choice
            if (promotionChoice != NONE)
            {
                // figure out what we want to promote to
                if (promotionChoice == PLAYER_KNIGHT)
                {
                    moveType = KNIGHT_PROMOTION;
                }
                else if (promotionChoice == PLAYER_BISHOP)
                {
                    moveType = BISHOP_PROMOTION;
                }
                else if (promotionChoice == PLAYER_ROOK)
                {
                    moveType = ROOK_PROMOTION;
                }
                else if (promotionChoice == PLAYER_QUEEN)
                {
                    moveType = QUEEN_PROMOTION;
                }
                promotionChoice = NONE;
            }
            // if we are promoting but we have not
            // decided what piece to promote to yet
            else
            {
                // remember the user has not decided what piece to promote to yet
                isPromoting = true;
                // clear the screen
                SDL_RenderClear(renderer);
                // draw a white background
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                SDL_Rect background = SDL_Rect{0, 0, WINDOW_SIZE, WINDOW_SIZE};
                SDL_RenderFillRect(renderer, &background);
                // render the promotion options
                for (SquareUI& option : promotionOptions)
                {
                    SDL_RenderCopy(
                            renderer,
                            pieceTextures[option.piece],
                            nullptr,
                            &option.bounds
                    );
                }
                /*
                 * present the promotion options. they will stay on the screen
                 * until the user clicks on one of the options. until then,
                 * we don't render the chess board again
                 */
                SDL_RenderPresent(renderer);
            }
        }
    }
    return makeMove(
        moveType,
        dragging.piece,
        captured,
        draggingFrom,
        draggingTo);
}

void ChessGame::onMousePressed(SDL_Point& mouse)
{
    // if we are deciding what piece to promote to
    if (isPromoting)
    {
        // go through the promotion options
        for (SquareUI& option : promotionOptions)
        {
            // figure out what option the user clicked on
            if (SDL_PointInRect(&mouse, &option.bounds))
            {
                // remember what the user chose
                promotionChoice = option.piece;
                // remember the user decided what to promote to
                isPromoting = false;
                return;
            }
        }
    }
    else
    {
        // figure out what square the user clicked on
        Square clicked = getSquareHovering(&mouse);
        // if we clicked on a square
        if (clicked != NULL_SQUARE)
        {
            SquareUI& square = board[clicked];
            // if we clicked on a piece
            if (square.piece != NONE)
            {
                // start dragging that piece
                draggingFrom = clicked;
                dragging.piece = square.piece;
                dragging.bounds = square.bounds;
                dragging.bounds.x = mouse.x - SQUARE_SIZE / 2;
                dragging.bounds.y = mouse.y - SQUARE_SIZE / 2;

                // generate legal moves for the dragging piece
                if (position.isEngineMove)
                {
                    search.moveGen.genEngineMoves();
                }
                else
                {
                    search.moveGen.genPlayerMoves();
                }
                for (Move& move : search.moveGen.moveList)
                {
                    if (getSquareFrom(move) == clicked)
                    {
                        // highlight move options
                        board[getSquareTo(move)].isMoveOption = true;
                    }
                }
                // remove the piece from its square
                square.piece = NONE;
            }
        }
    }
}

void ChessGame::onMouseReleased(SDL_Point& mouse)
{
    // if we are not deciding what piece to promote to
    if (!isPromoting)
    {
        // if we are dragging something
        if (dragging.piece != NONE)
        {
            // if we are not promoting
            if (promotionChoice == NONE)
            {
                // figure out what square we want to drop the piece
                draggingTo = getSquareHovering(&mouse);
            }
            if (draggingTo != NULL_SQUARE && board[draggingTo].isMoveOption)
            {
                // figure out what move the player made
                Move move = getMove();
                // if the move we just made was a promotion,
                // we need the user to decide what piece to promote to
                if (isPromoting)
                {
                    return;
                }
                clearHighlights();
                // make the move
                position.makeMove<false>(move);
                // if the move the player made was irreversible
                if (moves::isIrreversible(move))
                {
                    // clear the repetitions list. we can never get the old position again
                    search.repetitions.clear();
                }
                // the player played a draw by threefold repetition or 50 move rule
                if (search.repeated() || position.rights.halfMoveClock >= 50)
                {
                    // deal with this later
                    assert(false);
                }
                // remember the position the player played
                search.repetitions.push_back(position.hash);
                // set previous move highlights
                board[draggingTo].isPreviousMove = true;
                board[draggingFrom].isPreviousMove = true;
                // if we are in check, highlight our king
                Bitboard checkers = search.moveGen.getCheckers<true>();
                board[toSquare(position.pieces[position.isEngineMove ? ENGINE_KING : PLAYER_KING])].isChecking = checkers;
                // highlight the pieces checking our king
                while (checkers)
                {
                    board[popFirstPiece(checkers)].isChecking = true;
                }
            }
        }
        // we released the mouse.
        // forget whatever we were dragging
        draggingFrom = NULL_SQUARE;
        draggingTo = NULL_SQUARE;
        dragging.piece = NONE;
        clearMoveOptions();
        // bring graphics up to date with the position.
        // this moves the piece we were dragging back to the right square
        updateBoard();
    }
}

void ChessGame::run()
{
    render();
    while (true)
    {
        SDL_Event event;
        if (SDL_WaitEvent(&event))
        {
            // if the user wants to close the window
            if (event.type == SDL_QUIT)
            {
                // get out of the event loop
                break;
            }
            // if the user clicked the mouse
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_Point mouse = SDL_Point{
                    event.button.x,
                    event.button.y
                };
                onMousePressed(mouse);
            }
            // if the user released the mouse
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                SDL_Point mouse = SDL_Point{
                        event.button.x,
                        event.button.y
                };
                onMouseReleased(mouse);
            }
            // if the user moved the mouse
            else if (event.type == SDL_MOUSEMOTION)
            {
                // if we are dragging a piece
                if (dragging.piece != NONE)
                {
                    // move whatever piece we are dragging to the mouse
                    dragging.bounds.x = event.motion.x - SQUARE_SIZE / 2;
                    dragging.bounds.y = event.motion.y - SQUARE_SIZE / 2;
                }
            }
            // render the player's move
            if (!isPromoting)
            {
                render();
            }
            // if it is the engine's turn
            if (position.isEngineMove)
            {
                // spend a minimum of three seconds calculating a move...
                Move engineMove = search.getBestMove(3000);
                /*
                 * we just spend many seconds searching for the best move.
                 * meanwhile, the player might have been dragging the left mouse
                 * button around, or clicking the squares on the board. Because
                 * of this, we need to clear the even queue to prevent some graphical
                 * bugs from happening, and to stop the user from accidentally dragging pieces
                 * while the engine is still searching for the best move.
                 * SDL_FlushEvents() does not seem to work... so i am stuck with the following solution.
                 * TODO: find the real solution
                 */
                while (SDL_PollEvent(&event)) {}
                // play the move
                search.moveGen.position.makeMove<true>(engineMove);
                // if the engine played a draw by threefold repetition or 50 move rule
                if (search.repeated() || position.rights.halfMoveClock >= 50)
                {
                    // deal with this later
                    assert(false);
                }
                // remember the position the engine played
                search.repetitions.push_back(position.hash);
                clearHighlights();
                updateBoard();
                // to remember what pieces are checking the engine king, if any
                Bitboard checkers = search.moveGen.getCheckers<false>();
                // if we the engine is in check, highlight the king
                board[toSquare(position.pieces[position.isEngineMove ? ENGINE_KING : PLAYER_KING])].isChecking = checkers;
                // highlight the pieces checking the engine king
                while (checkers)
                {
                    board[popFirstPiece(checkers)].isChecking = true;
                }
                board[getSquareTo(engineMove)].isPreviousMove = true;
                board[getSquareFrom(engineMove)].isPreviousMove = true;
                // render the engine's move
                render();
            }
        }
    }
}
