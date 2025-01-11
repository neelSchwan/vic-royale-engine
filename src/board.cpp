#include "board.h"
#include <string>
#include <iostream>
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

void Board::printBitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? "1" : "0");
        }
        std::cout << "\n";
    }
}

std::string Board::generateFEN() {
    std::string currentFENString = "";
    int emptySquares = 0;

    for(int rank = 7; rank >= 0; --rank) {
        for(int file = 0; file < 8; ++file) {
            char piece = '\0'; // this defaults to no piece

            int square = rank * 8 + file;
            if(whitePawns & (1ULL << square)){piece = 'P';}
            else if(whiteKnights & (1ULL << square)){piece = 'N';}
            else if(whiteBishops & (1ULL << square)){piece = 'B';}
            else if(whiteRooks & (1ULL << square)){piece = 'R';}
            else if(whiteQueen & (1ULL << square)){piece = 'Q';}
            else if(whiteKing & (1ULL << square)){piece = 'K';}
            else if(blackPawns & (1ULL << square)){piece = 'p';}
            else if(blackKnights & (1ULL << square)){piece = 'n';}
            else if(blackBishops & (1ULL << square)){piece = 'b';}
            else if(blackRooks & (1ULL << square)){piece = 'r';}
            else if(blackQueen & (1ULL << square)){piece = 'q';}
            else if(blackKing & (1ULL << square)){piece = 'k';}

            /*
            If there is some piece found, and there are empty squares, then we first append the number of emptySquares.
            If there is some piece found, but there isn't empty squares, then we just add the piece to the currentFenString.
            */
            if(piece != '\0') {
                if(emptySquares > 0) { 
                    currentFENString +=std::to_string(emptySquares);
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

    return currentFENString;
}