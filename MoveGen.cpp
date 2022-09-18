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
    genPawnMoves<true, true>();
    genKnightMoves<true, true>();
}

void MoveGen::genPlayerMoves()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<false, true>();
    genKnightMoves<false, true>();
}

void MoveGen::genEngineCaptures()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<true, false>();
    genKnightMoves<true, false>();

}

void MoveGen::genPlayerCaptures()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<false, false>();
    genKnightMoves<false, false>();

}

void MoveGen::updateBitboards()
{
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

    // now figure out what empty squares we can move to
    empties = ~(enginePieces | playerPieces);

    playerMovable = enginePieces | empties;
    engineMovable = playerPieces | empties;

}

template<bool isEngine, bool quiets>
void MoveGen::genKnightMoves()
{
    Bitboard knights = position.pieces[isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT];

    while (knights)
    {
        Square from = popFirstPiece(knights);
        Bitboard moves;

        // moves and captures
        if (quiets)
        {
            moves = KNIGHT_MOVES[from] & (isEngine ? engineMovable : playerMovable);
        }
        // captures only
        else
        {
            moves = KNIGHT_MOVES[from] & (isEngine ? playerPieces : enginePieces);
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
void MoveGen::genPawnMoves()
{
    Bitboard pawns = position.pieces[isEngine ? ENGINE_PAWN : PLAYER_PAWN];

    Square from;
    Square to;

    if (quiets)
    {
        // generate one square pawn pushes
        Bitboard pushed = (isEngine ? south(pawns) : north(pawns)) & empties;
        // generate two square pawn pushes
        Bitboard pushed2 = (isEngine ? south(pushed) : north(pushed)) & empties;
        pushed2 &= isEngine ? RANK_4 : RANK_3;

        // add one square pawn moves
        while (pushed)
        {
            to = popFirstPiece(pushed);
            from = isEngine ? north(to) : south(to);

            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    NONE});
        }
        // add two square pawn moves
        while (pushed2)
        {
            to = popFirstPiece(pushed2);
            from = isEngine ? north(north(to)) : south(south(to));

            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    NONE});
        }
    }

    // add capture moves
    while (pawns)
    {
        from = popFirstPiece(pawns);
        Bitboard captures = isEngine ? ENGINE_PAWN_CAPTURES[from] : PLAYER_PAWN_CAPTURES[from];
        captures &= (isEngine ? playerPieces : enginePieces);
        while (captures)
        {
            to = popFirstPiece(captures);
            moveList.push_back(Move{
                    from,
                    to,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    position.getPiece(to)});
        }
    }
    
}
