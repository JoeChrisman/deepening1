//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_POSITION_H
#define DEEPENING1_POSITION_H

#include <vector>
#include <string>
#include "Bitboards.h"

enum MoveType
{
    NORMAL,
    CASTLE,
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
    Position(const std::string& fen);
    Piece getPiece(Square square);

    /*
     * a vector of bitboards of pieces. there is one bitboard for each piece type.
     * the vector is indexed using the Piece enumeration in Constants.h
     */
    std::vector<Bitboard> pieces;

    bool engineCastleKingside;
    bool engineCastleQueenside;
    bool playerCastleKingside;
    bool playerCastleQueenside;

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
        // put the piece on its new square
        pieces[move.moved] |= toBoard(move.to);

        // if we captured something
        if (move.captured != NONE)
        {
            // remove the piece we captured
            pieces[move.captured] ^= toBoard(move.to);

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
                    rooks |= toBoard(isRightCastle ? west(move.to) : east(move.to));

                }
            }
        }
        updateBitboards();
    };

private:


};


#endif //DEEPENING1_POSITION_H
