#include "board.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <stack>
#include <initializer_list>
#include <utility>
#include <cmath>
#include <random>
#include <chrono>

// Static member definitions
uint64_t Board::zobristTable[12][64];
uint64_t Board::zobristBlackToMove;
uint64_t Board::zobristCastling[16];
uint64_t Board::zobristEnPassant[8];

const std::vector<std::pair<int, int>> bishopDirs = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const std::vector<std::pair<int, int>> rookDirs = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

// Piece-Square Tables for positional evaluation
const int PAWN_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};

const int KNIGHT_TABLE[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};

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
    whitePawns = initSquares({8, 9, 10, 11, 12, 13, 14, 15});
    whiteKnights = initSquares({1, 6});
    whiteBishops = initSquares({2, 5});
    whiteRooks = initSquares({0, 7});
    whiteQueen = initSquares({3});
    whiteKing = initSquares({4});

    // Black Pieces
    blackPawns = initSquares({48, 49, 50, 51, 52, 53, 54, 55});
    blackKnights = initSquares({57, 62});
    blackBishops = initSquares({58, 61});
    blackRooks = initSquares({56, 63});
    blackQueen = initSquares({59});
    blackKing = initSquares({60});

    // Other properties
    castlingRights = 0b1111;
    enPassantTarget = 0ULL;
    halfmoveClock = 0;
    fullmoveCounter = 1;
    whiteToMove = true;

    // Initialize Zobrist hashing tables
    initZobrist();
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

    if (whitePawns & mask)
        return 1;
    if (whiteKnights & mask)
        return 2;
    if (whiteBishops & mask)
        return 3;
    if (whiteRooks & mask)
        return 4;
    if (whiteQueen & mask)
        return 5;
    if (whiteKing & mask)
        return 6;

    if (blackPawns & mask)
        return -1;
    if (blackKnights & mask)
        return -2;
    if (blackBishops & mask)
        return -3;
    if (blackRooks & mask)
        return -4;
    if (blackQueen & mask)
        return -5;
    if (blackKing & mask)
        return -6;

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
    case 1:
        whitePawns |= mask;
        break;
    case 2:
        whiteKnights |= mask;
        break;
    case 3:
        whiteBishops |= mask;
        break;
    case 4:
        whiteRooks |= mask;
        break;
    case 5:
        whiteQueen |= mask;
        break;
    case 6:
        whiteKing |= mask;
        break;
    case -1:
        blackPawns |= mask;
        break;
    case -2:
        blackKnights |= mask;
        break;
    case -3:
        blackBishops |= mask;
        break;
    case -4:
        blackRooks |= mask;
        break;
    case -5:
        blackQueen |= mask;
        break;
    case -6:
        blackKing |= mask;
        break;
    default:
        break;
    }
}

