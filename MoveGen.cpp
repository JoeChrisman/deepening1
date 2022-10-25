//
// Created by Joe Chrisman on 9/17/22.
//

#include "MoveGen.h"

MoveGen::MoveGen(Position& _position) :
position(_position)
{
}

void MoveGen::genEngineMoves()
{
    moveList.clear();
    updateSafeSquares<true>();
    updateResolverSquares<true>();
    updatePins<true, true>();
    updatePins<true, false>();

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
    updateSafeSquares<false>();
    updateResolverSquares<false>();
    updatePins<false, true>();
    updatePins<false, false>();

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
    updateSafeSquares<true>();
    updateResolverSquares<true>();
    updatePins<true, true>();
    updatePins<true, false>();

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
    updateSafeSquares<false>();
    updateResolverSquares<false>();
    updatePins<false, true>();
    updatePins<false, false>();

    genPawnMoves<false, false>();
    genKnightMoves<false, false>();
    genKingMoves<false, false>();
    genRookMoves<false, false>();
    genBishopMoves<false, false>();
    genQueenMoves<false, false>();
}

// generate all four promotion types for a pawn
template<bool isEngine>
void MoveGen::genPromotions(Square from, Square to, PieceType captured)
{
    for (int promotionType = KNIGHT_PROMOTION; promotionType <= QUEEN_PROMOTION; promotionType++)
    {
        moveList.push_back(makeMove(
            (MoveType)promotionType,
            isEngine ? ENGINE_PAWN : PLAYER_PAWN,
            captured,
            from,
            to));
    }
}

/*
 * we need a way to ensure pieces do not break absolute pins.
 * an absolute pin is when a piece has restricted movement
 * because some (or all) of its moves would allow an enemy attack on the king.
 * To keep track of pins, we have one bitboard for each pin direction, and
 * bits along the pin are set in these bitboards. we can easily filter moves this way
 */
template<bool isEngine, bool isCardinal>
void MoveGen::updatePins()
{
    Square king = toSquare(position.pieces[isEngine ? ENGINE_KING : PLAYER_KING]);

    // clear whatever pins may have existed last turn
    (isCardinal ? cardinalPins : ordinalPins) = EMPTY_BITBOARD;

    // scan outward from the king
    Bitboard pinned = getSlidingMoves<isCardinal>(king);
    // isolate friendly pieces along the king's outward rays
    pinned &= (isEngine ? position.enginePieces : position.playerPieces);

    // remove the pieces we suspect are pinned
    position.occupied ^= pinned;

    // scan outward from the king again, but this time,
    // the pieces that might be pinned have been removed
    Bitboard pins = getSlidingMoves<isCardinal>(king);

    // isolate sliding enemy pieces along the king's outwards rays
    Bitboard pinners = pins;
    if (isCardinal)
    {
        pinners &= position.pieces[isEngine ? PLAYER_ROOK : ENGINE_ROOK] |
                   position.pieces[isEngine ? PLAYER_QUEEN : ENGINE_QUEEN];
    }
    else
    {
        pinners &= position.pieces[isEngine ? PLAYER_BISHOP : ENGINE_BISHOP] |
                   position.pieces[isEngine ? PLAYER_QUEEN : ENGINE_QUEEN];
    }
    while (pinners)
    {
        Square pinner = popFirstPiece(pinners);

        // find the attack rays of the pinning piece
        Bitboard pin = getSlidingMoves<isCardinal>(pinner);
        // isolate the pins. pin is all the attack rays of the pinning piece,
        // pins is all the attack rays from the king, ignoring the pinned piece along the way
        pin &= pins;
        // add the pinning piece. we are allowed to capture it if we can
        pin |= toBoard(pinner);

        (isCardinal ? cardinalPins : ordinalPins) |= pin;
    }
    // put the pieces we suspect are pinned back on the board
    position.occupied ^= pinned;
}

/*
 * update the resolverSquares bitboard for a given position and side.
 * resolver squares are the squares we can move a piece to
 * in order to resolve a check given by an attacking piece.
 * we can resolve a check by blocking the attacking piece or capturing it.
 */
