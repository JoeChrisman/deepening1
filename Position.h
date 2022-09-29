//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_POSITION_H
#define DEEPENING1_POSITION_H

#include <vector>
#include <sstream>
#include "Squares.h"

class Position
{
public:

    // create a chess game from a FEN string
    Position(std::string fen);

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

    // a negative value when the player is winning
    // a positive value when the engine is winning
    int materialScore;

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
     * get a piece type given a square
     * if the square does not have a piece, NONE will be returned.
     */
    PieceType getPiece(Square square);

    /*
     * get a piece type given a square and piece side
     * if the square does not have a piece, NONE will be returned.
     */
    template<bool isEngine>
    PieceType getPiece(Square square)
    {
        Bitboard board = toBoard(square);

        for (int piece = isEngine ? ENGINE_PAWN : PLAYER_PAWN;
             piece <= (isEngine ? ENGINE_KING : PLAYER_KING);
             piece++)
        {
            if (pieces[piece] & board)
            {
                return (PieceType)piece;
            }
        }
        return NONE;
    }

    /*
     * Make a move in the position.
     * manipulate the bitboards to make whatever move we want,
     * based on the given move struct. this function does not
     * increment the full move count or half move clock, but it does
     * reset the half move clock when an irreversible move is made
     */
    template<bool isEngine>
    void makeMove(Move& move)
    {
        Bitboard to = toBoard(move.to);
        Bitboard from = toBoard(move.from);

        // remove the piece we are moving
        pieces[move.moved] ^= from;
        // update material score if we captured a piece
        materialScore += isEngine ? PIECE_SCORES[move.captured]
                                  : -PIECE_SCORES[move.captured];

        // if we captured something
        if (move.captured != NONE)
        {
            // captures are irreversible moves
            halfMoveClock = 0;
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
                pieces[move.captured] ^= to;
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
        if ((move.moved == (isEngine ? ENGINE_PAWN : PLAYER_PAWN)) && move.captured == NONE)
        {
            // pawn moves are irreversible moves
            halfMoveClock = 0;
            // if we made a double push pawn move
            if (abs(move.to - move.from) > 8)
            {
                // remember this double push pawn move enables en passant
                enPassantCapture = isEngine ? north(to) : south(to);
            }
        }

        // if we need to make a promotion
        if (move.type >= KNIGHT_PROMOTION)
        {
            // promotions are irreversible moves
            halfMoveClock = 0;
            // if we promoted to a queen
            if (move.type == QUEEN_PROMOTION)
            {
                pieces[isEngine ? ENGINE_QUEEN: PLAYER_QUEEN] ^= to;
                materialScore += isEngine ? PIECE_SCORES[ENGINE_QUEEN]
                                          : -PIECE_SCORES[ENGINE_QUEEN];
            }
            // if we promoted to a knight
            else if (move.type == KNIGHT_PROMOTION)
            {
                pieces[isEngine ? ENGINE_KNIGHT: PLAYER_KNIGHT] ^= to;
                materialScore += isEngine ? PIECE_SCORES[ENGINE_KNIGHT]
                                          : -PIECE_SCORES[ENGINE_KNIGHT];
            }
            // if we promoted to a rook
            else if (move.type == ROOK_PROMOTION)
            {
                pieces[isEngine ? ENGINE_ROOK: PLAYER_ROOK] ^= to;
                materialScore += isEngine ? PIECE_SCORES[ENGINE_ROOK]
                                          : -PIECE_SCORES[ENGINE_ROOK];
            }
            // if we promoted to a bishop
            else if (move.type == BISHOP_PROMOTION)
            {
                pieces[isEngine ? ENGINE_BISHOP: PLAYER_BISHOP] ^= to;
                materialScore += isEngine ? PIECE_SCORES[ENGINE_BISHOP]
                                          : -PIECE_SCORES[ENGINE_BISHOP];
            }
        }
        // if we did not make a promotion
        else
        {
            // put the piece we are moving on its new square
            pieces[move.moved] ^= to;
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
                // figure out which rook we want to move
                Bitboard& rooks = pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK];
                if (isEngine)
                {
                    // figure out which way we castled
                    bool isRightCastle = (move.to == F8 || move.to == G8);
                    // remove either the right or left rook
                    rooks ^= toBoard(isRightCastle ? H8 : A8);
                    // place a rook to the right or left of the king
                    rooks ^= isRightCastle ? west(to) : east(to);
                }
                else
                {
                    bool isRightCastle = (move.to == F1 || move.to == G1);
                    // remove either the right or left rook
                    rooks ^= toBoard(isRightCastle ? H1 : A1);
                    // place a rook to the right or left of the king
                    rooks ^= isRightCastle ? west(to) : east(to);
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
     * 3) half move clock for 50 move rule
     * these things will have to be stored on the call stack during the search
     */
    template<bool isEngine>
    void unMakeMove(Move& move)
    {
        Bitboard from = toBoard(move.from);
        Bitboard to = toBoard(move.to);

        // add the piece back to where it came from
        pieces[move.moved] ^= from;
        // update material score if we are un-capturing a piece
        materialScore -= isEngine ? PIECE_SCORES[move.captured]
                                  : -PIECE_SCORES[move.captured];

        // if we want to undo an en-passant capture
        if (move.type == EN_PASSANT)
        {
            // restore the captured pawn
            pieces[move.captured] ^= isEngine ? north(to) : south(to);
        }
        // if we want to undo a normal capture
        else if (move.captured != NONE)
        {
            // restore captured piece
            pieces[move.captured] ^= to;
        }
        // if we want to undo a promotion
        if (move.type >= KNIGHT_PROMOTION)
        {
            // remove our promoted queen
            if (move.type == QUEEN_PROMOTION)
            {
                pieces[isEngine ? ENGINE_QUEEN : PLAYER_QUEEN] ^= to;
                materialScore -= isEngine ? PIECE_SCORES[ENGINE_QUEEN]
                                          : -PIECE_SCORES[ENGINE_QUEEN];
            }
            // remove our promoted knight
            else if (move.type == KNIGHT_PROMOTION)
            {
                pieces[isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT] ^= to;
                materialScore -= isEngine ? PIECE_SCORES[ENGINE_KNIGHT]
                                          : -PIECE_SCORES[ENGINE_KNIGHT];
            }
            // remove our promoted rook
            else if (move.type == ROOK_PROMOTION)
            {
                pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK] ^= to;
                materialScore -= isEngine ? PIECE_SCORES[ENGINE_ROOK]
                                          : -PIECE_SCORES[ENGINE_ROOK];
            }
            // remove our promoted bishop
            else if (move.type == BISHOP_PROMOTION)
            {
                pieces[isEngine ? ENGINE_BISHOP : PLAYER_BISHOP] ^= to;
                materialScore -= isEngine ? PIECE_SCORES[ENGINE_BISHOP]
                                          : -PIECE_SCORES[ENGINE_BISHOP];
            }
        }
        // if we are not undoing promotion
        else
        {
            // remove the piece we moved from where it came from
            pieces[move.moved] ^= to;
            // if we want to undo castling
            if (move.type == CASTLE)
            {
                // we already moved the king back to where it came from.
                // we just have to un-castle the castled rook
                if (isEngine)
                {
                    bool wasRightCastle = move.to == F8 || move.to == G8;
                    // remove the rook from next to the king
                    pieces[ENGINE_ROOK] ^= wasRightCastle ? west(to) : east(to);
                    // put the rook back to where it came from
                    pieces[ENGINE_ROOK] ^= toBoard(wasRightCastle ? H8 : A8);
                }
                else
                {
                    bool wasRightCastle = move.to == F1 || move.to == G1;
                    // remove the rook from next to the king
                    pieces[PLAYER_ROOK] ^= wasRightCastle ? west(to) : east(to);
                    // put the rook back to where it came from
                    pieces[PLAYER_ROOK] ^= toBoard(wasRightCastle ? H1 : A1);

                }
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
