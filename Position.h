//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_POSITION_H
#define DEEPENING1_POSITION_H

#include <vector>
#include <string>
#include <sstream>
#include "Bitboards.h"

enum MoveType
{
    NORMAL,
    CASTLE,
    EN_PASSANT,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION
};

struct Move
{
    MoveType type;
    Square from;
    Square to;
    Piece moved;
    Piece captured;
};

class Position
{

public:
    // create a chess game from a FEN string
    Position(std::string fen);

    std::string toNotation(Square square);
    Square toSquare(std::string notation);

    Piece getPiece(Square square);

    /*
     * a vector of bitboards of pieces. there is one bitboard for each piece type.
     * the vector is indexed using the Piece enumeration in Constants.h
     */
    std::vector<Bitboard> pieces;

    // the bitboard of the capture square of an available en passant move
    Bitboard enPassantCapture;

    bool engineCastleKingside;
    bool engineCastleQueenside;
    bool playerCastleKingside;
    bool playerCastleQueenside;

    bool isEngineMove;

    // the number of half moves have been played since the last reversible move.
    // used for enforcing the draw by fifty move rule
    int halfMoveClock;

    // the number of full moves (ex. e4 e5) have been played so far.
    // not sure how it will ever be used. But it is a part of FEN specification
    int fullMoves;

    // some extra information about the position
    Bitboard empties;
    Bitboard occupied;
    Bitboard enginePieces;
    Bitboard playerPieces;
    Bitboard playerMovable; // engine pieces or empty squares
    Bitboard engineMovable; // player pieces or empty squares
    // update the additional information
    void updateBitboards();