template<bool isEngine>
void MoveGen::updateResolverSquares()
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
                resolverSquares = cardinalAttacks & getSlidingMoves<true>(toSquare(attackers));
                // any square that captures the checking piece is safe
                resolverSquares |= attackers;
            }
            // if the king is being attacked by a bishop or queen
            else if (ordinalAttackers)
            {
                // any square that blocks the attacking piece is safe
                resolverSquares = ordinalAttacks & getSlidingMoves<false>(toSquare(attackers));
                // any square that captures the checking piece is safe
                resolverSquares |= attackers;
            }
            // if the king is being attacked by a pawn or knight
            else
            {
                // we can't block an attack given by a pawn or knight, they are not sliding pieces.
                // we can only capture this piece to get out of check
                resolverSquares = attackers;
            }
        }
        // if there are multiple pieces attacking the king
        else
        {
            // there is no square we can move to that blocks a double check.
            // also, we can not capture two checking pieces at once.
            // so there are no safe squares, and the king must be moved in this case
            resolverSquares = EMPTY_BITBOARD;
        }
    }
    // if there is nothing attacking the king
    else
    {
        // we can move to any square without leaving the king in check
        resolverSquares = FULL_BITBOARD;
    }
}

/*
 * update the safeSquares bitboard for a given position and side.
 * a safe square is a square the king can legally move to.
 * We calculate the safe squares by finding all the opponent's
 * pseudo-legal attacks. any square that is not attacked is safe.
 */
template<bool isEngine>
void MoveGen::updateSafeSquares()
{
    safeSquares = EMPTY_BITBOARD;
    Bitboard king = position.pieces[isEngine ? ENGINE_KING : PLAYER_KING];

    // before we do any sliding piece calculation, remove the king from its square.
    // we need to do this so the king won't slide along an attacker's path, leaving itself in check.
    position.occupied ^= king;

    // add squares attacked in the cardinal directions
    Bitboard cardinalAttackers = position.pieces[isEngine ? PLAYER_ROOK : ENGINE_ROOK] |
                                 position.pieces[isEngine ? PLAYER_QUEEN : ENGINE_QUEEN];
    while (cardinalAttackers)
    {
        safeSquares |= getSlidingMoves<true>(popFirstPiece(cardinalAttackers));
    }
    // add squares attacked in the ordinal directions
    Bitboard ordinalAttackers = position.pieces[isEngine ? PLAYER_BISHOP : ENGINE_BISHOP] |
                                position.pieces[isEngine ? PLAYER_QUEEN : ENGINE_QUEEN];
    while (ordinalAttackers)
    {
        safeSquares |= getSlidingMoves<false>(popFirstPiece(ordinalAttackers));
    }
    // put the king back on the board
    position.occupied ^= king;

    // add pawn attacks
    Bitboard pawns = position.pieces[isEngine ? PLAYER_PAWN : ENGINE_PAWN];
    while (pawns)
    {
        Square pawn = popFirstPiece(pawns);
        safeSquares |= isEngine ? PLAYER_PAWN_CAPTURES[pawn] : ENGINE_PAWN_CAPTURES[pawn];
    }
    // add knight attacks
    Bitboard knights = position.pieces[isEngine ? PLAYER_KNIGHT : ENGINE_KNIGHT];
    while (knights)
    {
        safeSquares |= KNIGHT_MOVES[popFirstPiece(knights)];
    }
    // add king attacks
    safeSquares |= KING_MOVES[toSquare(position.pieces[isEngine ? PLAYER_KING : ENGINE_KING])];
    // turn "attacked squares" into "safe squares"
    safeSquares = ~safeSquares;
}

/*
 * use the magic bitboards defined in Magics.h to calculate a small hash.
 * this hash is derived by bit-shifting the result of a multiplication
 * between the blocking pieces and a pre-calculated magic number.
 * we can then use this hash to lookup the correct attack set in an attack table.
 *
 * This function returns moves for a sliding piece given its square
 * https://www.chessprogramming.org/Magic_Bitboards
 */
