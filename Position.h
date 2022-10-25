//
// Created by Joe Chrisman on 9/15/22.
//

#ifndef DEEPENING1_POSITION_H
#define DEEPENING1_POSITION_H

#include <vector>
#include <sstream>
#include "Moves.h"
#include "Zobrist.h"

/*
 * in the search, we must rapidly make and unmake moves.
 * we are able to figure out how to undo most things, like how
 * to un-capture a piece, or un-promote, or move a piece back, but there are some limitations.
 * this struct represents those limitations. we cannot undo the loss of castling rights because
 * we cannot prove the last move caused the right to be lost. we cannot undo the loss of an en-passant
 * opportunity because we cannot prove the last move caused that opportunity to disappear.
 * Because of these problems, we create copies of this struct and store them on the stack during the search.
 * then we pass the old rights to the undo function, and we can easily bring the board back
 * to its original state, including castling rights and en passant rights
 */
struct PositionRights
{
    bool playerCastleKingside;
    bool playerCastleQueenside;
    bool engineCastleKingside;
    bool engineCastleQueenside;

    // the square we can move a pawn to in order to capture en passant
    Bitboard enPassantCapture;

    // the number of half moves that have been played since the last reversible move.
    // used for enforcing the draw by fifty move rule
    int halfMoveClock;
};

class Position
{
public:
    // create a chess game from a FEN string
    Position(std::string fen);

    /*
     * a vector of bitboards of pieces. there is one bitboard for each piece type.
     * the vector is indexed using the PieceType enumeration in Moves.h
     */
    std::vector<Bitboard> pieces;

    PositionRights rights;

    bool isEngineMove;

    // the number of full moves (ex. e4 e5) have been played so far.
    // not sure how it will ever be used. But it is a part of FEN specification
    int fullMoves;

    // incrementally updated zobrist hash of the position
    Zobrist hash;

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
        MoveType moveType = getMoveType(move);
        PieceType pieceMoved = getPieceMoved(move);
        PieceType pieceCaptured = getPieceCaptured(move);
        Square squareFrom = getSquareFrom(move);
        Square squareTo = getSquareTo(move);

        rights.halfMoveClock++;

        Bitboard to = toBoard(squareTo);
        Bitboard from = toBoard(squareFrom);

        // remove the piece we are moving
        pieces[pieceMoved] ^= from;
        hash ^= SQUARE_PIECE_KEYS[squareFrom][pieceMoved];

        // if we are making an engine move remember it in the zobrist hash
        if (isEngine)
        {
            hash ^= ENGINE_TO_MOVE_KEY;
        }

