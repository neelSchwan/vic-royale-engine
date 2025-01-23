#include "board.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <initializer_list>

uint64_t initSquares(std::initializer_list<int> squares)
{
    uint64_t bitboard = 0;
    for (int square : squares)
    {
        bitboard |= (1ULL << square);
    }
    return bitboard;
}

Board::Board()
{

    // White Pieces
    whitePawns = initSquares({8, 9, 10, 11, 12, 13, 14, 15}); // a2, b2, c2, d2, e2, f2, g2, h2
    whiteKnights = initSquares({1, 6});                       // b1, g1
    whiteBishops = initSquares({2, 5});                       // c1, f1
    whiteRooks = initSquares({0, 7});                         // a1, h1
    whiteQueen = initSquares({3});                            // d1
    whiteKing = initSquares({4});                             // e1

    // Black Pieces
    blackPawns = initSquares({48, 49, 50, 51, 52, 53, 54, 55}); // a7, b7, c7, d7, e7, f7, g7, h7
    blackKnights = initSquares({57, 62});                       // b8, g8
    blackBishops = initSquares({58, 61});                       // c8, f8
    blackRooks = initSquares({56, 63});                         // a8, h8
    blackQueen = initSquares({59});                             // d8
    blackKing = initSquares({60});                              // e8

    castlingRights = 0b1111;
    enPassantTarget = 0;
    halfmoveClock = 0;
    fullmoveCounter = 1;
    whiteToMove = true;
}

void Board::resetBitboards()
{
    whitePawns = 0;
    whiteKnights = 0;
    whiteBishops = 0;
    whiteRooks = 0;
    whiteQueen = 0;
    whiteKing = 0;

    blackPawns = 0;
    blackKnights = 0;
    blackBishops = 0;
    blackRooks = 0;
    blackQueen = 0;
    blackKing = 0;
}

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

int getSquareFromBitboard(uint64_t bitboard)
{
    if (bitboard == 0)
    {
        return -1; // this bitboard is zero / isn't set.
    }

    return __builtin_ctzll(bitboard); // counts num unset bits before the first set bit.
}

std::string squareToAlgebraic(int square)
{
    if (square < 0 || square > 63)
    {
        throw std::invalid_argument("Square index must be between 0 and 63.");
    }

    // Calculate file and rank
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);

    return std::string() + file + rank;
}

std::string Board::generateFEN()
{
    std::string currentFENString = "";
    int emptySquares = 0;

    // 1. Piece Placement
    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            char piece = '\0';
            int square = rank * 8 + file;

            if (whitePawns & (1ULL << square))
                piece = 'P';
            else if (whiteKnights & (1ULL << square))
                piece = 'N';
            else if (whiteBishops & (1ULL << square))
                piece = 'B';
            else if (whiteRooks & (1ULL << square))
                piece = 'R';
            else if (whiteQueen & (1ULL << square))
                piece = 'Q';
            else if (whiteKing & (1ULL << square))
                piece = 'K';
            else if (blackPawns & (1ULL << square))
                piece = 'p';
            else if (blackKnights & (1ULL << square))
                piece = 'n';
            else if (blackBishops & (1ULL << square))
                piece = 'b';
            else if (blackRooks & (1ULL << square))
                piece = 'r';
            else if (blackQueen & (1ULL << square))
                piece = 'q';
            else if (blackKing & (1ULL << square))
                piece = 'k';

            if (piece != '\0')
            {
                if (emptySquares > 0)
                {
                    currentFENString += std::to_string(emptySquares);
                    emptySquares = 0;
                }
                currentFENString += piece;
            }
            else
            {
                emptySquares++;
            }
        }

        if (emptySquares > 0)
        {
            currentFENString += std::to_string(emptySquares);
            emptySquares = 0;
        }

        if (rank > 0)
        {
            currentFENString += "/";
        }
    }

    // 2. Side to Move
    currentFENString += " ";
    currentFENString += (whiteToMove ? "w" : "b");

    // 3. Castling Rights
    currentFENString += " ";
    std::string castlingRightsStr = "";
    if (castlingRights & 0b1000)
        castlingRightsStr += "K";
    if (castlingRights & 0b0100)
        castlingRightsStr += "Q";
    if (castlingRights & 0b0010)
        castlingRightsStr += "k";
    if (castlingRights & 0b0001)
        castlingRightsStr += "q";
    if (castlingRightsStr.empty())
        castlingRightsStr = "-";
    currentFENString += castlingRightsStr;

    // 4. En Passant Target
    currentFENString += " ";
    if (enPassantTarget != 0)
    {
        int epSquare = getSquareFromBitboard(enPassantTarget);
        currentFENString += squareToAlgebraic(epSquare);
    }
    else
    {
        currentFENString += "-";
    }

    // 5. Halfmove Clock and Fullmove Counter
    currentFENString += " " + std::to_string(halfmoveClock);
    currentFENString += " " + std::to_string(fullmoveCounter);

    return currentFENString;
}

