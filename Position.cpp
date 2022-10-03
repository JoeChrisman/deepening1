//
// Created by Joe Chrisman on 9/15/22.
//

#include "Position.h"

// position constructor.
// accepts a fen string and sets up the board accordingly
Position::Position(std::string fen)
{
    // initialize pieces
    pieces = std::vector<Bitboard>(12);
    hash = 0x0000000000000000;
    // set up the board
    readFen(fen);
    updateBitboards();
}

void Position::updateBitboards()
{

    // figure out what pieces the engine can capture
    playerPieces = pieces[PLAYER_PAWN] |
                   pieces[PLAYER_KNIGHT] |
                   pieces[PLAYER_BISHOP] |
                   pieces[PLAYER_ROOK] |
                   pieces[PLAYER_QUEEN] |
                   pieces[PLAYER_KING];

    // figure out what pieces the player can capture
    enginePieces = pieces[ENGINE_PAWN] |
                   pieces[ENGINE_KNIGHT] |
                   pieces[ENGINE_BISHOP] |
                   pieces[ENGINE_ROOK] |
                   pieces[ENGINE_QUEEN] |
                   pieces[ENGINE_KING];

    occupied = enginePieces | playerPieces;
    // now figure out what empty squares we can move to
    empties = ~occupied;

    playerMovable = enginePieces | empties;
    engineMovable = playerPieces | empties;
}

PieceType Position::getPiece(Square square)
{
    Bitboard board = toBoard(square);

    for (int piece = PLAYER_PAWN; piece < NONE; piece++)
    {
        if (pieces[piece] & board)
        {
            return (PieceType)piece;
        }
    }
    return NONE;
}

/*
 * initialize the position according to a FEN string.
 * This makes automated testing possible.
 * https://www.chessprogramming.org/Forsyth-Edwards_Notation
 */