// ---------- removePiece ----------
void Board::removePiece(int pieceType, int square)
{
    uint64_t mask = ~(1ULL << square);

    switch (pieceType)
    {
    case 1:
        whitePawns &= mask;
        break;
    case 2:
        whiteKnights &= mask;
        break;
    case 3:
        whiteBishops &= mask;
        break;
    case 4:
        whiteRooks &= mask;
        break;
    case 5:
        whiteQueen &= mask;
        break;
    case 6:
        whiteKing &= mask;
        break;
    case -1:
        blackPawns &= mask;
        break;
    case -2:
        blackKnights &= mask;
        break;
    case -3:
        blackBishops &= mask;
        break;
    case -4:
        blackRooks &= mask;
        break;
    case -5:
        blackQueen &= mask;
        break;
    case -6:
        blackKing &= mask;
        break;
    default:
        break;
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

    if ((currPiece > 0 && !whiteToMove) ||
        (currPiece < 0 && whiteToMove))
    {
        throw std::invalid_argument("Cannot move opponent's piece");
    }

    // 2. Generate legal moves and validate
    std::vector<Move> legalMoves = generateMoves(*this);
    bool moveFound = false;
    Move selectedMove;

    for (const Move &move : legalMoves)
    {
        if (move.fromSquare == fromSquare && move.toSquare == toSquare)
        {
            moveFound = true;
            selectedMove = move;
            break;
        }
    }

    if (!moveFound)
    {
        throw std::invalid_argument("Illegal move");
    }

    // 3. Check capture
    int capPiece = findPiece(toSquare);

    // 4. Save Move info
    Move newMove = selectedMove; // Use the move we found from legal moves
    newMove.capturedPiece = capPiece;
    newMove.prevCastlingRights = castlingRights;
    newMove.prevEntPassantTarget = enPassantTarget;
    newMove.oldHalfmoveClock = halfmoveClock;
    newMove.oldFullmoveCounter = fullmoveCounter;

    // 5. Capture
    if (capPiece != 0)
    {
        removePiece(capPiece, toSquare);

        // Possibly remove castling rights if we captured a rook in its corner
        if (abs(capPiece) == 4)
        {
            switch (toSquare)
            {
            case 0:
                castlingRights &= ~0b0100;
                break; // White Q-side
            case 7:
                castlingRights &= ~0b1000;
                break; // White K-side
            case 56:
                castlingRights &= ~0b0001;
                break; // Black q-side
            case 63:
                castlingRights &= ~0b0010;
                break; // Black k-side
            default:
                break;
            }
        }
    }

    // 6. Move piece
    movePiece(currPiece, fromSquare, toSquare);

    // 7. Handle castling
    if (selectedMove.isCastling)
    {
        int rookType = (currPiece > 0) ? 4 : -4;
        movePiece(rookType, selectedMove.rookFromSquare, selectedMove.rookToSquare);
    }

    // 8. Handle en passant
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

    // 9. Handle promotion
    if (selectedMove.promotedPiece != 0)
    {
        removePiece(currPiece, toSquare);
        placePiece(selectedMove.promotedPiece, toSquare);
    }

    // 10. Update castling rights
    if (abs(currPiece) == 6) // King
    {
        if (currPiece > 0)
            castlingRights &= 0b0011; // Remove white castling rights
        else
            castlingRights &= 0b1100; // Remove black castling rights
    }
    else if (abs(currPiece) == 4) // Rook
    {
        if (currPiece > 0)
        {
            if (fromSquare == 0)
                castlingRights &= ~0b0100; // White Q
            if (fromSquare == 7)
                castlingRights &= ~0b1000; // White K
        }
        else
        {
            if (fromSquare == 56)
                castlingRights &= ~0b0001; // Black q
            if (fromSquare == 63)
                castlingRights &= ~0b0010; // Black k
        }
    }

    // 11. Update move counters
    if (abs(currPiece) == 1 || capPiece != 0)
        halfmoveClock = 0;
    else
        halfmoveClock++;

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

    int fromSquare = lastMove.fromSquare;
    int toSquare = lastMove.toSquare;
    int movedPieceType = lastMove.movedPiece;
    int capturedPieceType = lastMove.capturedPiece;

    // Restore the board state
    castlingRights = lastMove.prevCastlingRights;
    whiteToMove = !whiteToMove; // revert
    enPassantTarget = lastMove.prevEntPassantTarget;
    halfmoveClock = lastMove.oldHalfmoveClock;
    fullmoveCounter = lastMove.oldFullmoveCounter;

    // Handle promotion revert
    if (lastMove.promotedPiece != 0)
    {
        removePiece(lastMove.promotedPiece, toSquare);
        // Put original pawn back
        if (lastMove.promotedPiece > 0)
            placePiece(1, toSquare); // White
        else
            placePiece(-1, toSquare); // Black
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
constexpr uint64_t knightAttacks(int square)
{
    uint64_t attacks = 0;
    int x = square % 8, y = square / 8;
    for (int dx : {-2, -1, 1, 2})
    {
        for (int dy : {-2, -1, 1, 2})
        {
            if (abs(dx) + abs(dy) == 3)
            {
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
                    attacks |= 1ULL << (ny * 8 + nx);
            }
        }
    }
    return attacks;
}

// Precompute king attack masks
constexpr uint64_t kingAttacks(int square)
{
    uint64_t attacks = 0;
    int x = square % 8, y = square / 8;
    for (int dx : {-1, 0, 1})
    {
        for (int dy : {-1, 0, 1})
        {
            if (dx == 0 && dy == 0)
                continue;
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
                attacks |= 1ULL << (ny * 8 + nx);
        }
    }
    return attacks;
}

// Ray attacks for sliding pieces (Bishop/Rook/Queen)
uint64_t rayAttacks(int square, const std::vector<std::pair<int, int>> &directions, uint64_t occupied)
{
    uint64_t attacks = 0;
    int x = square % 8, y = square / 8;
    for (auto [dx, dy] : directions)
    {
        for (int step = 1;; ++step)
        {
            int nx = x + dx * step, ny = y + dy * step;
            if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
                break;
            int target = ny * 8 + nx;
            attacks |= 1ULL << target;
            if (occupied & (1ULL << target))
                break;
        }
    }
    return attacks;
}

// Bishop directions: 4 diagonals
uint64_t bishopAttacks(int square, uint64_t occupied)
{
    return rayAttacks(square, bishopDirs, occupied);
}

// Rook directions: 4 cardinals
uint64_t rookAttacks(int square, uint64_t occupied)
{
    return rayAttacks(square, rookDirs, occupied);
}


uint64_t allPieces(const Board &board)
{
<<<<<<< HEAD
    moves.clear();

    // Occupied squares by both sides
    uint64_t occupied =   whitePawns   | whiteKnights | whiteBishops 
                        | whiteRooks   | whiteQueen   | whiteKing
                        | blackPawns   | blackKnights | blackBishops
                        | blackRooks   | blackQueen   | blackKing;

    // Friendly (side to move) and enemy bitboards
    const bool isWhite = whiteToMove;
    uint64_t friendly = isWhite 
      ? (whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing)
      : (blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing);

    uint64_t enemy = isWhite
      ? (blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing)
      : (whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing);

    // We will iterate over all 64 squares. 
    for (int square = 0; square < 64; ++square)
    {
        int piece = findPiece(square);
        if (piece == 0) 
            continue; // empty

        if ((piece > 0) != isWhite) 
            continue;

        // --- Generate moves based on piece type ---
        switch (std::abs(piece))
        {
            case 1: // Pawn
            {
                // If white: moves go "up" the board (increasing square index by +8).
                // If black: moves go "down" (decreasing square index by -8).

                const int forwardDir = (piece > 0) ? +8 : -8;
                const int startRank  = (piece > 0) ? 1 : 6;  // white pawns start rank=1, black=6
                const int rank       = square / 8;
                const int file       = square % 8;

                // Single pawn push (if target square is free)
                int forwardSq = square + forwardDir;
                if (forwardSq >= 0 && forwardSq < 64 && !(occupied & (1ULL << forwardSq)))
                {
                    // Create a Move
                    Board::Move m;
                    m.fromSquare     = square;
                    m.toSquare       = forwardSq;
                    m.movedPiece     = piece;
                    m.capturedPiece  = 0; // no capture
                    if ((piece > 0 && rank == 6) || (piece < 0 && rank == 1))
                    {
                        // Promotion
                        m.promotedPiece = (piece > 0) ? 5 : -5; // default to queen
                    }
                    else
                    {
                        m.promotedPiece = 0;
                    }

                    moves.push_back(m);

                    if (rank == startRank)
                    {
                        int doubleSq = square + 2 * forwardDir;
                        if (!(occupied & (1ULL << doubleSq)))
                        {
                            Board::Move dm;
                            dm.fromSquare     = square;
                            dm.toSquare       = doubleSq;
                            dm.movedPiece     = piece;
                            dm.capturedPiece  = 0;
                            dm.promotedPiece  = 0;

                            moves.push_back(dm);
                        }
                    }
                }

                // 3) Pawn captures (left and right diagonals)
                //   White: left capture => square + 7, right capture => square + 9
                //   Black: left capture => square - 9, right capture => square - 7
                // We can do them via (square + forwardDir +/- 1).
                for (int sideCapture : {-1, +1})
                {
                    if ((file == 0 && sideCapture == -1) || (file == 7 && sideCapture == +1))
                        continue; // can't capture off the edge

                    int capSquare = square + forwardDir + sideCapture;
                    if (capSquare < 0 || capSquare >= 64) 
                        continue;

                    uint64_t capMask = (1ULL << capSquare);
                    // normal capture
                    if (enemy & capMask)
                    {
                        Board::Move cm;
                        cm.fromSquare     = square;
                        cm.toSquare       = capSquare;
                        cm.movedPiece     = piece;
                        cm.capturedPiece  = findPiece(capSquare);
                        // Check promotion
                        if ((piece > 0 && rank == 6) || (piece < 0 && rank == 1))
                            cm.promotedPiece = (piece > 0) ? 5 : -5; // queen
                        else
                            cm.promotedPiece = 0;

                        moves.push_back(cm);
                    }
                    // en passant capture
                    if (enPassantTarget != 0ULL && (enPassantTarget & capMask))
                    {
                        Board::Move epm;
                        epm.fromSquare     = square;
                        epm.toSquare       = capSquare;
                        epm.movedPiece     = piece;
                        // The capturedPiece is the pawn on the adjacent rank, i.e. the square behind `capSquare`.
                        // If White just advanced a black pawn, it would be on 'capSquare - 8' etc.
                        int epCapturedSquare = capSquare + ((piece > 0) ? -8 : +8);
                        epm.capturedPiece    = findPiece(epCapturedSquare);

                        epm.promotedPiece    = 0; // no direct promotion on en passant

                        moves.push_back(epm);
                    }
                }
                break;
            }
            case 2: // Knight
            {
                // Use the precomputed or the function knightAttacks(square)
                uint64_t attacks = knightAttacks(square);
                // Exclude friendly squares
                attacks &= ~friendly;

                // For each set bit, create a move
                while (attacks)
                {
                    int to = __builtin_ctzll(attacks);
                    attacks &= (attacks - 1); // clear that bit

                    Board::Move km;
                    km.fromSquare    = square;
                    km.toSquare      = to;
                    km.movedPiece    = piece;
                    km.capturedPiece = (enemy & (1ULL << to)) ? findPiece(to) : 0;
                    km.promotedPiece = 0;

                    moves.push_back(km);
                }
                break;
            }
            case 3: // Bishop
            {
                uint64_t att = bishopAttacks(square, occupied);
                // Exclude friendly squares
                att &= ~friendly;

                // Each set bit is either an empty square or an enemy piece
                while (att)
                {
                    int to = __builtin_ctzll(att);
                    att &= (att - 1);

                    Board::Move bm;
                    bm.fromSquare    = square;
                    bm.toSquare      = to;
                    bm.movedPiece    = piece;
                    bm.capturedPiece = (enemy & (1ULL << to)) ? findPiece(to) : 0;
                    bm.promotedPiece = 0;

                    moves.push_back(bm);
                }
                break;
            }
            case 4: // Rook
            {
                uint64_t att = rookAttacks(square, occupied);
                att &= ~friendly;

                while (att)
                {
                    int to = __builtin_ctzll(att);
                    att &= (att - 1);

                    Board::Move rm;
                    rm.fromSquare    = square;
                    rm.toSquare      = to;
                    rm.movedPiece    = piece;
                    rm.capturedPiece = (enemy & (1ULL << to)) ? findPiece(to) : 0;
                    rm.promotedPiece = 0;

                    moves.push_back(rm);
                }
                break;
            }
            case 5: // Queen
            {
                // Queen attacks = bishop + rook directions
                uint64_t att = bishopAttacks(square, occupied) | rookAttacks(square, occupied);
                att &= ~friendly;

                while (att)
                {
                    int to = __builtin_ctzll(att);
                    att &= (att - 1);

                    Board::Move qm;
                    qm.fromSquare    = square;
                    qm.toSquare      = to;
                    qm.movedPiece    = piece;
                    qm.capturedPiece = (enemy & (1ULL << to)) ? findPiece(to) : 0;
                    qm.promotedPiece = 0;

                    moves.push_back(qm);
                }
                break;
            }
            case 6: // King
            {
                // Normal moves
                uint64_t att = kingAttacks(square);
                // Exclude friendly squares
                att &= ~friendly;

                while (att)
                {
                    int to = __builtin_ctzll(att);
                    att &= (att - 1);

                    Board::Move km;
                    km.fromSquare    = square;
                    km.toSquare      = to;
                    km.movedPiece    = piece;
                    km.capturedPiece = (enemy & (1ULL << to)) ? findPiece(to) : 0;
                    km.promotedPiece = 0;

                    moves.push_back(km);
                }

                if (piece > 0)
                {
                    // White short castle
                    if (castlingRights & 0b1000) // Suppose this bit is White King-side
                    {
                        // squares 5 & 6 must be empty, and the king can't be in check, etc.
                        if (!(occupied & ( (1ULL << 5) | (1ULL << 6) )))
                        {
                            Board::Move castleMove;
                            castleMove.fromSquare = square; // e1
                            castleMove.toSquare   = 6;      // g1
                            castleMove.movedPiece = 6;      // White King
                            castleMove.capturedPiece = 0;
                            castleMove.isCastling = true;
                            castleMove.rookFromSquare = 7;  // h1
                            castleMove.rookToSquare   = 5;  // f1
                            moves.push_back(castleMove);
                        }
                    }
                    // White long castle
                    if (castlingRights & 0b0100) // Suppose this bit is White Queen-side
                    {
                        // squares 1,2,3 must be empty
                        if (!(occupied & ( (1ULL << 1) | (1ULL << 2) | (1ULL << 3) )))
                        {
                            Board::Move castleMove;
                            castleMove.fromSquare = square; // e1
                            castleMove.toSquare   = 2;      // c1
                            castleMove.movedPiece = 6;      // White King
                            castleMove.capturedPiece = 0;
                            castleMove.isCastling = true;
                            castleMove.rookFromSquare = 0;  // a1
                            castleMove.rookToSquare   = 3;  // d1
                            moves.push_back(castleMove);
                        }
                    }
                }
                else 
                {
                    // Black short castle
                    if (castlingRights & 0b0010) // black king-side
                    {
                        // squares 61 & 62 must be empty
                        if (!(occupied & ( (1ULL << 61) | (1ULL << 62) )))
                        {
                            Board::Move castleMove;
                            castleMove.fromSquare = square; // e8 => 60
                            castleMove.toSquare   = 62; 
                            castleMove.movedPiece = -6;   // Black King
                            castleMove.capturedPiece = 0;
                            castleMove.isCastling = true;
                            castleMove.rookFromSquare = 63; // h8
                            castleMove.rookToSquare   = 61; // f8
                            moves.push_back(castleMove);
                        }
                    }
                    // Black long castle
                    if (castlingRights & 0b0001) // black queen-side
                    {
                        // squares 57,58,59 must be empty
                        if (!(occupied & ( (1ULL << 57) | (1ULL << 58) | (1ULL << 59) )))
                        {
                            Board::Move castleMove;
                            castleMove.fromSquare = square; // e8 => 60
                            castleMove.toSquare   = 58; 
                            castleMove.movedPiece = -6; // Black King
                            castleMove.capturedPiece = 0;
                            castleMove.isCastling = true;
                            castleMove.rookFromSquare = 56; // a8
                            castleMove.rookToSquare   = 59; // d8
                            moves.push_back(castleMove);
                        }
                    }
                }
                break;
            } // end king
        } // end switch
    } // end for(squares)
=======
    return board.whitePawns | board.whiteKnights | board.whiteBishops |
           board.whiteRooks | board.whiteQueen | board.whiteKing |
           board.blackPawns | board.blackKnights | board.blackBishops |
           board.blackRooks | board.blackQueen | board.blackKing;
}

uint64_t friendlyPieces(const Board &board)
{
    if (board.whiteToMove)
        return board.whitePawns | board.whiteKnights | board.whiteBishops |
               board.whiteRooks | board.whiteQueen | board.whiteKing;
    else
        return board.blackPawns | board.blackKnights | board.blackBishops |
               board.blackRooks | board.blackQueen | board.blackKing;
}

uint64_t enemyPieces(const Board &board)
{
    if (board.whiteToMove)
        return board.blackPawns | board.blackKnights | board.blackBishops |
               board.blackRooks | board.blackQueen | board.blackKing;
    else
        return board.whitePawns | board.whiteKnights | board.whiteBishops |
               board.whiteRooks | board.whiteQueen | board.whiteKing;
}

int findLSB(uint64_t bitboard)
{
    if (bitboard == 0)
        return -1;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(bitboard);
#else
    int idx = 0;
    while (!(bitboard & 1ULL))
    {
        bitboard >>= 1;
        ++idx;
    }
    return idx;
#endif
}

std::vector<Board::Move> generateMoves(Board &board)
{
    std::vector<Board::Move> moves;
    uint64_t friendly = friendlyPieces(board);
    uint64_t enemy = enemyPieces(board);
    uint64_t all = allPieces(board);

    if (board.whiteToMove)
    {
        // White pawn moves
        uint64_t pawns = board.whitePawns;
        while (pawns)
        {
            int sq = findLSB(pawns);
            pawns &= (pawns - 1);
            int rank = sq / 8, file = sq % 8;
            // Single push
            int target = sq + 8;
            if (target < 64 && !(all & (1ULL << target)))
            {
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = 1;
                moves.push_back(move);
                // Double push from starting rank (rank 1 in 0-indexed)
                if (rank == 1)
                {
                    int target2 = sq + 16;
                    if (!(all & (1ULL << target2)))
                    {
                        Board::Move move2;
                        move2.fromSquare = sq;
                        move2.toSquare = target2;
                        move2.movedPiece = 1;
                        moves.push_back(move2);
                    }
                }
            }
            // Captures: diagonal left and right
            if (file > 0)
            {
                int target = sq + 7;
                if (target < 64 && (enemy & (1ULL << target)))
                {
                    Board::Move move;
                    move.fromSquare = sq;
                    move.toSquare = target;
                    move.movedPiece = 1;
                    moves.push_back(move);
                }
            }
            if (file < 7)
            {
                int target = sq + 9;
                if (target < 64 && (enemy & (1ULL << target)))
                {
                    Board::Move move;
                    move.fromSquare = sq;
                    move.toSquare = target;
                    move.movedPiece = 1;
                    moves.push_back(move);
                }
            }
        }
        // White knight moves
        uint64_t knights = board.whiteKnights;
        while (knights)
        {
            int sq = findLSB(knights);
            knights &= (knights - 1);
            uint64_t knightMoves = knightAttacks(sq);
            knightMoves &= ~friendly;
            while (knightMoves)
            {
                int target = findLSB(knightMoves);
                knightMoves &= (knightMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = 2;
                moves.push_back(move);
            }
        }
        // White bishop moves
        uint64_t bishops = board.whiteBishops;
        while (bishops)
        {
            int sq = findLSB(bishops);
            bishops &= (bishops - 1);
            uint64_t bishopMoves = bishopAttacks(sq, all);
            bishopMoves &= ~friendly;

            while (bishopMoves)
            {
                int target = findLSB(bishopMoves);
                bishopMoves &= (bishopMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = 3;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }
        }

        // White rook moves
        uint64_t rooks = board.whiteRooks;
        while (rooks)
        {
            int sq = findLSB(rooks);
            rooks &= (rooks - 1);
            uint64_t rookMoves = rookAttacks(sq, all);
            rookMoves &= ~friendly;

            while (rookMoves)
            {
                int target = findLSB(rookMoves);
                rookMoves &= (rookMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = 4;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }
        }

        // White queen moves
        uint64_t queens = board.whiteQueen;
        while (queens)
        {
            int sq = findLSB(queens);
            queens &= (queens - 1);
            uint64_t queenMoves = bishopAttacks(sq, all) | rookAttacks(sq, all);
            queenMoves &= ~friendly;

            while (queenMoves)
            {
                int target = findLSB(queenMoves);
                queenMoves &= (queenMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = 5;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }
        }

        // White king moves
        uint64_t king = board.whiteKing;
        if (king)
        {
            int sq = findLSB(king);
            uint64_t kingMoves = kingAttacks(sq);
            kingMoves &= ~friendly;

            while (kingMoves)
            {
                int target = findLSB(kingMoves);
                kingMoves &= (kingMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = 6;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }

            // Castling moves
            if (board.castlingRights & 0b1000 &&        // King-side castle
                !(all & ((1ULL << 5) | (1ULL << 6))) && // f1 and g1 must be empty
                sq == 4)                                // King must be on e1
            {
                Board::Move move;
                move.fromSquare = 4;
                move.toSquare = 6;
                move.movedPiece = 6;
                move.isCastling = true;
                move.rookFromSquare = 7;
                move.rookToSquare = 5;
                moves.push_back(move);
            }
            if (board.castlingRights & 0b0100 &&                      // Queen-side castle
                !(all & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) && // b1, c1, and d1 must be empty
                sq == 4)                                              // King must be on e1
            {
                Board::Move move;
                move.fromSquare = 4;
                move.toSquare = 2;
                move.movedPiece = 6;
                move.isCastling = true;
                move.rookFromSquare = 0;
                move.rookToSquare = 3;
                moves.push_back(move);
            }
        }
    }
    else
    {
        // Black pawn moves
        uint64_t pawns = board.blackPawns;
        while (pawns)
        {
            int sq = findLSB(pawns);
            pawns &= (pawns - 1);
            int rank = sq / 8, file = sq % 8;
            // Single push (downward)
            int target = sq - 8;
            if (target >= 0 && !(all & (1ULL << target)))
            {
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = -1;
                moves.push_back(move);
                // Double push from starting rank (rank 6 in 0-indexed)
                if (rank == 6)
                {
                    int target2 = sq - 16;
                    if (!(all & (1ULL << target2)))
                    {
                        Board::Move move2;
                        move2.fromSquare = sq;
                        move2.toSquare = target2;
                        move2.movedPiece = -1;
                        moves.push_back(move2);
                    }
                }
            }
            // Captures
            if (file > 0)
            {
                int target = sq - 9;
                if (target >= 0 && !(friendly & (1ULL << target)) && (enemy & (1ULL << target)))
                {
                    Board::Move move;
                    move.fromSquare = sq;
                    move.toSquare = target;
                    move.movedPiece = -1;
                    moves.push_back(move);
                }
            }
            if (file < 7)
            {
                int target = sq - 7;
                if (target >= 0 && (enemy & (1ULL << target)))
                {
                    Board::Move move;
                    move.fromSquare = sq;
                    move.toSquare = target;
                    move.movedPiece = -1;
                    moves.push_back(move);
                }
            }
        }
        // Black knight moves
        uint64_t knights = board.blackKnights;
        while (knights)
        {
            int sq = findLSB(knights);
            knights &= (knights - 1);
            uint64_t knightMoves = knightAttacks(sq);
            knightMoves &= ~friendly;
            while (knightMoves)
            {
                int target = findLSB(knightMoves);
                knightMoves &= (knightMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = -2;
                moves.push_back(move);
            }
        }
        // Black bishop moves
        uint64_t bishops = board.blackBishops;
        while (bishops)
        {
            int sq = findLSB(bishops);
            bishops &= (bishops - 1);
            uint64_t bishopMoves = bishopAttacks(sq, all);
            bishopMoves &= ~friendly;

            while (bishopMoves)
            {
                int target = findLSB(bishopMoves);
                bishopMoves &= (bishopMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = -3;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }
        }

        // Black rook moves
        uint64_t rooks = board.blackRooks;
        while (rooks)
        {
            int sq = findLSB(rooks);
            rooks &= (rooks - 1);
            uint64_t rookMoves = rookAttacks(sq, all);
            rookMoves &= ~friendly;

            while (rookMoves)
            {
                int target = findLSB(rookMoves);
                rookMoves &= (rookMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = -4;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }
        }

        // Black queen moves
        uint64_t queens = board.blackQueen;
        while (queens)
        {
            int sq = findLSB(queens);
            queens &= (queens - 1);
            uint64_t queenMoves = bishopAttacks(sq, all) | rookAttacks(sq, all);
            queenMoves &= ~friendly;

            while (queenMoves)
            {
                int target = findLSB(queenMoves);
                queenMoves &= (queenMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = -5;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }
        }

        // Black king moves
        uint64_t king = board.blackKing;
        if (king)
        {
            int sq = findLSB(king);
            uint64_t kingMoves = kingAttacks(sq);
            kingMoves &= ~friendly;

            while (kingMoves)
            {
                int target = findLSB(kingMoves);
                kingMoves &= (kingMoves - 1);
                Board::Move move;
                move.fromSquare = sq;
                move.toSquare = target;
                move.movedPiece = -6;
                move.capturedPiece = board.findPiece(target);
                moves.push_back(move);
            }

            // Castling moves
            if (board.castlingRights & 0b0010 &&          // King-side castle
                !(all & ((1ULL << 61) | (1ULL << 62))) && // f8 and g8 must be empty
                sq == 60)                                 // King must be on e8
            {
                Board::Move move;
                move.fromSquare = 60;
                move.toSquare = 62;
                move.movedPiece = -6;
                move.isCastling = true;
                move.rookFromSquare = 63;
                move.rookToSquare = 61;
                moves.push_back(move);
            }
            if (board.castlingRights & 0b0001 &&                         // Queen-side castle
                !(all & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) && // b8, c8, and d8 must be empty
                sq == 60)                                                // King must be on e8
            {
                Board::Move move;
                move.fromSquare = 60;
                move.toSquare = 58;
                move.movedPiece = -6;
                move.isCastling = true;
                move.rookFromSquare = 56;
                move.rookToSquare = 59;
                moves.push_back(move);
            }
        }
    }
    return moves;
}

// perft: recursively counts leaf nodes up to a given depth
uint64_t perft(Board &board, int depth)
{
    if (depth == 0)
        return 1ULL;
    uint64_t nodes = 0;
    std::vector<Board::Move> moves = generateMoves(board);
    for (auto &move : moves)
    {
        board.makeMove(move.fromSquare, move.toSquare);
        nodes += perft(board, depth - 1);
        board.undoMove();
    }
    return nodes;
}

// Initialize Zobrist hashing tables
void Board::initZobrist()
{
    std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    for (int piece = 0; piece < 12; piece++)
    {
        for (int square = 0; square < 64; square++)
        {
            zobristTable[piece][square] = rng();
        }
    }
    zobristBlackToMove = rng();
    for (int i = 0; i < 16; i++)
    {
        zobristCastling[i] = rng();
    }
    for (int i = 0; i < 8; i++)
    {
        zobristEnPassant[i] = rng();
    }
}

uint64_t Board::calculatePositionKey() const
{
    uint64_t key = 0;

    // Hash pieces
    for (int square = 0; square < 64; square++)
    {
        int piece = findPiece(square);
        if (piece != 0)
        {
            int pieceIndex = (piece > 0 ? piece - 1 : -piece + 5); // Map to 0-11
            key ^= zobristTable[pieceIndex][square];
        }
    }

    // Hash side to move
    if (!whiteToMove)
    {
        key ^= zobristBlackToMove;
    }

    // Hash castling rights
    key ^= zobristCastling[castlingRights];

    // Hash en passant
    if (enPassantTarget)
    {
        int epFile = findLSB(enPassantTarget) % 8;
        key ^= zobristEnPassant[epFile];
    }

    return key;
}

int Board::evaluatePosition() const
{
    int score = 0;

    // Material counting
    score += __builtin_popcountll(whitePawns) * 100;
    score += __builtin_popcountll(whiteKnights) * 320;
    score += __builtin_popcountll(whiteBishops) * 330;
    score += __builtin_popcountll(whiteRooks) * 500;
    score += __builtin_popcountll(whiteQueen) * 900;

    score -= __builtin_popcountll(blackPawns) * 100;
    score -= __builtin_popcountll(blackKnights) * 320;
    score -= __builtin_popcountll(blackBishops) * 330;
    score -= __builtin_popcountll(blackRooks) * 500;
    score -= __builtin_popcountll(blackQueen) * 900;

    // Center control and piece development bonuses
    const uint64_t centerSquares = (1ULL << 27) | (1ULL << 28) | (1ULL << 35) | (1ULL << 36); // e4,d4,e5,d5
    const uint64_t extendedCenter = centerSquares |
                                    (1ULL << 26) | (1ULL << 29) | // c4,f4
                                    (1ULL << 34) | (1ULL << 37);  // c5,f5

    // Bonus for controlling center squares
    uint64_t whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing;
    uint64_t blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing;

    score += 10 * __builtin_popcountll(whitePieces & centerSquares);
    score += 5 * __builtin_popcountll(whitePieces & extendedCenter);
    score -= 10 * __builtin_popcountll(blackPieces & centerSquares);
    score -= 5 * __builtin_popcountll(blackPieces & extendedCenter);

    // Development bonus for knights and bishops
    const uint64_t whiteBackRank = 0xFFULL;
    const uint64_t blackBackRank = 0xFF00000000000000ULL;

    // Bonus for developed minor pieces
    score += 20 * __builtin_popcountll(whiteKnights & ~whiteBackRank);
    score += 20 * __builtin_popcountll(whiteBishops & ~whiteBackRank);
    score -= 20 * __builtin_popcountll(blackKnights & ~blackBackRank);
    score -= 20 * __builtin_popcountll(blackBishops & ~blackBackRank);

    // Positional scoring for pawns
    uint64_t wp = whitePawns;
    while (wp)
    {
        int sq = findLSB(wp);
        score += PAWN_TABLE[sq];
        wp &= (wp - 1);
    }

    uint64_t bp = blackPawns;
    while (bp)
    {
        int sq = findLSB(bp);
        score -= PAWN_TABLE[63 - sq]; // Flip table for black
        bp &= (bp - 1);
    }

    // Positional scoring for knights
    uint64_t wn = whiteKnights;
    while (wn)
    {
        int sq = findLSB(wn);
        score += KNIGHT_TABLE[sq];
        wn &= (wn - 1);
    }

    uint64_t bn = blackKnights;
    while (bn)
    {
        int sq = findLSB(bn);
        score -= KNIGHT_TABLE[63 - sq];
        bn &= (bn - 1);
    }

    // Add bonus for bishop pair
    if (__builtin_popcountll(whiteBishops) >= 2)
        score += 50;
    if (__builtin_popcountll(blackBishops) >= 2)
        score -= 50;

    // Penalize doubled pawns
    for (int file = 0; file < 8; file++)
    {
        uint64_t fileMask = 0x0101010101010101ULL << file;
        int whitePawnsOnFile = __builtin_popcountll(whitePawns & fileMask);
        int blackPawnsOnFile = __builtin_popcountll(blackPawns & fileMask);
        if (whitePawnsOnFile > 1)
            score -= 20 * (whitePawnsOnFile - 1);
        if (blackPawnsOnFile > 1)
            score += 20 * (blackPawnsOnFile - 1);
    }

    // Always return score from White's perspective
    return score;
>>>>>>> ed6e603c3ea48d617e8aff6178633c86cbed0521
}

