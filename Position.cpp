//
// Created by Joe Chrisman on 9/15/22.
//

#include "Position.h"

/*
 * position constructor. fill the board according to an FEN string.
 * this works, but for now this implementation is not fully FEN compliant.
 * this is a simplified version and it only reads the pieces and their positions.
 */
Position::Position(const std::string& fen)
{
    pieces = std::vector<Bitboard>(12);

    int rank = 7;
    int file = 0;
    for (char c : fen)
    {
        assert(file <= 8);
        assert(rank >= 0);

        Bitboard board = toBoard(getSquare(rank, file));
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
}

/*
 * get a piece type for a given square using the bitboards.
 * there is no efficient way to do this, we will check every bitboard worst case.
 * if there is no piece, this function returns the NONE piece.
 *
 * TODO: this function will have to be faster. during move generation,
 *  we need to ask ourselves: "What piece did I just capture?".
 *  maybe find a way to get rid of the some branching (the if() and the piece<NONE)
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