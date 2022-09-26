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
    // set up the board
    readFen(fen);
    updateBitboards();
}

std::string Position::toNotation(Square square)
{
    std::string notation = "--";
    int rank = getRank(square);
    int file = getFile(square);
    notation[0] = 'a' + (ENGINE_IS_WHITE ? 8 - file : file);
    notation[1] = '1' + (ENGINE_IS_WHITE ? 8 - rank : rank);
    return notation;
}

Square Position::toSquare(std::string notation)
{
    if (notation[0] >= 'a' && notation[0] <= 'h' &&
        notation[1] >= '1' && notation[1] <= '8')
    {
        int file = (ENGINE_IS_WHITE ? 'h' - notation[0] : notation[0] - 'a');
        int rank = (ENGINE_IS_WHITE ? '8' - notation[1] : notation[1] - '1');
        return getSquare(rank, file);
    }
    return NULL_SQUARE;
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


/*
 * get a piece type for a given square using the bitboards.
 * there is no efficient way to do this, we will check every bitboard worst case.
 * if there is no piece, this function returns the NONE piece.
 *
 * TODO: this function will have to be faster. during move generation,
 *  we need to ask ourselves: "What piece did I just capture?".
 *  maybe find a way to get rid of the some branching (the if() and the piece<NONE)
 *  maybe store pieces in a traditional 8x8 array, alongside the bitboard representation?
 */
Piece Position::getPiece(Square square)
{
    assert(square >= A1);
    assert(square <= H8);

    Bitboard board = toBoard(square);
    for (int piece = PLAYER_PAWN; piece < NONE; piece++)
    {
        if (pieces[piece] & board)
        {
            return (Piece)piece;
        }
    }
    return NONE;
}

/*
 * fill the board according to an FEN string.
 * this works, but for now this implementation is not yet fully FEN compliant.
 * this is a simplified version because we don't care about half-move and full-move clock
 * https://www.chessprogramming.org/Forsyth-Edwards_Notation
 */
void Position::readFen(const std::string& fen)
{
    std::string fields[6] = {
        "", // 1) piece movement
        "", // 2) side to move
        "", // 3) castling ability
        "", // 4) en passant capture square
        "", // TODO 5) half-move clock
        ""  // TODO 6) full-move clock
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
            Bitboard board = toBoard(getSquare(rank, file));
            switch (c)
            {
                case 'p': pieces[ENGINE_IS_WHITE ? PLAYER_PAWN : ENGINE_PAWN] |= board; break;
                case 'P': pieces[ENGINE_IS_WHITE ? ENGINE_PAWN : PLAYER_PAWN] |= board; break;
                case 'n': pieces[ENGINE_IS_WHITE ? PLAYER_KNIGHT : ENGINE_KNIGHT] |= board; break;
                case 'N': pieces[ENGINE_IS_WHITE ? ENGINE_KNIGHT : PLAYER_KNIGHT] |= board; break;
                case 'b': pieces[ENGINE_IS_WHITE ? PLAYER_BISHOP : ENGINE_BISHOP] |= board; break;
                case 'B': pieces[ENGINE_IS_WHITE ? ENGINE_BISHOP : PLAYER_BISHOP] |= board; break;
                case 'r': pieces[ENGINE_IS_WHITE ? PLAYER_ROOK : ENGINE_ROOK] |= board; break;
                case 'R': pieces[ENGINE_IS_WHITE ? ENGINE_ROOK : PLAYER_ROOK] |= board; break;
                case 'q': pieces[ENGINE_IS_WHITE ? PLAYER_QUEEN : ENGINE_QUEEN] |= board; break;
                case 'Q': pieces[ENGINE_IS_WHITE ? ENGINE_QUEEN : PLAYER_QUEEN] |= board; break;
                case 'k': pieces[ENGINE_IS_WHITE ? PLAYER_KING : ENGINE_KING] |= board; break;
                case 'K': pieces[ENGINE_IS_WHITE ? ENGINE_KING : PLAYER_KING] |= board; break;
                default: assert(false); // invalid FEN string
            }
            file++;
        }
    }

    isEngineMove = fields[1].find(ENGINE_IS_WHITE ? 'w' : 'b') != std::string::npos;

    engineCastleKingside = fields[2].find(ENGINE_IS_WHITE ? 'K' : 'k') != std::string::npos;
    engineCastleQueenside = fields[2].find(ENGINE_IS_WHITE ? 'Q' : 'q') != std::string::npos;
    playerCastleKingside = fields[2].find(ENGINE_IS_WHITE ? 'k' : 'K') != std::string::npos;
    playerCastleQueenside = fields[2].find(ENGINE_IS_WHITE ? 'q' : 'Q') != std::string::npos;

    Square enPassant = toSquare(fields[3]);
    enPassantCapture = (enPassant != NULL_SQUARE) ? toBoard(enPassant) : EMPTY_BITBOARD;
}