    /*
     * Make a move in the position.
     * manipulate the bitboards to make whatever move we want
     */
    template<bool isEngine>
    void makeMove(Move& move)
    {
        // remove the piece we are moving
        pieces[move.moved] ^= toBoard(move.from);

        // if we captured something
        if (move.captured != NONE)
        {
            // if we captured en-passant
            if (move.type == EN_PASSANT)
            {
                // remove the pawn we captured
                pieces[isEngine ? PLAYER_PAWN : ENGINE_PAWN] ^= isEngine ? north(enPassantCapture)
                                                                         : south(enPassantCapture);
            }
            else
            {
                // remove the piece we captured
                pieces[move.captured] ^= toBoard(move.to);
            }

            // if we captured a rook
            if (move.captured == (isEngine ? PLAYER_ROOK : ENGINE_ROOK))
            {
                if (isEngine)
                {
                    // if we captured a player rook on its initial square
                    if (move.to == A1)
                    {
                        // forbid the player from using that rook to castle
                        (ENGINE_IS_WHITE ? playerCastleKingside : playerCastleQueenside) = false;
                    }
                    // if we captured a player rook on its initial square
                    else if (move.to == H1)
                    {
                        // forbid the player from using that rook to castle
                        (ENGINE_IS_WHITE ? playerCastleQueenside : playerCastleKingside) = false;
                    }
                }
                else
                {
                    // if we captured an engine rook on its initial square
                    if (move.to == H8)
                    {
                        // forbid the engine from using that rook to castle
                        (ENGINE_IS_WHITE ? engineCastleQueenside : engineCastleKingside) = false;
                    }
                    // if we captured an engine rook on its initial square
                    else if (move.to == A8)
                    {
                        // forbid the engine from using that rook to castle
                        (ENGINE_IS_WHITE ? engineCastleKingside : engineCastleQueenside) = false;
                    }
                }
            }
        }
        // reset en passant capture because it is now illegal if we did not just play it
        enPassantCapture = EMPTY_BITBOARD;

        // if we moved a pawn without a capture
        if (move.moved == PLAYER_PAWN || move.moved == ENGINE_PAWN && move.captured == NONE)
        {
            // if we made a double push pawn move
            if (abs(move.to - move.from) > 8)
            {
                enPassantCapture = toBoard(isEngine ? north(move.to) : south(move.to));
            }
        }

        // if we need to make a promotion
        if (move.type >= KNIGHT_PROMOTION)
        {
            // if we promoted to a knight
            if (move.type == KNIGHT_PROMOTION)
            {
                pieces[isEngine ? ENGINE_KNIGHT: PLAYER_KNIGHT] |= toBoard(move.to);
            }
            // if we promoted to a bishop
            else if (move.type == BISHOP_PROMOTION)
            {
                pieces[isEngine ? ENGINE_BISHOP: PLAYER_BISHOP] |= toBoard(move.to);
            }
            // if we promoted to a rook
            else if (move.type == ROOK_PROMOTION)
            {
                pieces[isEngine ? ENGINE_ROOK: PLAYER_ROOK] |= toBoard(move.to);
            }
            // if we promoted to a queen
            else if (move.type == QUEEN_PROMOTION)
            {
                pieces[isEngine ? ENGINE_QUEEN: PLAYER_QUEEN] |= toBoard(move.to);
            }
        }
        // if we did not make a promotion
        else
        {
            // put the piece on its new square
            pieces[move.moved] |= toBoard(move.to);
        }

        // if we moved a rook
        if (move.moved == (isEngine ? ENGINE_ROOK : PLAYER_ROOK))
        {
            if (isEngine)
            {
                // if we moved an engine rook from its initial square
                if (move.from == A8)
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? engineCastleKingside : engineCastleQueenside) = false;
                }
                // if we moved an engine rook from its initial square
                else if (move.from == H8)
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? engineCastleQueenside : engineCastleKingside) = false;
                }
            }
            else
            {
                // if we moved a player rook from its initial square
                if (move.from == A1)
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? playerCastleKingside : playerCastleQueenside) = false;
                }
                // if we moved a player rook from its initial square
                else if (move.from == H1)
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? playerCastleQueenside : playerCastleKingside) = false;
                }
            }
        }
        // if we moved a king
        else if (move.moved == (isEngine ? ENGINE_KING : PLAYER_KING))
        {
            // disallow all castling for this player
            (isEngine ? engineCastleKingside : playerCastleKingside) = false;
            (isEngine ? engineCastleQueenside : playerCastleQueenside) = false;

            // if we castled
            if (move.type == CASTLE)
            {
                // figure out which rooks we want to move
                Bitboard& rooks = pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK];
                if (isEngine)
                {
                    // figure out which way we castled
                    bool isRightCastle = (move.to == F8 || move.to == G8);
                    // remove either the right or left rook
                    rooks ^= toBoard(isRightCastle ? H8 : A8);
                    // place a rook to the right or left of the king
                    rooks |= toBoard(isRightCastle ? west(move.to) : east(move.to));
                }
                else
                {
                    bool isRightCastle = (move.to == F1 || move.to == G1);
                    // remove either the right or left rook
                    rooks ^= toBoard(isRightCastle ? H1 : A1);
                    // place a rook to the right or left of the king
                    rooks ^= toBoard(isRightCastle ? west(move.to) : east(move.to));
                }
            }
        }
        // update additional position information
        updateBitboards();
        isEngineMove = !isEngineMove;
    };

    /*
     * un-make a move on the board. This function will restore the board state
     * to the exact way it was before the given move was made, except for these things:
     * 1) castling rights
     * 2) en passant capture square
     * It does not undo these things because:
     * 1) there is no way to know if the previous move caused a loss of castling right
     * 2) there is no way to know what the en passant square should become, if we were to undo
     * because of these reasons, the en passant capture square and castling rights will be stored
     * on the call stack during the search. Hopefully, doing unmake() should still be faster than
     * copying the entire position onto the stack during the search.
     * TODO: test performance of make-search-unmake vs copy-make-search-restore
     */
    template<bool isEngine>
    void unMakeMove(Move& move)
    {
        // add the piece back to where it came from
        pieces[move.moved] ^= toBoard(move.from);

        // if we want to undo an en-passant capture
        if (move.type == EN_PASSANT)
        {
            // restore the captured pawn
            pieces[move.captured] ^= toBoard(isEngine ? north(move.to) : south(move.to));
        }
        // if we want to undo a normal capture
        else if (move.captured != NONE)
        {
            // restore captured piece
            pieces[move.captured] ^= toBoard(move.to);
        }
        // if we want to undo a promotion
        if (move.type >= KNIGHT_PROMOTION)
        {
            // remove our promoted knight
            if (move.type == KNIGHT_PROMOTION)
            {
                pieces[isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT] ^= toBoard(move.to);
            }
            // remove our promoted bishop
            else if (move.type == BISHOP_PROMOTION)
            {
                pieces[isEngine ? ENGINE_BISHOP : PLAYER_BISHOP] ^= toBoard(move.to);
            }
            // remove our promoted rook
            else if (move.type == ROOK_PROMOTION)
            {
                pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK] ^= toBoard(move.to);
            }
            // remove our promoted queen
            else if (move.type == QUEEN_PROMOTION)
            {
                pieces[isEngine ? ENGINE_QUEEN : PLAYER_QUEEN] ^= toBoard(move.to);
            }
        }
        // if we are not undoing promotion
        else
        {
            // remove the piece we moved from where it came from
            pieces[move.moved] ^= toBoard(move.to);
        }
        // if we want to undo castling
        if (move.type == CASTLE)
        {
            // we already moved the king back to where it came from.
            // we just have to un-castle the castled rook
            if (isEngine)
            {
                bool wasRightCastle = move.to == F8 || move.to == G8;
                // remove the rook from next to the king
                pieces[ENGINE_ROOK] ^= toBoard(wasRightCastle ? west(move.to) : east(move.to));
                // put the rook back to where it came from
                pieces[ENGINE_ROOK] ^= toBoard(wasRightCastle ? H8 : A8);
            }
            else
            {
                bool wasRightCastle = move.to == F1 || move.to == G1;
                // remove the rook from next to the king
                pieces[PLAYER_ROOK] ^= toBoard(wasRightCastle ? west(move.to) : east(move.to));
                // put the rook back to where it came from
                pieces[PLAYER_ROOK] ^= toBoard(wasRightCastle ? H1 : A1);

            }
        }
        // update additional position information
        updateBitboards();
        isEngineMove = !isEngineMove;

    }

private:
    void readFen(const std::string& fen);


};


#endif //DEEPENING1_POSITION_H
