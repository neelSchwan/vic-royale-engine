#include "board.h"
#include <iostream>
#include <stdexcept>

// Helper to initialize bitboard with a given set of squares
static uint64_t initSquares(std::initializer_list<int> squares)
{
    uint64_t bitboard = 0ULL;
    for (int square : squares)
    {
        bitboard |= (1ULL << square);
    }
    return bitboard;
}

// ---------- Board::Move Constructor ----------
Board::Move::Move()
    : fromSquare(0), toSquare(0), movedPiece(0), capturedPiece(0),
      promotedPiece(0), prevCastlingRights(0), prevEntPassantTarget(0),
      oldHalfmoveClock(0), oldFullmoveCounter(0), isCastling(false),
      rookFromSquare(-1), rookToSquare(-1)
{
}

// ---------- Board Constructor ----------
Board::Board()
{
    // White Pieces
    whitePawns   = initSquares({8, 9, 10, 11, 12, 13, 14, 15});
    whiteKnights = initSquares({1, 6});
    whiteBishops = initSquares({2, 5});
    whiteRooks   = initSquares({0, 7});
    whiteQueen   = initSquares({3});
    whiteKing    = initSquares({4});

    // Black Pieces
    blackPawns   = initSquares({48, 49, 50, 51, 52, 53, 54, 55});
    blackKnights = initSquares({57, 62});
    blackBishops = initSquares({58, 61});
    blackRooks   = initSquares({56, 63});
    blackQueen   = initSquares({59});
    blackKing    = initSquares({60});

    // Other properties
    castlingRights  = 0b1111;
    enPassantTarget = 0ULL;
    halfmoveClock   = 0;
    fullmoveCounter = 1;
    whiteToMove     = true;
}

// ---------- resetBitboards ----------
void Board::resetBitboards()
{
    whitePawns = 0ULL;
    whiteKnights = 0ULL;
    whiteBishops = 0ULL;
    whiteRooks = 0ULL;
    whiteQueen = 0ULL;
    whiteKing = 0ULL;

    blackPawns = 0ULL;
    blackKnights = 0ULL;
    blackBishops = 0ULL;
    blackRooks = 0ULL;
    blackQueen = 0ULL;
    blackKing = 0ULL;
}

// ---------- findPiece ----------
int Board::findPiece(int square) const
{
    uint64_t mask = (1ULL << square);

    if (whitePawns & mask)   return  1;
    if (whiteKnights & mask) return  2;
    if (whiteBishops & mask) return  3;
    if (whiteRooks & mask)   return  4;
    if (whiteQueen & mask)   return  5;
    if (whiteKing & mask)    return  6;

    if (blackPawns & mask)   return -1;
    if (blackKnights & mask) return -2;
    if (blackBishops & mask) return -3;
    if (blackRooks & mask)   return -4;
    if (blackQueen & mask)   return -5;
    if (blackKing & mask)    return -6;

    return 0;
}

// ---------- movePiece ----------
void Board::movePiece(int pieceType, int fromSquare, int toSquare)
{
    removePiece(pieceType, fromSquare);
    placePiece(pieceType, toSquare);
}

// ---------- placePiece ----------
void Board::placePiece(int pieceType, int square)
{
    uint64_t mask = (1ULL << square);

    switch (pieceType)
    {
        case  1:  whitePawns   |= mask; break;
        case  2:  whiteKnights |= mask; break;
        case  3:  whiteBishops |= mask; break;
        case  4:  whiteRooks   |= mask; break;
        case  5:  whiteQueen   |= mask; break;
        case  6:  whiteKing    |= mask; break;
        case -1:  blackPawns   |= mask; break;
        case -2:  blackKnights |= mask; break;
        case -3:  blackBishops |= mask; break;
        case -4:  blackRooks   |= mask; break;
        case -5:  blackQueen   |= mask; break;
        case -6:  blackKing    |= mask; break;
        default: break;
    }
}

// ---------- removePiece ----------
void Board::removePiece(int pieceType, int square)
{
    uint64_t mask = ~(1ULL << square);

    switch (pieceType)
    {
        case  1:  whitePawns   &= mask; break;
        case  2:  whiteKnights &= mask; break;
        case  3:  whiteBishops &= mask; break;
        case  4:  whiteRooks   &= mask; break;
        case  5:  whiteQueen   &= mask; break;
        case  6:  whiteKing    &= mask; break;
        case -1:  blackPawns   &= mask; break;
        case -2:  blackKnights &= mask; break;
        case -3:  blackBishops &= mask; break;
        case -4:  blackRooks   &= mask; break;
        case -5:  blackQueen   &= mask; break;
        case -6:  blackKing    &= mask; break;
        default: break;
    }
}

