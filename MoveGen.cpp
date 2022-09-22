//
// Created by Joe Chrisman on 9/17/22.
//

#include "MoveGen.h"

MoveGen::MoveGen() : position(ENGINE_IS_WHITE ?
                              ENGINE_WHITE_FEN :
                              ENGINE_BLACK_FEN)
{
}

void MoveGen::genEngineMoves()
{
    moveList.clear();
    updateBitboards();
    updateSafeSquares<true>();

    genPawnMoves<true, true>();
    genKnightMoves<true, true>();
    genKingMoves<true, true>();
    genRookMoves<true, true>();
    genBishopMoves<true, true>();
    genQueenMoves<true, true>();
}

void MoveGen::genPlayerMoves()
{
    moveList.clear();
    updateBitboards();
    updateSafeSquares<false>();

    genPawnMoves<false, true>();
    genKnightMoves<false, true>();
    genKingMoves<false, true>();
    genRookMoves<false, true>();
    genBishopMoves<false, true>();
    genQueenMoves<false, true>();
}

void MoveGen::genEngineCaptures()
{
    moveList.clear();
    updateBitboards();
    updateSafeSquares<true>();

    genPawnMoves<true, false>();
    genKnightMoves<true, false>();
    genKingMoves<true, false>();
    genRookMoves<true, false>();
    genBishopMoves<true, false>();
    genQueenMoves<true, false>();
}

void MoveGen::genPlayerCaptures()
{
    moveList.clear();
    updateBitboards();
    updateSafeSquares<false>();

    genPawnMoves<false, false>();
    genKnightMoves<false, false>();
    genKingMoves<false, false>();
    genRookMoves<false, false>();
    genBishopMoves<false, false>();
    genQueenMoves<false, false>();
}

void MoveGen::updateBitboards()
{
    safeSquares = FULL_BITBOARD;

    // figure out what pieces the engine can capture
    playerPieces = position.pieces[PLAYER_PAWN] |
                    position.pieces[PLAYER_KNIGHT] |
                    position.pieces[PLAYER_BISHOP] |
                    position.pieces[PLAYER_ROOK] |
                    position.pieces[PLAYER_QUEEN] |
                    position.pieces[PLAYER_KING];

    // figure out what pieces the player can capture
    enginePieces = position.pieces[ENGINE_PAWN] |
                    position.pieces[ENGINE_KNIGHT] |
                    position.pieces[ENGINE_BISHOP] |
                    position.pieces[ENGINE_ROOK] |
                    position.pieces[ENGINE_QUEEN] |
                    position.pieces[ENGINE_KING];

    occupied = enginePieces | playerPieces;
    // now figure out what empty squares we can move to
    empties = ~occupied;

    playerMovable = enginePieces | empties;
    engineMovable = playerPieces | empties;

}