template<bool isCardinal>
Bitboard MoveGen::getSlidingMoves(Square from)
{
    if (isCardinal)
    {
        return CARDINAL_ATTACKS
            [from]
            [(CARDINAL_BLOCKERS[from] & position.occupied) * CARDINAL_MAGICS[from] >> 52];
    }
    return ORDINAL_ATTACKS
        [from]
        [(ORDINAL_BLOCKERS[from] & position.occupied) * ORDINAL_MAGICS[from] >> 55];
}

/*
 * knights are pretty easy. they just leap from one square to another,
 * so they can be implemented with a single bitboard in an array lookup.
 * additionally, we need to make sure we don't move onto our own pieces,
 * leave our king in check, or break any pin. If a knight is pinned,
 * it has no legal moves at all. it is the only piece with that property
 */
template<bool isEngine, bool quiets>
void MoveGen::genKnightMoves()
{
    Bitboard knights = position.pieces[isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT];
    // don't generate moves for pinned knights
    knights &= ~(cardinalPins | ordinalPins);

    while (knights)
    {
        Square from = popFirstPiece(knights);
        Bitboard moves = KNIGHT_MOVES[from];
        // throw away moves that leave the king in check
        moves &= resolverSquares;
        // moves and captures
        if (quiets)
        {
            moves &= (isEngine ? position.engineMovable : position.playerMovable);
        }
        // captures only
        else
        {
            moves &= (isEngine ? position.playerPieces : position.enginePieces);
        }
        // add knight moves to the move list
        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_KNIGHT : PLAYER_KNIGHT,
                position.getPiece<!isEngine>(to),
                from,
                to));
        }
    }
}

/*
 * king moves are a little complicated. The way they move is simple,
 * and their movement can be implemented by a single array lookup,
 * but we additionally have to validate castling moves.
 * A king can castle if:
 * 1) there are no pieces in between the king and the rook
 * 2) there are no attacked squares along the king's path during castling
 * 3) the king is not attacked by an enemy piece
 * 4) the king has not lost the right to castle
 *
 * "along the king's path" is not necessary the same as "in between the rook and the king".
 * they mean the same thing when we castle kingside. but not when we castle queenside
 */
template<bool isEngine, bool quiets>
void MoveGen::genKingMoves()
{
    Square from = toSquare(position.pieces[isEngine ? ENGINE_KING : PLAYER_KING]);
    Bitboard moves = KING_MOVES[from];
    // don't let the king walk onto attacked squares
    moves &= safeSquares;
    // don't let the king capture his own pieces
    moves &= (isEngine ? position.engineMovable : position.playerMovable);

    // if we want to generate all moves
    if (quiets)
    {
        Bitboard king = toBoard(from);
        // if the king is not in check, we might be able to castle
        if (king & safeSquares)
        {
            // if the king is allowed to castle queenside
            if (isEngine ? position.rights.engineCastleQueenside : position.rights.playerCastleQueenside)
            {
                // if there are no pieces in between the king and rook
                if (!(QUEENSIDE_CASTLE_EMPTIES & (isEngine ? RANK_7 : RANK_0) & position.occupied))
                {
                    // if there are no attacked squares along the king's path
                    if (!(QUEENSIDE_CASTLE_CHECKS & (isEngine ? RANK_7 : RANK_0) & ~safeSquares))
                    {
                        // we can castle queenside. add the castling move
                        moveList.push_back(makeMove(
                            CASTLE,
                            isEngine ? ENGINE_KING : PLAYER_KING,
                            NONE,
                            from,
                            ENGINE_IS_WHITE ? east(east(from)) : west(west(from))
                            ));
                    }
                }
            }

            // if the king is allowed to castle kingside
            if (isEngine ? position.rights.engineCastleKingside : position.rights.playerCastleKingside)
            {
                // if there are no pieces along the king's path and all the squares are safe
                if (!(KINGSIDE_CASTLE_CHECKS & (isEngine ? RANK_7 : RANK_0) & (position.occupied | ~safeSquares)))
                {
                    // we can castle kingside. add the castling move
                    moveList.push_back(makeMove(
                            CASTLE,
                            isEngine ? ENGINE_KING : PLAYER_KING,
                            NONE,
                            from,
                            ENGINE_IS_WHITE ? west(west(from)) : east(east(from))
                    ));
                }
            }
        }
    }
    // only captures
    else
    {
        moves &= (isEngine ? position.playerPieces : position.enginePieces);
    }
    // add king moves, not including castling moves
    while (moves)
    {
        Square to = popFirstPiece(moves);
        moveList.push_back(makeMove(
            NORMAL,
            isEngine ? ENGINE_KING : PLAYER_KING,
            position.getPiece<!isEngine>(to),
            from,
            to));
    }
}