std::vector<std::string> split(const std::string &input, char delim)
{
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, delim))
    {
        tokens.push_back(token);
    }

    return tokens;
}

void Board::setBoardFromFEN(std::string fenNotationStr)
{
    resetBitboards();

    // Split the FEN string into components
    std::vector<std::string> fenComponents = split(fenNotationStr, ' ');
    if (fenComponents.size() != 6)
    {
        throw std::invalid_argument("Invalid FEN string: must have exactly 6 components.");
    }

    for (size_t i = 0; i < fenComponents.size(); ++i)
    {
        std::cout << "Component " << i + 1 << ": " << fenComponents[i] << "\n";
    }

    // Parse Piece Placement
    std::string piecePlacement = fenComponents[0];
    int squareIndex = 56; // Start at top-left (rank 8, file a)
    for (char c : piecePlacement)
    {
        if (c == '/')
        {
            // Move to the next rank
            squareIndex -= 8;
        }
        else if (isdigit(c))
        {
            // Skip empty squares
            squareIndex += c - '0';
        }
        else
        {
            uint64_t mask = (1ULL << squareIndex);
            switch (c)
            {
            case 'P':
                whitePawns |= mask;
                break;
            case 'N':
                whiteKnights |= mask;
                break;
            case 'B':
                whiteBishops |= mask;
                break;
            case 'R':
                whiteRooks |= mask;
                break;
            case 'Q':
                whiteQueen |= mask;
                break;
            case 'K':
                whiteKing |= mask;
                break;
            case 'p':
                blackPawns |= mask;
                break;
            case 'n':
                blackKnights |= mask;
                break;
            case 'b':
                blackBishops |= mask;
                break;
            case 'r':
                blackRooks |= mask;
                break;
            case 'q':
                blackQueen |= mask;
                break;
            case 'k':
                blackKing |= mask;
                break;
            default:
                throw std::invalid_argument("Invalid piece character in FEN string.");
            }
            squareIndex++;
        }
    }

    std::string sideToMove = fenComponents[1];
    whiteToMove = (sideToMove == "w");

    std::string castlingRightsStr = fenComponents[2];
    castlingRights = 0;
    if (castlingRightsStr != "-")
    {
        if (castlingRightsStr.find('K') != std::string::npos)
            castlingRights |= 0b1000;
        if (castlingRightsStr.find('Q') != std::string::npos)
            castlingRights |= 0b0100;
        if (castlingRightsStr.find('k') != std::string::npos)
            castlingRights |= 0b0010;
        if (castlingRightsStr.find('q') != std::string::npos)
            castlingRights |= 0b0001;
    }

    std::string enPassantStr = fenComponents[3];
    if (enPassantStr != "-")
    {
        int file = enPassantStr[0] - 'a';
        int rank = enPassantStr[1] - '1';
        enPassantTarget = (1ULL << (rank * 8 + file));
    }
    else
    {
        enPassantTarget = 0;
    }

    halfmoveClock = std::stoi(fenComponents[4]);

    fullmoveCounter = std::stoi(fenComponents[5]);
}