// ---------- printBitboard ----------
void Board::printBitboard(uint64_t bitboard)
{
    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? "1" : "0");
        }
        std::cout << "\n";
    }
}

// ---------- makeMove ----------
void Board::makeMove(int fromSquare, int toSquare)
{
    // 1. Check piece color vs. side to move
    int currPiece = findPiece(fromSquare);
    if (currPiece == 0)
        throw std::invalid_argument("Square is empty");

    if ( (currPiece > 0 && !whiteToMove) ||
         (currPiece < 0 &&  whiteToMove) )
    {
        throw std::invalid_argument("Cannot move opponent's piece");
    }

    // 2. Check capture
    int capPiece = findPiece(toSquare);

    // 3. Save Move info
    Move newMove;
    newMove.fromSquare         = fromSquare;
    newMove.toSquare           = toSquare;
    newMove.movedPiece         = currPiece;
    newMove.capturedPiece      = capPiece;
    newMove.prevCastlingRights = castlingRights;
    newMove.prevEntPassantTarget = enPassantTarget;
    newMove.oldHalfmoveClock   = halfmoveClock;
    newMove.oldFullmoveCounter = fullmoveCounter;
    newMove.isCastling         = false;  // default

    // 4. Capture
    if (capPiece != 0)
    {
        removePiece(capPiece, toSquare);

        // Possibly remove castling rights if we captured a rook in its corner
        if (abs(capPiece) == 4)
        {
            switch (toSquare)
            {
                case 0:  castlingRights &= ~0b0100; break; // White Q-side
                case 7:  castlingRights &= ~0b1000; break; // White K-side
                case 56: castlingRights &= ~0b0001; break; // Black q-side
                case 63: castlingRights &= ~0b0010; break; // Black k-side
                default: break;
            }
        }
    }

    // 5. Move piece
    movePiece(currPiece, fromSquare, toSquare);

    // 6. Castling checks
    if (abs(currPiece) == 6) // King
    {
        if (currPiece > 0)
        {
            // White king moved => remove white castling rights
            castlingRights &= 0b0011;

            // White short castle
            if (fromSquare == 4 && toSquare == 6)
            {
                movePiece(4, 7, 5); // move rook h1->f1
                newMove.isCastling     = true;
                newMove.rookFromSquare = 7;
                newMove.rookToSquare   = 5;
            }
            // White long castle
            else if (fromSquare == 4 && toSquare == 2)
            {
                movePiece(4, 0, 3); // move rook a1->d1
                newMove.isCastling     = true;
                newMove.rookFromSquare = 0;
                newMove.rookToSquare   = 3;
            }
        }
        else
        {
            // Black king moved => remove black castling rights
            castlingRights &= 0b1100;

            // Black short castle
            if (fromSquare == 60 && toSquare == 62)
            {
                movePiece(-4, 63, 61); // rook h8->f8
                newMove.isCastling     = true;
                newMove.rookFromSquare = 63;
                newMove.rookToSquare   = 61;
            }
            // Black long castle
            else if (fromSquare == 60 && toSquare == 58)
            {
                movePiece(-4, 56, 59); // rook a8->d8
                newMove.isCastling     = true;
                newMove.rookFromSquare = 56;
                newMove.rookToSquare   = 59;
            }
        }
    }
    else if (abs(currPiece) == 4) // Rook
    {
        // If rook moves from its corner, remove that castling right
        if (currPiece > 0)
        {
            // White rook
            if (fromSquare == 0) castlingRights &= ~0b0100; // White Q
            if (fromSquare == 7) castlingRights &= ~0b1000; // White K
        }
        else
        {
            // Black rook
            if (fromSquare == 56) castlingRights &= ~0b0001; // Black q
            if (fromSquare == 63) castlingRights &= ~0b0010; // Black k
        }
    }

    // 7. En passant updates
    if (abs(currPiece) == 1)
    {
        if (abs(fromSquare - toSquare) == 16)
        {
            // Double pawn push => set en passant
            int epSquareIdx = (fromSquare + toSquare) / 2;
            enPassantTarget = (1ULL << epSquareIdx);
        }
        else
        {
            enPassantTarget = 0ULL;
        }
    }
    else
    {
        enPassantTarget = 0ULL;
    }

    // 8. Pawn promotion
    bool isWhitePawn = (currPiece == 1);
    bool isBlackPawn = (currPiece == -1);
    int  toRank      = toSquare / 8;

    if (isWhitePawn && toRank == 7)
    {
        removePiece(1, toSquare);
        placePiece(5, toSquare);
        newMove.promotedPiece = 5;  // White queen
    }
    else if (isBlackPawn && toRank == 0)
    {
        removePiece(-1, toSquare);
        placePiece(-5, toSquare);
        newMove.promotedPiece = -5; // Black queen
    }
    else
    {
        newMove.promotedPiece = 0;
    }

    // 9. Halfmove clock
    if (abs(currPiece) == 1 || capPiece != 0)
        halfmoveClock = 0;
    else
        halfmoveClock++;

    // 10. Switch side + increment fullmove if black just moved
    whiteToMove = !whiteToMove;
    if (!whiteToMove)
        fullmoveCounter++;

    // Save the move
    moveHistory.push(newMove);
}