template<bool isEngine>
void MoveGen::updateSafeSquares()
{
    Square king = toSquare(position.pieces[isEngine ? ENGINE_KING : PLAYER_KING]);

    // scan outward in cardinal and ordinal directions from the king
    Bitboard cardinalAttacks = getSlidingMoves<true>(king);
    Bitboard ordinalAttacks = getSlidingMoves<false>(king);

    // isolate the attacking sliding pieces
    Bitboard cardinalAttackers = cardinalAttacks &
            (isEngine ? position.pieces[PLAYER_QUEEN] | position.pieces[PLAYER_ROOK]
                      : position.pieces[ENGINE_QUEEN] | position.pieces[ENGINE_ROOK]);
    Bitboard ordinalAttackers = ordinalAttacks &
            (isEngine ? position.pieces[PLAYER_QUEEN] | position.pieces[PLAYER_BISHOP]
                      : position.pieces[ENGINE_QUEEN] | position.pieces[ENGINE_BISHOP]);

    // add bishops, rooks, and queens attacking the king
    Bitboard attackers = cardinalAttackers | ordinalAttackers;
    // add knights attacking the king
    attackers |= KNIGHT_MOVES[king] & position.pieces[isEngine ? PLAYER_KNIGHT : ENGINE_KNIGHT];
    // add pawns attacking the king
    if (isEngine)
    {
        attackers |= ENGINE_PAWN_CAPTURES[king] & position.pieces[PLAYER_PAWN];
    }
    else
    {
        attackers |= PLAYER_PAWN_CAPTURES[king] & position.pieces[ENGINE_PAWN];
    }
    if (attackers)
    {
        // if there is one piece attacking the king
        if (countPieces(attackers) == 1)
        {
            // if the king is being attacked by a rook or queen
            if (cardinalAttackers)
            {
                // any square that blocks the attacking piece is safe
                safeSquares = cardinalAttacks & getSlidingMoves<true>(toSquare(attackers));
                // any square that captures the checking piece is safe
                safeSquares |= attackers;
            }
            // if the king is being attacked by a bishop or queen
            else if (ordinalAttackers)
            {
                // any square that blocks the attacking piece is safe
                safeSquares = ordinalAttacks & getSlidingMoves<false>(toSquare(attackers));
                // any square that captures the checking piece is safe
                safeSquares |= attackers;
            }
            // if the king is being attacked by a pawn or knight
            else
            {
                // we can't block an attack given by a pawn or knight, they are not sliding pieces.
                // we can only capture this piece to get out of check
                safeSquares = attackers;
            }
        }
        // if there are multiple pieces attacking the king
        else
        {
            // there is no square we can move to that blocks a double check.
            // also, we can not capture two checking pieces at once.
            // so there are no safe squares, and the king must be moved in this case
            safeSquares = EMPTY_BITBOARD;
        }
    }
    // if there is nothing attacking the king
    else
    {
        // we can move to any square without leaving the king in check
        safeSquares = FULL_BITBOARD;
    }

}

/*
 * use the magic bitboards defined in Magic.h to calculate a small hash.
 * this hash is derived by bit-shifting the result of a multiplication
 * between the blocking pieces and a pre-calculated magic number.
 * we can then use this hash to lookup the correct attack set in an attack table.
 *
 * This function returns moves for a sliding piece given its square
 */
template<bool isCardinal>
Bitboard MoveGen::getSlidingMoves(Square from)
{
    if (isCardinal)
    {
        return CARDINAL_ATTACKS
            [from]
            [(CARDINAL_BLOCKERS[from] & occupied) * CARDINAL_MAGICS[from] >> 52];
    }
    return ORDINAL_ATTACKS
        [from]
        [(ORDINAL_BLOCKERS[from] & occupied) * ORDINAL_MAGICS[from] >> 55];
}

template<bool isEngine, bool quiets>
void MoveGen::genKnightMoves()
{
    Bitboard knights = position.pieces[isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT];
    while (knights)
    {
        Square from = popFirstPiece(knights);
        Bitboard moves = KNIGHT_MOVES[from] & safeSquares;

        // moves and captures
        if (quiets)
        {
            moves &= (isEngine ? engineMovable : playerMovable);
        }
        // captures only
        else
        {
            moves &= (isEngine ? playerPieces : enginePieces);
        }

        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(Move{
                from,
                to,
                isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT,
                position.getPiece(to)
            });
        }
    }
}

template<bool isEngine, bool quiets>
void MoveGen::genKingMoves()
{
    Square from = toSquare(position.pieces[isEngine ? ENGINE_KING : PLAYER_KING]);
    Bitboard moves = KING_MOVES[from];
    // all moves
    if (quiets)
    {
        moves &= (isEngine ? engineMovable : playerMovable);
    }
    // only captures
    else
    {
        moves &= (isEngine ? playerPieces : enginePieces);
    }

    while (moves)
    {
        Square to = popFirstPiece(moves);
        moveList.push_back(Move{
            from,
            to,
            isEngine ? ENGINE_KING : PLAYER_KING,
            position.getPiece(to)
        });
    }
}