// Move logic

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
// Bitboard helpers
void Board::removePiece(int pieceType, int square)
{
    uint64_t mask = (1ULL << square);

    if (pieceType == 1)
        whitePawns &= ~mask;
    if (pieceType == 2)
        whiteKnights &= ~mask;
    if (pieceType == 3)
        whiteBishops &= ~mask;
    if (pieceType == 4)
        whiteRooks &= ~mask;
    if (pieceType == 5)
        whiteQueen &= ~mask;
    if (pieceType == 6)
        whiteKing &= ~mask;
    if (pieceType == -1)
        blackPawns &= ~mask;
    if (pieceType == -2)
        blackKnights &= ~mask;
    if (pieceType == -3)
        blackBishops &= ~mask;
    if (pieceType == -4)
        blackRooks &= ~mask;
    if (pieceType == -5)
        blackQueen &= ~mask;
    if (pieceType == -6)
        blackKing &= ~mask;
}

void Board::placePiece(int pieceType, int square)
{
    uint64_t mask = (1ULL << square);

    if (pieceType == 1)
        whitePawns |= mask;
    if (pieceType == 2)
        whiteKnights |= mask;
    if (pieceType == 3)
        whiteBishops |= mask;
    if (pieceType == 4)
        whiteRooks |= mask;
    if (pieceType == 5)
        whiteQueen |= mask;
    if (pieceType == 6)
        whiteKing |= mask;
    if (pieceType == -1)
        blackPawns |= mask;
    if (pieceType == -2)
        blackKnights |= mask;
    if (pieceType == -3)
        blackBishops |= mask;
    if (pieceType == -4)
        blackRooks |= mask;
    if (pieceType == -5)
        blackQueen |= mask;
    if (pieceType == -6)
        blackKing |= mask;
}

// Move a piece from one square to another
void Board::movePiece(int pieceType, int fromSquare, int toSquare)
{
    removePiece(pieceType, fromSquare);
    placePiece(pieceType, toSquare);
}

