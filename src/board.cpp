#include "board.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <initializer_list>


uint64_t initSquares(std::initializer_list<int> squares) {
    uint64_t bitboard = 0;
    for(int square : squares) {
        bitboard |= (1ULL << square);
    }
    return bitboard;
}

Board::Board(){

    // White Pieces
    whitePawns   = initSquares({8,9,10,11,12,13,14,15}); // a2, b2, c2, d2, e2, f2, g2, h2
    whiteKnights = initSquares({1,6}); // b1, g1
    whiteBishops = initSquares({2,5}); // c1, f1
    whiteRooks   = initSquares({0,7}); // a1, h1
    whiteQueen   = initSquares({3}); // d1
    whiteKing    = initSquares({4}); // e1

    // Black Pieces
    blackPawns   = initSquares({48,49,50,51,52,53,54,55}); // a7, b7, c7, d7, e7, f7, g7, h7
    blackKnights = initSquares({57,62}); // b8, g8
    blackBishops = initSquares({58,61}); // c8, f8
    blackRooks   = initSquares({56,63}); // a8, h8
    blackQueen   = initSquares({59}); // d8
    blackKing    = initSquares({60}); // e8

    castlingRights = 0b1111;
    enPassantTarget = 0;
    halfmoveClock = 0;
    fullmoveCounter = 1;
    whiteToMove = true;
}

void Board::resetBitboards() {
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

void Board::printBitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? "1" : "0");
        }
        std::cout << "\n";
    }
}

int getSquareFromBitboard(uint64_t bitboard) {
    if(bitboard == 0) {
        return -1; // this bitboard is zero / isn't set.
    }

    return __builtin_ctzll(bitboard); // counts num unset bits before the first set bit.
}

std::string squareToAlgebraic(int square) {
    if (square < 0 || square > 63) {
        throw std::invalid_argument("Square index must be between 0 and 63.");
    }

    // Calculate file and rank
    char file = 'a' + (square % 8);
    char rank = '1' + (square / 8);

    return std::string() + file + rank;
}

std::string Board::generateFEN() {
    std::string currentFENString = "";
    int emptySquares = 0;

    // 1. Piece Placement
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            char piece = '\0';
            int square = rank * 8 + file;

            if (whitePawns & (1ULL << square)) piece = 'P';
            else if (whiteKnights & (1ULL << square)) piece = 'N';
            else if (whiteBishops & (1ULL << square)) piece = 'B';
            else if (whiteRooks & (1ULL << square)) piece = 'R';
            else if (whiteQueen & (1ULL << square)) piece = 'Q';
            else if (whiteKing & (1ULL << square)) piece = 'K';
            else if (blackPawns & (1ULL << square)) piece = 'p';
            else if (blackKnights & (1ULL << square)) piece = 'n';
            else if (blackBishops & (1ULL << square)) piece = 'b';
            else if (blackRooks & (1ULL << square)) piece = 'r';
            else if (blackQueen & (1ULL << square)) piece = 'q';
            else if (blackKing & (1ULL << square)) piece = 'k';

            if (piece != '\0') {
                if (emptySquares > 0) {
                    currentFENString += std::to_string(emptySquares);
                    emptySquares = 0;
                }
                currentFENString += piece;
            } else {
                emptySquares++;
            }
        }

        if (emptySquares > 0) {
            currentFENString += std::to_string(emptySquares);
            emptySquares = 0;
        }

        if (rank > 0) {
            currentFENString += "/";
        }
    }

    // 2. Side to Move
    currentFENString += " ";
    currentFENString += (whiteToMove ? "w" : "b");

    // 3. Castling Rights
    currentFENString += " ";
    std::string castlingRightsStr = "";
    if (castlingRights & 0b1000) castlingRightsStr += "K";
    if (castlingRights & 0b0100) castlingRightsStr += "Q";
    if (castlingRights & 0b0010) castlingRightsStr += "k";
    if (castlingRights & 0b0001) castlingRightsStr += "q";
    if (castlingRightsStr.empty()) castlingRightsStr = "-";
    currentFENString += castlingRightsStr;

    // 4. En Passant Target
    currentFENString += " ";
    if (enPassantTarget != 0) {
        int epSquare = getSquareFromBitboard(enPassantTarget);
        currentFENString += squareToAlgebraic(epSquare);
    } else {
        currentFENString += "-";
    }

    // 5. Halfmove Clock and Fullmove Counter
    currentFENString += " " + std::to_string(halfmoveClock);
    currentFENString += " " + std::to_string(fullmoveCounter);

    return currentFENString;
}

std::vector<std::string> split(const std::string& input, char delim) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, delim)) {
        tokens.push_back(token);
    }

    return tokens;
}

void Board::setBoardFromFEN(std::string fenNotationStr) {
    resetBitboards();

    // Split the FEN string into components
    std::vector<std::string> fenComponents = split(fenNotationStr, ' ');
    if (fenComponents.size() != 6) {
        throw std::invalid_argument("Invalid FEN string: must have exactly 6 components.");
    }

    for (size_t i = 0; i < fenComponents.size(); ++i) {
        std::cout << "Component " << i + 1 << ": " << fenComponents[i] << "\n";
    }

    // Parse Piece Placement
    std::string piecePlacement = fenComponents[0];
    int squareIndex = 56; // Start at top-left (rank 8, file a)
    for (char c : piecePlacement) {
        if (c == '/') {
            // Move to the next rank
            squareIndex -= 16;
        } else if (isdigit(c)) {
            // Skip empty squares
            squareIndex += c - '0';
        } else {
            uint64_t mask = (1ULL << squareIndex);
            switch (c) {
                case 'P': whitePawns |= mask; break;
                case 'N': whiteKnights |= mask; break;
                case 'B': whiteBishops |= mask; break;
                case 'R': whiteRooks |= mask; break;
                case 'Q': whiteQueen |= mask; break;
                case 'K': whiteKing |= mask; break;
                case 'p': blackPawns |= mask; break;
                case 'n': blackKnights |= mask; break;
                case 'b': blackBishops |= mask; break;
                case 'r': blackRooks |= mask; break;
                case 'q': blackQueen |= mask; break;
                case 'k': blackKing |= mask; break;
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
    if (castlingRightsStr != "-") {
        if (castlingRightsStr.find('K') != std::string::npos) castlingRights |= 0b1000;
        if (castlingRightsStr.find('Q') != std::string::npos) castlingRights |= 0b0100;
        if (castlingRightsStr.find('k') != std::string::npos) castlingRights |= 0b0010;
        if (castlingRightsStr.find('q') != std::string::npos) castlingRights |= 0b0001;
    }

    std::string enPassantStr = fenComponents[3];
    if (enPassantStr != "-") {
        int file = enPassantStr[0] - 'a';
        int rank = enPassantStr[1] - '1';
        enPassantTarget = (1ULL << (rank * 8 + file));
    } else {
        enPassantTarget = 0;
    }

    halfmoveClock = std::stoi(fenComponents[4]);

    fullmoveCounter = std::stoi(fenComponents[5]);
}



