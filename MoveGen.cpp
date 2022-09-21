//
// Created by Joe Chrisman on 9/17/22.
//

#include "MoveGen.h"

MoveGen::MoveGen() : position(ENGINE_IS_WHITE ?
                              ENGINE_WHITE_FEN :
                              ENGINE_BLACK_FEN)
{
    for (Square square = A1; square <= H8; square++)
    {
        Bitboard cardinalEndpoints = EMPTY_BITBOARD;

        int file = getFile(square);
        int rank = getRank(square);
        cardinalEndpoints |= toBoard((Square)(file + 7 * 8)); // north
        cardinalEndpoints |= toBoard((Square)(rank * 8 + 7)); // east
        cardinalEndpoints |= toBoard((Square)file); // south
        cardinalEndpoints |= toBoard((Square)(square - file)); // west

        cardinalMagics[square].blockers = getCardinalAttacks(square, cardinalEndpoints ,false);
        cardinalMagics[square].magic = getMagicNumber(square, true);

        ordinalMagics[square].blockers = getOrdinalAttacks(square, OUTER_SQUARES, false);
        ordinalMagics[square].magic = getMagicNumber(square, false);
    }
}

void MoveGen::genEngineMoves()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<true, true>();
    genKnightMoves<true, true>();
    genKingMoves<true, true>();
}

void MoveGen::genPlayerMoves()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<false, true>();
    genKnightMoves<false, true>();
    genKingMoves<false, true>();
}

void MoveGen::genEngineCaptures()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<true, false>();
    genKnightMoves<true, false>();
    genKingMoves<true, false>();
}

void MoveGen::genPlayerCaptures()
{
    moveList.clear();
    updateBitboards();
    genPawnMoves<false, false>();
    genKnightMoves<false, false>();
    genKingMoves<false, false>();
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
        Bitboard moves = KNIGHT_MOVES[from];

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

    // add capture moves
    while (pawns)
    {
        Square from = popFirstPiece(pawns);
        Bitboard captures = isEngine ? ENGINE_PAWN_CAPTURES[from] : PLAYER_PAWN_CAPTURES[from];
        captures &= (isEngine ? playerPieces : enginePieces);
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

Bitboard MoveGen::getMagicNumber(Square square, bool isCardinal)
{
    Bitboard allBlockers = isCardinal ? cardinalMagics[square].blockers : ordinalMagics[square].blockers;

    int numPermutations = 1 << countPieces(allBlockers);
    int maxPermutations = isCardinal ? 4096 : 512;

    uint64_t attacks[maxPermutations]; // attack bitboards by blocker permutation index
    uint64_t blockers[maxPermutations]; // blocker bitboard by blocker permutation index
    uint64_t attacksSeen[maxPermutations]; // attack bitboards by magic hash index

    for (int permutation = 0; permutation < maxPermutations; permutation++)
    {
        Bitboard actualBlockers = EMPTY_BITBOARD;
        Bitboard possibleBlockers = allBlockers;
        int blockerPermutation = permutation;
        while (possibleBlockers)
        {
            Bitboard blocker = popFirstBitboard(possibleBlockers);
            if (blockerPermutation % 2)
            {
                actualBlockers |= blocker;
            }
            blockerPermutation >>= 1;
        }
        blockers[permutation] = actualBlockers;
        attacks[permutation] = isCardinal ? getCardinalAttacks(square, actualBlockers, true) :
                                            getOrdinalAttacks(square, actualBlockers, true);
    }

    for (int tries = 0; tries < 1000000; tries++)
    {
        for (int i = 0; i < maxPermutations; i++)
        {
            attacksSeen[i] = EMPTY_BITBOARD;
        }

        Bitboard magic = getRandomBitboard() & getRandomBitboard() & getRandomBitboard();
        for (int permutation = 0; permutation < numPermutations; permutation++)
        {
            Bitboard hash = blockers[permutation] * magic >> (isCardinal ? 52 : 55);
            if (!attacksSeen[hash])
            {
                attacksSeen[hash] = attacks[permutation];
            }
            else if (attacksSeen[hash] != attacks[permutation])
            {
                magic = 0;
                break;
            }
        }
        // if we found a magic number
        if (magic)
        {
            for (int i = 0; i < maxPermutations; i++)
            {
                if (isCardinal)
                {
                    cardinalAttacks[square][i] = attacksSeen[i];
                }
                else
                {
                    ordinalAttacks[square][i] = attacksSeen[i];
                }
            }

            return magic;
        }
    }

    std::cerr << (isCardinal ? "Cardinal" : "Ordinal") << " magic number generation failed on square " << square << std::endl;
    assert(false);
}

Bitboard MoveGen::getCardinalAttacks(Square from, Bitboard blockers, bool captures)
{
    Bitboard attacks = EMPTY_BITBOARD;
    Bitboard attack;

    int rank = getRank(from);
    int file = getFile(from);

    for (int up = rank + 1; up < 8; up++)
    {
        attack = toBoard(getSquare(up, file));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
    }
    for (int right = file + 1; right < 8; right++)
    {
        attack = toBoard(getSquare(rank, right));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
    }
    for (int down = rank - 1; down >= 0; down--)
    {
        attack = toBoard(getSquare(down, file));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
    }
    for (int left = file - 1; left >= 0 ; left--)
    {
        attack = toBoard(getSquare(rank, left));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
    }
    return attacks;
}

Bitboard MoveGen::getOrdinalAttacks(Square from, Bitboard blockers, bool captures)
{
    Bitboard attacks = EMPTY_BITBOARD;
    Bitboard attack;

    int rank = getRank(from);
    int file = getFile(from);

    // northeast
    int distance = 1;
    while (isOnBoard(rank + distance, file + distance))
    {
        attack = toBoard(getSquare(rank + distance, file + distance));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
        distance++;
    }
    // southeast
    distance = 1;
    while (isOnBoard(rank - distance, file + distance))
    {
        attack = toBoard(getSquare(rank - distance, file + distance));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
        distance++;
    }
    // southwest
    distance = 1;
    while (isOnBoard(rank - distance, file - distance))
    {
        attack = toBoard(getSquare(rank - distance, file - distance));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
        distance++;
    }
    // northwest
    distance = 1;
    while (isOnBoard(rank + distance, file - distance))
    {
        attack = toBoard(getSquare(rank + distance, file - distance));
        if (attack & blockers)
        {
            if (captures)
            {
                attacks |= attack;
            }
            break;
        }
        attacks |= attack;
        distance++;
    }

    return attacks;
}