template<bool isEngine, bool quiets>
void MoveGen::genPawnMoves()
{
    Bitboard pawns = position.pieces[isEngine ? ENGINE_PAWN : PLAYER_PAWN];
    // generate non-captures
    if (quiets)
    {
        // generate one square pawn pushes
        Bitboard pushed = (isEngine ? south(pawns) : north(pawns)) & empties;
        // generate two square pawn pushes
        Bitboard pushed2 = (isEngine ? south(pushed) : north(pushed)) & empties;
        // make sure two square pawn push comes from initial rank
        pushed2 &= isEngine ? RANK_4 : RANK_3;
        // throw away pawn moves that leave the king in check
        pushed &= safeSquares;
        pushed2 &= safeSquares;

        // add one square pawn moves
        while (pushed)
        {
            Square to = popFirstPiece(pushed);
            Square from = isEngine ? north(to) : south(to);

            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    NONE});
        }
        // add two square pawn moves
        while (pushed2)
        {
            Square to = popFirstPiece(pushed2);
            Square from = isEngine ? north(north(to)) : south(south(to));

            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    NONE});
        }
    }

    // generate captures
    while (pawns)
    {
        Square from = popFirstPiece(pawns);
        Bitboard captures = isEngine ? ENGINE_PAWN_CAPTURES[from] : PLAYER_PAWN_CAPTURES[from];
        captures &= (isEngine ? playerPieces : enginePieces);
        // throw away pawn captures that leave the king in check
        captures &= safeSquares;
        while (captures)
        {
            Square to = popFirstPiece(captures);
            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    position.getPiece(to)});
        }
    }
}

template<bool isEngine, bool quiets>
void MoveGen::genRookMoves()
{
    Bitboard rooks = position.pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK];
    while (rooks)
    {
        Square from = popFirstPiece(rooks);
        Bitboard moves = getSlidingMoves<true>(from) & safeSquares;
        // all moves
        if (quiets)
        {
            moves &= isEngine ? engineMovable : playerMovable;
        }
        // only captures
        else
        {
            moves &= isEngine ? playerPieces : enginePieces;
        }
        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(Move{
                from,
                to,
                isEngine ? ENGINE_ROOK : PLAYER_ROOK,
                position.getPiece(to)
            });
        }
    }
}

template<bool isEngine, bool quiets>
void MoveGen::genBishopMoves()
{
    Bitboard bishops = position.pieces[isEngine ? ENGINE_BISHOP : PLAYER_BISHOP];
    while (bishops)
    {
        Square from = popFirstPiece(bishops);
        Bitboard moves = getSlidingMoves<false>(from) & safeSquares;
        // all moves
        if (quiets)
        {
            moves &= isEngine ? engineMovable : playerMovable;
        }
        // only captures
        else
        {
            moves &= isEngine ? playerPieces : enginePieces;
        }
        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_BISHOP : PLAYER_BISHOP,
                    position.getPiece(to)
            });
        }
    }
}

template<bool isEngine, bool quiets>
void MoveGen::genQueenMoves()
{
    Bitboard queens = position.pieces[isEngine ? ENGINE_QUEEN : PLAYER_QUEEN];
    while (queens)
    {
        Square from = popFirstPiece(queens);
        Bitboard moves = getSlidingMoves<true>(from) | getSlidingMoves<false>(from);
        // throw away queen moves that leave the king in check
        moves &= safeSquares;
        // all moves
        if (quiets)
        {
            moves &= isEngine ? engineMovable : playerMovable;
        }
        // only captures
        else
        {
            moves &= isEngine ? playerPieces : enginePieces;
        }
        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(Move{
                from,
                to,
                isEngine ? ENGINE_QUEEN : PLAYER_QUEEN,
                position.getPiece(to)
            });
        }
    }
}
