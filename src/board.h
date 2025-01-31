#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>
#include <stack>

class Board
{
public:
    // Constructor
    Board();

    void resetBitboards();
    /*
    ----------------------------------------------------------------------------------------------
    Core Data
        - 12 Bitboards for piece representation
        - Castling rights
        - En passant target square
        - Halfmove clock and fullmove counter
        - Player to move
    ----------------------------------------------------------------------------------------------
    */

    // ------ White Pieces ------
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueen;
    uint64_t whiteKing;

    // ------ Black Pieces ------
    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueen;
    uint64_t blackKing;

    struct Move {
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

    Move()
        : fromSquare(0), toSquare(0), movedPiece(0), capturedPiece(0),
          promotedPiece(0), prevCastlingRights(0), prevEntPassantTarget(0),
          oldHalfmoveClock(0), oldFullmoveCounter(0), isCastling(false),
          rookFromSquare(-1), rookToSquare(-1)
    {
    }
};

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
    std::stack<Move> moveHistory;

    /*
    ----------------------------------------------------------------------------------------------
    FEN NOTATION PARSING
        - Standard starting position
        - FEN parsing and exporting
    ----------------------------------------------------------------------------------------------
    */

    std::string currentFENString; // Lowercase = black pieces, uppercase = white pieces

    // Generates the current FEN notation dynamically based on the bitboards for each piece
    std::string generateFEN();

    void setBoardFromFEN(std::string fenNotationStr);

    /*
    ----------------------------------------------------------------------------------------------
    BITBOARD UTILITIES
        - Set/unset specific bits
        - Query specific squares or piece types
    ----------------------------------------------------------------------------------------------
    */

    // Sets a bit in the given bitboard at the specified square
    uint64_t setBit(uint64_t bitboard, int square);

    // Clears a bit in the given bitboard at the specified square
    uint64_t clearBit(uint64_t bitboard, int square);

    // Counts the number of set bits in a given bitboard
    uint64_t countBits(uint64_t bitboard);

    /**
     * Finds the least-significant bit (LSB).
     * The LSB corresponds to bit position 0 in the bitboard (square a1 in typical layouts).
     */
    int findLSB(uint64_t bitboard);

    /**
     * Finds the most-significant bit (MSB).
     * The MSB corresponds to bit position 63 in the bitboard (square h8 in typical layouts).
     */
    int findMSB(uint64_t bitboard);

    // Masks
    uint64_t getRankMask(int rank);
    uint64_t getFileMask(int file);
    uint64_t getDiagonalMask(int square);
    uint64_t getAntiDiagonalMask(int square);

    /*
    ----------------------------------------------------------------------------------------------
    BOARD OPERATIONS
        - Execute a move
        - Undo a move
        - Check legality of moves
        - Detect check/checkmate
    ----------------------------------------------------------------------------------------------
    */

    // Executes a move on the board (e.g., update bitboards, special moves, capture, promotion)
    void makeMove(int fromSquare, int toSquare);

    // Undoes the last move, restoring the board to its previous state
    // (Requires storing enough move history to revert changes)
    void undoMove();

    // Determines if the current player’s king is in check
    bool isCheck();

    // Determines if the current position is checkmate (king in check + no legal moves)
    bool isCheckmate();

    int findPiece(int square) const;

    void movePiece(int pieceType, int fromSquare, int toSquare);
    void placePiece(int pieceType, int square);
    void removePiece(int pieceType, int square);
    /*
    ----------------------------------------------------------------------------------------------
    DEBUGGING
        - Print the board
    ----------------------------------------------------------------------------------------------
    */

    // Prints the board to the console in a human-readable format (ASCII)
    void printBitboard(uint64_t bitboard);
};

#endif // BOARD_H