void Board::makeMove(int fromSquare, int toSquare)
{
    // 1. Check piece color vs. side to move
    int currPiece = findPiece(fromSquare);
    if (currPiece == 0)
    {
        throw std::invalid_argument("Square is empty");
    }
    else if (currPiece > 0 && !whiteToMove)
    {
        throw std::invalid_argument("Cannot move white pieces when it's black's turn");
    }
    else if (currPiece < 0 && whiteToMove)
    {
        throw std::invalid_argument("Cannot move black pieces when it's white's turn");
    }

    // 2. Find any captured piece (if present)
    int capPiece = findPiece(toSquare);

    // 3. Build the Move struct
    Move newMove;
    newMove.fromSquare = fromSquare;
    newMove.toSquare = toSquare;
    newMove.capturedPiece = capPiece;
    newMove.movedPiece = currPiece;
    newMove.prevCastlingRights = castlingRights;
    newMove.prevEntPassantTarget = enPassantTarget;
    newMove.oldHalfmoveClock = halfmoveClock;
    newMove.oldFullmoveCounter = fullmoveCounter;
    newMove.isCastling = false;      // default
    newMove.rookFromSquare = -1;     // default
    newMove.rookToSquare = -1;       // default

    // 4. Remove captured piece if any
    if (capPiece != 0)
    {
        removePiece(capPiece, toSquare);

        // If we captured a rook in its original square, remove that castling right
        if (abs(capPiece) == 4)
        {
            switch (toSquare)
            {
            case 0:  castlingRights &= ~0b0100; break; // White's queenside rook
            case 7:  castlingRights &= ~0b1000; break; // White's kingside rook
            case 56: castlingRights &= ~0b0001; break; // Black's queenside rook
            case 63: castlingRights &= ~0b0010; break; // Black's kingside rook
            default: break;
            }
        }
    }

    // move the piece on the bitboards
    movePiece(currPiece, fromSquare, toSquare);

    // 6. Handle castling rights modifications for moving your own rook or king
    if (abs(currPiece) == 6)
    {
        if (currPiece > 0)
        {
            castlingRights &= 0b0011;

            // White short castle: e1 (4) -> g1 (6)
            if (fromSquare == 4 && toSquare == 6)
            {
                // Move the rook from h1 (7) to f1 (5)
                movePiece(4, 7, 5);
                newMove.isCastling = true;
                newMove.rookFromSquare = 7;
                newMove.rookToSquare = 5;
            }
            // White long castle: e1 (4) -> c1 (2)
            else if (fromSquare == 4 && toSquare == 2)
            {
                // Move the rook from a1 (0) to d1 (3)
                movePiece(4, 0, 3);
                newMove.isCastling = true;
                newMove.rookFromSquare = 0;
                newMove.rookToSquare = 3;
            }
        }
        else
        {
            castlingRights &= 0b1100;

            if (fromSquare == 60 && toSquare == 62)
            {
                movePiece(-4, 63, 61);
                newMove.isCastling = true;
                newMove.rookFromSquare = 63;
                newMove.rookToSquare = 61;
            }
            else if (fromSquare == 60 && toSquare == 58)
            {
                movePiece(-4, 56, 59);
                newMove.isCastling = true;
                newMove.rookFromSquare = 56;
                newMove.rookToSquare = 59;
            }
        }
    }
    // If a rook moves from its starting square, remove that specific castling right
    else if (abs(currPiece) == 4)
    {
        if (currPiece > 0) // White Rook
        {
            if (fromSquare == 0) castlingRights &= ~0b0100; // remove White Queenside (Q)
            if (fromSquare == 7) castlingRights &= ~0b1000; // remove White Kingside (K)
        }
        else // Black Rook
        {
            if (fromSquare == 56) castlingRights &= ~0b0001; // remove Black q
            if (fromSquare == 63) castlingRights &= ~0b0010; // remove Black k
        }
    }

    if (abs(currPiece) == 1)
    {
        // double push?
        if (std::abs(fromSquare - toSquare) == 16)
        {
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

    bool isWhitePawn = (currPiece == 1);
    bool isBlackPawn = (currPiece == -1);
    int toRank = toSquare / 8;

    if (isWhitePawn && toRank == 7)
    {
        removePiece(1, toSquare);
        placePiece(5, toSquare);
        newMove.promotedPiece = 5;
    }
    else if (isBlackPawn && toRank == 0)
    {
        removePiece(-1, toSquare);
        placePiece(-5, toSquare);
        newMove.promotedPiece = -5;
    }
    else
    {
        newMove.promotedPiece = 0;
    }

    // 9. Halfmove clock update (reset if pawn move or capture)
    if (abs(currPiece) == 1 || capPiece != 0)
        halfmoveClock = 0;
    else
        halfmoveClock++;

    // 10. Switch side to move; increment fullmoveCounter if black just played
    whiteToMove = !whiteToMove;
    if (!whiteToMove)
    {
        fullmoveCounter++;
    }

    // push the move on the stack
    moveHistory.push(newMove);
}

void Board::undoMove()
{
    if (moveHistory.empty())
    {
        throw std::runtime_error("No moves to undo.");
    }

    Move lastMove = moveHistory.top();
    moveHistory.pop();

    int fromSquare       = lastMove.fromSquare;
    int toSquare         = lastMove.toSquare;
    int movedPieceType   = lastMove.movedPiece;
    int capturedPieceType= lastMove.capturedPiece;

    // 1. Restore castlingRights, sideToMove, enPassantTarget, halfmove, fullmove
    castlingRights = lastMove.prevCastlingRights;
    whiteToMove = !whiteToMove;  // we had toggled it in makeMove
    enPassantTarget = lastMove.prevEntPassantTarget;
    halfmoveClock = lastMove.oldHalfmoveClock;
    fullmoveCounter = lastMove.oldFullmoveCounter;

    if (lastMove.promotedPiece != 0)
    {
        removePiece(lastMove.promotedPiece, toSquare);
        // Place the original pawn
        if (lastMove.promotedPiece > 0)
            placePiece(1, toSquare);   // White pawn
        else
            placePiece(-1, toSquare);  // Black pawn
    }

    // 3. Move the piece back to its original square
    movePiece(movedPieceType, toSquare, fromSquare);

    if (lastMove.isCastling)
    {
        movePiece((movedPieceType > 0 ? 4 : -4),
                  lastMove.rookToSquare,
                  lastMove.rookFromSquare);
    }

    if (capturedPieceType != 0)
    {
        placePiece(capturedPieceType, toSquare);
    }
}