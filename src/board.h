#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>
#include <stack>
#include <vector>
#include <initializer_list>

/**
 * Represents a chess board using bitboards.
 */
class Board
{
private:
    static void initZobrist();

public:
    Board();

    // ----------------------------------
    // Core Data
    // ----------------------------------
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueen;
    uint64_t whiteKing;

    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueen;
    uint64_t blackKing;

    // Castling rights (4 bits: White King-side, White Queen-side, Black King-side, Black Queen-side)
    uint8_t castlingRights;

    // En passant target square (encoded as a bitboard with only one bit set, or 0 if none)
    uint64_t enPassantTarget;

    // Halfmove clock (for 50-move rule)
    int halfmoveClock;

    // Fullmove counter (starts at 1 and increments after Black's move)
    int fullmoveCounter;

    // Current player to move (true for White, false for Black)
    bool whiteToMove;

    // ----------------------------------
    // Move Structure
    // ----------------------------------
    struct Move
    {
        int fromSquare;
        int toSquare;
        int movedPiece;
        int capturedPiece;
        int promotedPiece;

        unsigned char prevCastlingRights;
        uint64_t prevEntPassantTarget;
        int oldHalfmoveClock;
        int oldFullmoveCounter;

        bool isCastling;
        int rookFromSquare;
        int rookToSquare;

        Move();
    };

    // Stack to keep track of move history (for undo)
    std::stack<Move> moveHistory;

    // ----------------------------------
    // Board operations
    // ----------------------------------
    void resetBitboards();
    void makeMove(int fromSquare, int toSquare);
    void undoMove();

    /**
     * Finds which piece (type) is on a given square.
     * Positive = White piece, Negative = Black piece, 0 if empty.
     */
    int findPiece(int square) const;

    /**
     * Move a piece from one square to another on internal bitboards.
     * (Does not handle captures, etc. — just a low-level bitboard update.)
     */
    void movePiece(int pieceType, int fromSquare, int toSquare);

    /**
     * Places a piece on a given square (no removal).
     */
    void placePiece(int pieceType, int square);

    /**
     * Removes a piece from a given square (no replacement).
     */
    void removePiece(int pieceType, int square);

    // Debug: prints an 8x8 grid for a given bitboard.
    void printBitboard(uint64_t bitboard) const;

    // Zobrist hashing tables
    static uint64_t zobristTable[12][64];
    static uint64_t zobristBlackToMove;
    static uint64_t zobristCastling[16];
    static uint64_t zobristEnPassant[8];

    // ----------------------------------
    // Position evaluation and hashing
    // ----------------------------------
    uint64_t calculatePositionKey() const;
    int evaluatePosition() const;

    // ----------------------------------
    // Check detection
    // ----------------------------------
    /**
     * Returns true if the specified king is in check.
     * @param isWhiteKing true for white king, false for black king
     */
    bool isInCheck(bool isWhiteKing) const;

    /**
     * Returns true if the current side to move is in check.
     */
    bool isCurrentPlayerInCheck() const;

    /**
     * Returns true if the current position is checkmate.
     */
    bool isCheckmate() const;

    /**
     * Returns true if the current position is stalemate.
     */
    bool isStalemate() const;

    /**
     * Returns true if a move would leave the moving side's king in check.
     * @param move The move to validate
     */
    bool wouldLeaveInCheck(const Move &move) const;

    enum PieceType
    {
        NONE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    bool wouldLeaveInCheck(int from, int to, PieceType promotion = NONE) const;
};

std::vector<Board::Move> generateMoves(Board &board);
uint64_t perft(Board &board, int depth);

#endif // BOARD_H