        // if we captured something
        if (pieceCaptured != NONE)
        {
            // captures are irreversible moves
            rights.halfMoveClock = 0;
            // if we captured en-passant
            if (moveType == EN_PASSANT)
            {
                // calculate capture square
                Bitboard enPassant = isEngine ? north(rights.enPassantCapture) : south(rights.enPassantCapture);
                // remove the pawn we captured
                pieces[isEngine ? PLAYER_PAWN : ENGINE_PAWN] ^= enPassant;
                hash ^= SQUARE_PIECE_KEYS[toSquare(enPassant)][pieceCaptured];
            }
            else
            {
                // remove the piece we captured
                pieces[pieceCaptured] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][pieceCaptured];
                // if we captured a rook
                if (pieceCaptured == (isEngine ? PLAYER_ROOK : ENGINE_ROOK))
                {
                    if (isEngine)
                    {
                        // if we captured a player rook on its initial square
                        if (squareTo == A1 && (ENGINE_IS_WHITE ? rights.playerCastleKingside : rights.playerCastleQueenside))
                        {
                            // forbid the player from using that rook to castle
                            (ENGINE_IS_WHITE ? rights.playerCastleKingside : rights.playerCastleQueenside) = false;
                            hash ^= ENGINE_IS_WHITE ? PLAYER_CASTLE_KINGSIDE_KEY : PLAYER_CASTLE_QUEENSIDE_KEY;
                        }
                        // if we captured a player rook on its initial square
                        else if (squareTo == H1 && (ENGINE_IS_WHITE ? rights.playerCastleQueenside : rights.playerCastleKingside))
                        {
                            // forbid the player from using that rook to castle
                            (ENGINE_IS_WHITE ? rights.playerCastleQueenside : rights.playerCastleKingside) = false;
                            hash ^= ENGINE_IS_WHITE ? PLAYER_CASTLE_QUEENSIDE_KEY : PLAYER_CASTLE_KINGSIDE_KEY;
                        }
                    }
                    else
                    {
                        // if we captured an engine rook on its initial square
                        if (squareTo == H8 && (ENGINE_IS_WHITE ? rights.engineCastleQueenside : rights.engineCastleKingside))
                        {
                            // forbid the engine from using that rook to castle
                            (ENGINE_IS_WHITE ? rights.engineCastleQueenside : rights.engineCastleKingside) = false;
                            hash ^= ENGINE_IS_WHITE ? ENGINE_CASTLE_QUEENSIDE_KEY : ENGINE_CASTLE_KINGSIDE_KEY;

                        }
                            // if we captured an engine rook on its initial square
                        else if (squareTo == A8 && (ENGINE_IS_WHITE ? rights.engineCastleKingside : rights.engineCastleQueenside))
                        {
                            // forbid the engine from using that rook to castle
                            (ENGINE_IS_WHITE ? rights.engineCastleKingside : rights.engineCastleQueenside) = false;
                            hash ^= ENGINE_IS_WHITE ? ENGINE_CASTLE_KINGSIDE_KEY : ENGINE_CASTLE_QUEENSIDE_KEY;
                        }
                    }
                }
            }
        }
        // reset en passant capture because it is now illegal one move after
        if (rights.enPassantCapture)
        {
            // remove the en passant capture option
            hash ^= EN_PASSANT_KEYS[getFile(toSquare(rights.enPassantCapture))];
            rights.enPassantCapture = EMPTY_BITBOARD;
        }
        // if we moved a pawn without a capture
        if ((pieceMoved == (isEngine ? ENGINE_PAWN : PLAYER_PAWN)) && pieceCaptured == NONE)
        {
            // pawn moves are irreversible moves
            rights.halfMoveClock = 0;
            // if we made a double push pawn move
            if (abs(squareTo - squareFrom) > 8)
            {
                // remember this double push pawn move enables en passant
                rights.enPassantCapture = isEngine ? north(to) : south(to);
                hash ^= EN_PASSANT_KEYS[getFile(squareTo)];
            }
        }

        // if we need to make a promotion
        if (moveType >= KNIGHT_PROMOTION)
        {
            // promotions are irreversible moves
            rights.halfMoveClock = 0;
            if (moveType == QUEEN_PROMOTION)
            {
                pieces[isEngine ? ENGINE_QUEEN: PLAYER_QUEEN] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_QUEEN: PLAYER_QUEEN];
            }
            else if (moveType == KNIGHT_PROMOTION)
            {
                pieces[isEngine ? ENGINE_KNIGHT: PLAYER_KNIGHT] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_KNIGHT: PLAYER_KNIGHT];
            }
            else if (moveType == ROOK_PROMOTION)
            {
                pieces[isEngine ? ENGINE_ROOK: PLAYER_ROOK] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_ROOK: PLAYER_ROOK];
            }
            else if (moveType == BISHOP_PROMOTION)
            {
                pieces[isEngine ? ENGINE_BISHOP: PLAYER_BISHOP] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_BISHOP: PLAYER_BISHOP];
            }
        }
        // if we did not make a promotion
        else
        {
            // put the piece we are moving on its new square
            pieces[pieceMoved] ^= to;
            hash ^= SQUARE_PIECE_KEYS[squareTo][pieceMoved];
        }
        // if we moved a rook
        if (pieceMoved == (isEngine ? ENGINE_ROOK : PLAYER_ROOK))
        {
            if (isEngine)
            {
                // if we moved an engine rook from its initial square
                if (squareFrom == A8 && (ENGINE_IS_WHITE ? rights.engineCastleKingside : rights.engineCastleQueenside))
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? rights.engineCastleKingside : rights.engineCastleQueenside) = false;
                    hash ^= ENGINE_IS_WHITE ? ENGINE_CASTLE_KINGSIDE_KEY : ENGINE_CASTLE_QUEENSIDE_KEY;
                }
                // if we moved an engine rook from its initial square
                else if (squareFrom == H8 && (ENGINE_IS_WHITE ? rights.engineCastleQueenside : rights.engineCastleKingside))
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? rights.engineCastleQueenside : rights.engineCastleKingside) = false;
                    hash ^= ENGINE_IS_WHITE ? ENGINE_CASTLE_QUEENSIDE_KEY : ENGINE_CASTLE_KINGSIDE_KEY;
                }
            }
            else
            {
                // if we moved a player rook from its initial square
                if (squareFrom == A1 && (ENGINE_IS_WHITE ? rights.playerCastleKingside : rights.playerCastleQueenside))
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? rights.playerCastleKingside : rights.playerCastleQueenside) = false;
                    hash ^= ENGINE_IS_WHITE ? PLAYER_CASTLE_KINGSIDE_KEY : PLAYER_CASTLE_QUEENSIDE_KEY;
                }
                // if we moved a player rook from its initial square
                else if (squareFrom == H1 && (ENGINE_IS_WHITE ? rights.playerCastleQueenside : rights.playerCastleKingside))
                {
                    // forbid the engine from using that rook to castle
                    (ENGINE_IS_WHITE ? rights.playerCastleQueenside : rights.playerCastleKingside) = false;
                    hash ^= ENGINE_IS_WHITE ? PLAYER_CASTLE_QUEENSIDE_KEY : PLAYER_CASTLE_KINGSIDE_KEY;
                }
            }
        }
        // if we moved a king
        else if (pieceMoved == (isEngine ? ENGINE_KING : PLAYER_KING))
        {
            // disallow all castling for this player
            if (isEngine ? rights.engineCastleKingside : rights.playerCastleKingside)
            {
                (isEngine ? rights.engineCastleKingside : rights.playerCastleKingside) = false;
                hash ^= isEngine ? ENGINE_CASTLE_KINGSIDE_KEY : PLAYER_CASTLE_KINGSIDE_KEY;
            }
            if (isEngine ? rights.engineCastleQueenside : rights.playerCastleQueenside)
            {
                (isEngine ? rights.engineCastleQueenside : rights.playerCastleQueenside) = false;
                hash ^= isEngine ? ENGINE_CASTLE_QUEENSIDE_KEY : PLAYER_CASTLE_QUEENSIDE_KEY;
            }

            // if we castled
            if (moveType == CASTLE)
            {
                if (isEngine)
                {
                    // figure out which way we castled
                    bool isRightCastle = (squareTo == F8 || squareTo == G8);
                    // remove either the right or left rook
                    pieces[ENGINE_ROOK] ^= toBoard(isRightCastle ? H8 : A8);
                    hash ^= SQUARE_PIECE_KEYS[isRightCastle ? H8 : A8][ENGINE_ROOK];
                    // place a rook to the right or left of the king
                    pieces[ENGINE_ROOK] ^= isRightCastle ? west(to) : east(to);
                    hash ^= SQUARE_PIECE_KEYS[isRightCastle ? west(squareTo) : east(squareTo)][ENGINE_ROOK];
                }
                else
                {
                    bool isRightCastle = (squareTo == F1 || squareTo == G1);
                    // remove either the right or left rook
                    pieces[PLAYER_ROOK] ^= toBoard(isRightCastle ? H1 : A1);
                    hash ^= SQUARE_PIECE_KEYS[isRightCastle ? H1 : A1][PLAYER_ROOK];

                    // place a rook to the right or left of the king
                    pieces[PLAYER_ROOK] ^= isRightCastle ? west(to) : east(to);
                    hash ^= SQUARE_PIECE_KEYS[isRightCastle ? west(squareTo) : east(squareTo)][PLAYER_ROOK];
                }
            }
        }
        // update additional position information
        updateBitboards();
        isEngineMove = !isEngineMove;
    };

    /*
     * un-make a move on the board. This function will restore the board state
     * to the exact way it was before the given move was made. PreviousRights is
     * a copy of the original position's rights. this makes it possible to undo
     * things like losing the right to castle.
     */
    template<bool isEngine>
    void unMakeMove(Move& move, PositionRights& previousRights)
    {
        MoveType moveType = getMoveType(move);
        PieceType pieceMoved = getPieceMoved(move);
        PieceType pieceCaptured = getPieceCaptured(move);
        Square squareFrom = getSquareFrom(move);
        Square squareTo = getSquareTo(move);

        Bitboard from = toBoard(squareFrom);
        Bitboard to = toBoard(squareTo);

        // add the piece back to where it came from
        pieces[pieceMoved] ^= from;
        hash ^= SQUARE_PIECE_KEYS[squareFrom][pieceMoved];

        // if we are un making an engine move forget it in the zobrist hash
        if (isEngine)
        {
            hash ^= ENGINE_TO_MOVE_KEY;
        }

        // if we lost the right to castle during the previous move, undo the loss of castling rights.
        // also add the appropriate castling key back to the zobrist hash
        if (rights.playerCastleKingside != previousRights.playerCastleKingside)
        {
            rights.playerCastleKingside = previousRights.playerCastleKingside;
            hash ^= PLAYER_CASTLE_KINGSIDE_KEY;
        }
        if (rights.playerCastleQueenside != previousRights.playerCastleQueenside)
        {
            rights.playerCastleQueenside = previousRights.playerCastleQueenside;
            hash ^= PLAYER_CASTLE_QUEENSIDE_KEY;
        }
        if (rights.engineCastleKingside != previousRights.engineCastleKingside)
        {
            rights.engineCastleKingside = previousRights.engineCastleKingside;
            hash ^= ENGINE_CASTLE_KINGSIDE_KEY;
        }
        if (rights.engineCastleQueenside != previousRights.engineCastleQueenside)
        {
            rights.engineCastleQueenside = previousRights.engineCastleQueenside;
            hash ^= ENGINE_CASTLE_QUEENSIDE_KEY;
        }
        // if our en passant rights changed during the last move
        if (rights.enPassantCapture != previousRights.enPassantCapture)
        {
            // if our last move created an en passant opportunity, remove it from the hash
            if (rights.enPassantCapture)
            {
                hash ^= EN_PASSANT_KEYS[getFile(toSquare(rights.enPassantCapture))];
            }
            // if our last move erased an en passant opportunity, bring it back to the hash
            if (previousRights.enPassantCapture)
            {
                hash ^= EN_PASSANT_KEYS[getFile(toSquare(previousRights.enPassantCapture))];
            }
            rights.enPassantCapture = previousRights.enPassantCapture;
        }
        rights.halfMoveClock = previousRights.halfMoveClock;

        // if we want to undo an en-passant capture
        if (moveType == EN_PASSANT)
        {
            // restore the captured pawn
            pieces[pieceCaptured] ^= isEngine ? north(to) : south(to);
            hash ^= SQUARE_PIECE_KEYS[isEngine ? north(squareTo) : south(squareTo)][pieceCaptured];
        }
        // if we want to undo a normal capture
        else if (pieceCaptured != NONE)
        {
            // restore captured piece
            pieces[pieceCaptured] ^= to;
            hash ^= SQUARE_PIECE_KEYS[squareTo][pieceCaptured];
        }
        // if we want to undo a promotion
        if (moveType >= KNIGHT_PROMOTION)
        {
            if (moveType == QUEEN_PROMOTION)
            {
                pieces[isEngine ? ENGINE_QUEEN : PLAYER_QUEEN] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_QUEEN : PLAYER_QUEEN];
            }
            else if (moveType == KNIGHT_PROMOTION)
            {
                pieces[isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT];
            }
            else if (moveType == ROOK_PROMOTION)
            {
                pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_ROOK : PLAYER_ROOK];
            }
            else if (moveType == BISHOP_PROMOTION)
            {
                pieces[isEngine ? ENGINE_BISHOP : PLAYER_BISHOP] ^= to;
                hash ^= SQUARE_PIECE_KEYS[squareTo][isEngine ? ENGINE_BISHOP : PLAYER_BISHOP];
            }
        }
        // if we are not undoing promotion
        else
        {
            // remove the piece we moved from where it went to
            pieces[pieceMoved] ^= to;
            hash ^= SQUARE_PIECE_KEYS[squareTo][pieceMoved];
            // if we want to undo castling
            if (moveType == CASTLE)
            {
                // we already moved the king back to where it came from.
                // we just have to un-castle the castled rook
                if (isEngine)
                {
                    bool wasRightCastle = squareTo == F8 || squareTo == G8;
                    // remove the rook from next to the king
                    pieces[ENGINE_ROOK] ^= wasRightCastle ? west(to) : east(to);
                    hash ^= SQUARE_PIECE_KEYS[wasRightCastle ? west(squareTo) : east(squareTo)][ENGINE_ROOK];
                    // put the rook back to where it came from
                    pieces[ENGINE_ROOK] ^= toBoard(wasRightCastle ? H8 : A8);
                    hash ^= SQUARE_PIECE_KEYS[wasRightCastle ? H8 : A8][ENGINE_ROOK];
                }
                else
                {
                    bool wasRightCastle = squareTo == F1 || squareTo == G1;
                    // remove the rook from next to the king
                    pieces[PLAYER_ROOK] ^= wasRightCastle ? west(to) : east(to);
                    hash ^= SQUARE_PIECE_KEYS[wasRightCastle ? west(squareTo) : east(squareTo)][PLAYER_ROOK];
                    // put the rook back to where it came from
                    pieces[PLAYER_ROOK] ^= toBoard(wasRightCastle ? H1 : A1);
                    hash ^= SQUARE_PIECE_KEYS[wasRightCastle ? H1 : A1][PLAYER_ROOK];
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