/*
 * pawns are the most complicated piece on the chess board.
 * pawns move forward one square, or two squares if the pawn has never moved.
 * pawns capture enemy pieces one square diagonally
 * pawns can promote themselves to a powerful piece upon reaching the end of the board.
 * pawns can capture enemy pawns that where moved up two squares last move, landing next to the pawn.
 * this movement is called en passant. the pawn moves diagonally, and captures the pawn next to it.
 * this is they only situation in chess where the capturing piece does not land on the same square as the captured piece
 */
template<bool isEngine, bool quiets>
void MoveGen::genPawnMoves()
{
    Bitboard pawns = position.pieces[isEngine ? ENGINE_PAWN : PLAYER_PAWN];
    // generate non-captures
    if (quiets)
    {
        // don't generate pawn pushes for diagonally pinned pawns
        Bitboard pushed = pawns & ~ordinalPins;
        // generate one square pawn pushes
        pushed = isEngine ? south(pushed) : north(pushed);
        // don't generate promotions here, those are not quiet moves.
        pushed &= isEngine ? ~RANK_0 : ~RANK_7;
        // we can only push a pawn if the square in front of it is empty
        pushed &= position.empties;
        // generate two square pawn pushes
        Bitboard pushed2 = isEngine ? south(pushed) : north(pushed);
        // make sure a two square pawn push comes from the initial rank
        pushed2 &= isEngine ? RANK_4 : RANK_3;
        // we can only push a pawn if the square in front of it is empty
        pushed2 &= position.empties;
        // throw away pawn moves that leave the king in check
        pushed &= resolverSquares;
        pushed2 &= resolverSquares;
        // add one square pawn moves to the move list
        while (pushed)
        {
            Square to = popFirstPiece(pushed);
            Square from = isEngine ? north(to) : south(to);
            // throw away pawn moves that break a cardinal pin
            if ((toBoard(from) & cardinalPins) && !(toBoard(to) & cardinalPins))
            {
                continue;
            }
            // add the one square pawn push
            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                NONE,
                from,
                to));
        }
        // add two square pawn moves
        while (pushed2)
        {
            Square to = popFirstPiece(pushed2);
            Square from = isEngine ? north(north(to)) : south(south(to));
            // throw away moves that break a cardinal pin
            if ((toBoard(from) & cardinalPins) && !(toBoard(to) & cardinalPins))
            {
                continue;
            }
            // add the two square pawn push
            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                NONE,
                from,
                to));
        }
    }
    // don't generate capture moves or promotions for cardinal pinned pawns
    pawns &= ~cardinalPins;
    // don't generate push promotions for ordinal pinned pawns
    Bitboard pushPromotions = pawns & ~ordinalPins;
    // push the pawns one square forward
    pushPromotions = isEngine ? south(pushPromotions) : north(pushPromotions);
    // make sure we cannot promote on top of another piece
    pushPromotions &= position.empties;
    // throw away pawn push promotions that leave the king in check
    pushPromotions &= resolverSquares;
    // isolate promotion squares
    pushPromotions &= isEngine ? RANK_0 : RANK_7;
    // add push promotion moves
    while (pushPromotions)
    {
        Square to = popFirstPiece(pushPromotions);
        Square from = isEngine ? north(to) : south(to);
        // generate all four promotion types
        genPromotions<isEngine>(
                from,
                to,
                NONE
        );
    }
    // generate captures
    while (pawns)
    {
        Square from = popFirstPiece(pawns);
        Bitboard captures = isEngine ? ENGINE_PAWN_CAPTURES[from] : PLAYER_PAWN_CAPTURES[from];
        // if this pawn is ordinal pinned
        if (toBoard(from) & ordinalPins)
        {
            // we can still capture, but only along the pin
            captures &= ordinalPins;
        }
        /*
         * find an en passant capture before we validate legality for a normal capturing move.
         * en passant works slightly differently because the capturing pawn does not
         * land on the same square it captures on, so things that work well for other
         * moves like pin detection and blocker detection need special cases
         */
        Bitboard enPassant = captures & position.rights.enPassantCapture;
        // throw away pawn captures that leave the king in check
        captures &= resolverSquares;
        // make sure we can only capture enemy pieces
        captures &= (isEngine ? position.playerPieces : position.enginePieces);
        // isolate moves capturing with promotion
        Bitboard promotionCaptures = captures & (isEngine ? RANK_0 : RANK_7);
        // make sure we do not include promotion captures as normal captures
        captures &= (isEngine ? ~RANK_0 : ~RANK_7);
        // add normal capture moves
        while (captures)
        {
            Square to = popFirstPiece(captures);
            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                position.getPiece<!isEngine>(to),
                from,
                to));
        }
        // add promotion capture moves
        while (promotionCaptures)
        {
            Square to = popFirstPiece(promotionCaptures);
            genPromotions<isEngine>(
                from,
                to,
                position.getPiece<!isEngine>(to)
            );
        }
        /*
         * get rid of en passant captures that leave the king in check.
         * we need to shift the resolver squares to see if capturing this pawn
         * would actually resolve a check. when we generate the resolver
         * squares, we don't take en passant captures into account
         */
        enPassant &= isEngine ? south(resolverSquares) : north(resolverSquares);
        // if we can capture en passant
        if (enPassant)
        {
            /*
             * there is a special case that must be accounted for.
             * we cannot capture en passant if it breaks a horizontal pin.
             * this pin will not be detected by the normal pin generator because
             * this pin goes through two pieces, not one. So now we must do some
             * special manual pin detection for en passant captures
             */
            // start with removing the pawn we are capturing by en-passant
            Bitboard enPassantSquare = isEngine ? north(position.rights.enPassantCapture) : south(position.rights.enPassantCapture);
            position.occupied ^= enPassantSquare;
            /*
             * get cardinal sliding attacks outward from the pawn we are capturing with,
             * but without the that pawn that is being captured en passant.
             * this creates an attack ray where we can then check for piece types
             * in order to detect a horizontally pinned en passant move
             */
            Bitboard horizontalAttacks = getSlidingMoves<true>(from);
            // make sure we only care about a horizontal pin
            horizontalAttacks &= (isEngine ? RANK_3 : RANK_4);
            // isolate our king and an attacking enemy horizontal sliding piece
            horizontalAttacks &= position.pieces[isEngine ? ENGINE_KING : PLAYER_KING] |
                                 position.pieces[isEngine ? PLAYER_QUEEN : ENGINE_QUEEN] |
                                 position.pieces[isEngine ? PLAYER_ROOK : ENGINE_ROOK];
            // put the pawn that is being captured en-passant back on the board
            position.occupied ^= enPassantSquare;

            // if there are not two pieces on the pin ray, the pawn is not pinned
            if (countPieces(horizontalAttacks) != 2)
            {
                moveList.push_back(makeMove(
                    EN_PASSANT,
                    isEngine ? ENGINE_PAWN : PLAYER_PAWN,
                    isEngine ? PLAYER_PAWN : ENGINE_PAWN,
                    from,
                    toSquare(enPassant)));
            }
        }
    }
}