// ---------- undoMove ----------
void Board::undoMove()
{
    if (moveHistory.empty())
        throw std::runtime_error("No moves to undo.");

    Move lastMove = moveHistory.top();
    moveHistory.pop();

    int fromSquare       = lastMove.fromSquare;
    int toSquare         = lastMove.toSquare;
    int movedPieceType   = lastMove.movedPiece;
    int capturedPieceType= lastMove.capturedPiece;

    // Restore the board state
    castlingRights   = lastMove.prevCastlingRights;
    whiteToMove      = !whiteToMove; // revert
    enPassantTarget  = lastMove.prevEntPassantTarget;
    halfmoveClock    = lastMove.oldHalfmoveClock;
    fullmoveCounter  = lastMove.oldFullmoveCounter;

    // Handle promotion revert
    if (lastMove.promotedPiece != 0)
    {
        removePiece(lastMove.promotedPiece, toSquare);
        // Put original pawn back
        if (lastMove.promotedPiece > 0)
            placePiece(1, toSquare);   // White
        else
            placePiece(-1, toSquare);  // Black
    }

    // Move the piece back
    movePiece(movedPieceType, toSquare, fromSquare);

    // If castling, move the rook back
    if (lastMove.isCastling)
    {
        int rookType = (movedPieceType > 0) ? 4 : -4;
        movePiece(rookType, lastMove.rookToSquare, lastMove.rookFromSquare);
    }

    // If there was a captured piece, restore it
    if (capturedPieceType != 0)
    {
        placePiece(capturedPieceType, toSquare);
    }
}


// ----------- MOVE GENERATION -------------

// Precompute knight attack masks
constexpr uint64_t knightAttacks(int square) {
    uint64_t attacks = 0;
    int x = square % 8, y = square / 8;
    for (int dx : {-2, -1, 1, 2}) {
        for (int dy : {-2, -1, 1, 2}) {
            if (abs(dx) + abs(dy) == 3) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
                    attacks |= 1ULL << (ny * 8 + nx);
            }
        }
    }
    return attacks;
}

// Precompute king attack masks
constexpr uint64_t kingAttacks(int square) {
    uint64_t attacks = 0;
    int x = square % 8, y = square / 8;
    for (int dx : {-1, 0, 1}) {
        for (int dy : {-1, 0, 1}) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
                attacks |= 1ULL << (ny * 8 + nx);
        }
    }
    return attacks;
}

// Ray attacks for sliding pieces (Bishop/Rook/Queen)
uint64_t rayAttacks(int square, const std::vector<std::pair<int, int>>& directions, uint64_t occupied) {
    uint64_t attacks = 0;
    int x = square % 8, y = square / 8;
    for (auto [dx, dy] : directions) {
        for (int step = 1; ; ++step) {
            int nx = x + dx * step, ny = y + dy * step;
            if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) break;
            int target = ny * 8 + nx;
            attacks |= 1ULL << target;
            if (occupied & (1ULL << target)) break;
        }
    }
    return attacks;
}

// Bishop directions: 4 diagonals
const std::vector<std::pair<int, int>> bishopDirs = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
uint64_t bishopAttacks(int square, uint64_t occupied) {
    return rayAttacks(square, bishopDirs, occupied);
}

// Rook directions: 4 cardinals
const std::vector<std::pair<int, int>> rookDirs = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
uint64_t rookAttacks(int square, uint64_t occupied) {
    return rayAttacks(square, rookDirs, occupied);
}