void Position::readFen(const std::string& fen)
{
    std::string fields[6] = {
        "", // 1) piece movement
        "", // 2) side to move
        "", // 3) castling ability
        "", // 4) en passant capture square
        "", // 5) half move clock
        ""  // 6) full move clock
    };
    // parse the fields
    std::stringstream stream(fen);
    int fieldIndex = 0;
    while (std::getline(stream, fields[fieldIndex], ' '))
    {
        fieldIndex++;
    }

    int rank = 7;
    int file = 0;
    for (char c : fields[0])
    {
        // if we read a digit, we want to skip over some squares
        if (c >= '0' && c <= '9')
        {
            file += c - '0';
        }
        // if we read a slash, we need to go to the next rank and continue reading
        else if (c == '/')
        {
            file = 0;
            rank--;
        }
        else
        {
            Square square = getSquare(rank, file);
            Bitboard piece = toBoard(square);

            if (c == 'P')
            {
                pieces[ENGINE_IS_WHITE ? ENGINE_PAWN : PLAYER_PAWN] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][ENGINE_IS_WHITE ? ENGINE_PAWN : PLAYER_PAWN];
            }
            else if (c == 'N')
            {
                pieces[ENGINE_IS_WHITE ? ENGINE_KNIGHT : PLAYER_KNIGHT] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][ENGINE_IS_WHITE ? ENGINE_KNIGHT : PLAYER_KNIGHT];
            }
            else if (c == 'B')
            {
                pieces[ENGINE_IS_WHITE ? ENGINE_BISHOP : PLAYER_BISHOP] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][ENGINE_IS_WHITE ? ENGINE_BISHOP : PLAYER_BISHOP];
            }
            else if (c == 'R')
            {
                 pieces[ENGINE_IS_WHITE ? ENGINE_ROOK : PLAYER_ROOK] |= piece;
                 hash ^= SQUARE_PIECE_KEYS[square][ENGINE_IS_WHITE ? ENGINE_ROOK : PLAYER_ROOK];
            }
            else if (c == 'Q')
            {
                pieces[ENGINE_IS_WHITE ? ENGINE_QUEEN : PLAYER_QUEEN] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][ENGINE_IS_WHITE ? ENGINE_QUEEN : PLAYER_QUEEN];
            }
            else if (c == 'K')
            {
                pieces[ENGINE_IS_WHITE ? ENGINE_KING : PLAYER_KING] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][ENGINE_IS_WHITE ? ENGINE_KING : PLAYER_KING];
            }
            else if (c == 'p')
            {
                pieces[!ENGINE_IS_WHITE ? ENGINE_PAWN : PLAYER_PAWN] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][!ENGINE_IS_WHITE ? ENGINE_PAWN : PLAYER_PAWN];
            }
            else if (c == 'n')
            {
                pieces[!ENGINE_IS_WHITE ? ENGINE_KNIGHT : PLAYER_KNIGHT] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][!ENGINE_IS_WHITE ? ENGINE_KNIGHT : PLAYER_KNIGHT];
            }
            else if (c == 'b')
            {
                pieces[!ENGINE_IS_WHITE ? ENGINE_BISHOP : PLAYER_BISHOP] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][!ENGINE_IS_WHITE ? ENGINE_BISHOP : PLAYER_BISHOP];
            }
            else if (c == 'r')
            {
                pieces[!ENGINE_IS_WHITE ? ENGINE_ROOK : PLAYER_ROOK] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][!ENGINE_IS_WHITE ? ENGINE_ROOK : PLAYER_ROOK];
            }
            else if (c == 'q')
            {
                pieces[!ENGINE_IS_WHITE ? ENGINE_QUEEN : PLAYER_QUEEN] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][!ENGINE_IS_WHITE ? ENGINE_QUEEN : PLAYER_QUEEN];
            }
            else if (c == 'k')
            {
                pieces[!ENGINE_IS_WHITE ? ENGINE_KING : PLAYER_KING] |= piece;
                hash ^= SQUARE_PIECE_KEYS[square][!ENGINE_IS_WHITE ? ENGINE_KING : PLAYER_KING];
            }
            else
            {
                // invalid FEN string
                assert(false);
            }
            file++;
        }
    }

    isEngineMove = fields[1].find(ENGINE_IS_WHITE ? 'w' : 'b') != std::string::npos;
    if (isEngineMove)
    {
        hash ^= ENGINE_TO_MOVE_KEY;
    }

    rights.engineCastleKingside = fields[2].find(ENGINE_IS_WHITE ? 'K' : 'k') != std::string::npos;
    if (rights.engineCastleKingside)
    {
        hash ^= ENGINE_CASTLE_KINGSIDE_KEY;
    }
    rights.engineCastleQueenside = fields[2].find(ENGINE_IS_WHITE ? 'Q' : 'q') != std::string::npos;
    if (rights.engineCastleQueenside)
    {
        hash ^= ENGINE_CASTLE_QUEENSIDE_KEY;
    }
    rights.playerCastleKingside = fields[2].find(ENGINE_IS_WHITE ? 'k' : 'K') != std::string::npos;
    if (rights.playerCastleKingside)
    {
        hash ^= PLAYER_CASTLE_KINGSIDE_KEY;
    }
    rights.playerCastleQueenside = fields[2].find(ENGINE_IS_WHITE ? 'q' : 'Q') != std::string::npos;
    if (rights.playerCastleQueenside)
    {
        hash ^= PLAYER_CASTLE_QUEENSIDE_KEY;
    }

    Square enPassant = squares::toSquare(fields[3]);
    rights.enPassantCapture = (enPassant != NULL_SQUARE) ? toBoard(enPassant) : EMPTY_BITBOARD;
    if (enPassant != NULL_SQUARE)
    {
        hash ^= EN_PASSANT_KEYS[getFile(enPassant)];
    }

    // if the half-move or full-move clocks are not given, atoi() will still be zero
    halfMoveClock = atoi(fields[4].c_str());
    fullMoves = atoi(fields[5].c_str());
}