/*
 * rooks slide along the board in the cardinal directions,
 * stopping when they capture an enemy piece or are blocked by a friendly piece.
 * sliding move generation is done by hash table lookup.
 * all we need to do is make sure ordinal pinned rooks do not move,
 * and make sure cardinal pinned rooks do not move out of a pin
 */
template<bool isEngine, bool quiets>
void MoveGen::genRookMoves()
{
    Bitboard rooks = position.pieces[isEngine ? ENGINE_ROOK : PLAYER_ROOK];
    // don't generate moves for an ordinal pinned rook
    rooks &= ~ordinalPins;

    while (rooks)
    {
        Square from = popFirstPiece(rooks);
        Bitboard moves = getSlidingMoves<true>(from);

        // throw away moves that leave the king in check
        moves &= resolverSquares;
        // if this cardinal piece is cardinal pinned
        if (toBoard(from) & cardinalPins)
        {
            // it can still move, but it is restricted along the pin
            moves &= cardinalPins;
        }

        // all moves
        if (quiets)
        {
            moves &= isEngine ? position.engineMovable : position.playerMovable;
        }
        // only captures
        else
        {
            moves &= isEngine ? position.playerPieces : position.enginePieces;
        }
        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_ROOK : PLAYER_ROOK,
                position.getPiece<!isEngine>(to),
                from,
                to));
        }
    }
}

/*
 * bishops slide along the board in the ordinal directions,
 * stopping when they capture an enemy piece or are blocked by a friendly piece.
 * sliding move generation is done by hash table lookup.
 * all we need to do is make sure cardinal pinned bishops do not move,
 * and make sure ordinal pinned bishops do not move out of a pin
 */
template<bool isEngine, bool quiets>
void MoveGen::genBishopMoves()
{
    Bitboard bishops = position.pieces[isEngine ? ENGINE_BISHOP : PLAYER_BISHOP];
    // don't generate moves for a cardinal pinned bishop
    bishops &= ~cardinalPins;
    while (bishops)
    {
        Square from = popFirstPiece(bishops);
        Bitboard moves = getSlidingMoves<false>(from);

        // throw away moves that leave the king in check
        moves &= resolverSquares;
        // if this ordinal piece is ordinal pinned
        if (toBoard(from) & ordinalPins)
        {
            // it can still move, but it is restricted along the pin
            moves &= ordinalPins;
        }

        // all moves
        if (quiets)
        {
            moves &= isEngine ? position.engineMovable : position.playerMovable;
        }
        // only captures
        else
        {
            moves &= isEngine ? position.playerPieces : position.enginePieces;
        }
        while (moves)
        {
            Square to = popFirstPiece(moves);
            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_BISHOP : PLAYER_BISHOP,
                position.getPiece<!isEngine>(to),
                from,
                to));
        }
    }
}

/*
 * queens slide along the board in the cardinal and ordinal directions,
 * stopping when they capture an enemy piece or are blocked by a friendly piece.
 * sliding move generation is done by hash table lookup.
 * doing pin resolution is a little more complicated for queens,
 * because we have to make sure the queen does not illegally jump from
 * one cardinal/ordinal pin to a different pin of the same type.
 * We don't have to check for this with other sliding pieces
 */
template<bool isEngine, bool quiets>
void MoveGen::genQueenMoves()
{
    Bitboard queens = position.pieces[isEngine ? ENGINE_QUEEN : PLAYER_QUEEN];
    while (queens)
    {
        Square from = popFirstPiece(queens);
        Bitboard queen = toBoard(from);
        Bitboard moves = EMPTY_BITBOARD;

        // if we are not cardinal pinned
        if (queen & ~cardinalPins)
        {
            // generate ordinal moves
            moves |= getSlidingMoves<false>(from);
            // if we are ordinal pinned
            if (queen & ordinalPins)
            {
                // restrict queen movement to ordinal pin
                moves &= ordinalPins;
            }
        }
        // if we are not ordinal pinned
        if (queen & ~ordinalPins)
        {
            // generate cardinal moves
            moves |= getSlidingMoves<true>(from);
            // if we are cardinal pinned
            if (queen & cardinalPins)
            {
                // restrict queen movement to cardinal pin
                moves &= cardinalPins;
            }
        }
        // throw away queen moves that leave the king in check
        moves &= resolverSquares;
        // all moves
        if (quiets)
        {
            moves &= isEngine ? position.engineMovable : position.playerMovable;
        }
        // only captures
        else
        {
            moves &= isEngine ? position.playerPieces : position.enginePieces;
        }
        // add queen moves to the move list
        while (moves)
        {
            Square to = popFirstPiece(moves);

            moveList.push_back(makeMove(
                NORMAL,
                isEngine ? ENGINE_QUEEN : PLAYER_QUEEN,
                position.getPiece<!isEngine>(to),
                from,
                to));
        }
    }
}
