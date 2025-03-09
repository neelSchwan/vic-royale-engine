#include "fen.h"
#include "board.h"

#include <cctype>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <iostream>

namespace
{
    // A local helper function to split a string by a delimiter
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

    // A local helper for extracting a single set bit as an index
    int getSquareFromBitboard(uint64_t bitboard)
    {
        if (bitboard == 0ULL)
        {
            return -1;
        }
        // __builtin_ctzll count trailing zeros
        return __builtin_ctzll(bitboard);
    }

    // Convert a 0-63 square index to standard algebraic notation (e.g. 0 -> a1, 63 -> h8)
    std::string squareToAlgebraic(int square)
    {
        if (square < 0 || square > 63)
            throw std::invalid_argument("Square index must be between 0 and 63.");

        char file = 'a' + (square % 8);
        char rank = '1' + (square / 8);

        return std::string() + file + rank;
    }
} // anonymous namespace

std::string generateFEN(const Board &board)
{
    std::string fen;
    int emptySquares = 0;

    // 1. Piece Placement
    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            char piece = '\0';
            int square = rank * 8 + file;
            uint64_t mask = (1ULL << square);

            // White
            if (board.whitePawns & mask)
                piece = 'P';
            else if (board.whiteKnights & mask)
                piece = 'N';
            else if (board.whiteBishops & mask)
                piece = 'B';
            else if (board.whiteRooks & mask)
                piece = 'R';
            else if (board.whiteQueen & mask)
                piece = 'Q';
            else if (board.whiteKing & mask)
                piece = 'K';
            // Black
            else if (board.blackPawns & mask)
                piece = 'p';
            else if (board.blackKnights & mask)
                piece = 'n';
            else if (board.blackBishops & mask)
                piece = 'b';
            else if (board.blackRooks & mask)
                piece = 'r';
            else if (board.blackQueen & mask)
                piece = 'q';
            else if (board.blackKing & mask)
                piece = 'k';

            if (piece != '\0')
            {
                if (emptySquares > 0)
                {
                    fen += std::to_string(emptySquares);
                    emptySquares = 0;
                }
                fen += piece;
            }
            else
            {
                emptySquares++;
            }
        }

        if (emptySquares > 0)
        {
            fen += std::to_string(emptySquares);
            emptySquares = 0;
        }

        if (rank > 0)
        {
            fen += "/";
        }
    }

    // 2. Side to Move
    fen += " ";
    fen += (board.whiteToMove ? "w" : "b");

    // 3. Castling rights
    fen += " ";
    {
        std::string castling;
        if (board.castlingRights & 0b1000)
            castling += "K";
        if (board.castlingRights & 0b0100)
            castling += "Q";
        if (board.castlingRights & 0b0010)
            castling += "k";
        if (board.castlingRights & 0b0001)
            castling += "q";
        if (castling.empty())
            castling = "-";
        fen += castling;
    }

    // 4. En passant
    fen += " ";
    if (board.enPassantTarget != 0ULL)
    {
        int epSquare = getSquareFromBitboard(board.enPassantTarget);
        fen += squareToAlgebraic(epSquare);
    }
    else
    {
        fen += "-";
    }

    // 5. Halfmove clock
    fen += " " + std::to_string(board.halfmoveClock);
    // 6. Fullmove counter
    fen += " " + std::to_string(board.fullmoveCounter);

    return fen;
}

void setBoardFromFEN(Board &board, const std::string &fenNotationStr)
{
    board.resetBitboards();

    // Split FEN
    std::vector<std::string> parts = split(fenNotationStr, ' ');
    if (parts.size() != 6)
        throw std::invalid_argument("Invalid FEN: must have 6 parts.");

    // 1. Piece Placement
    {
        std::string piecePlacement = parts[0];
        int squareIndex = 56; // start from a8 in row-major
        for (char c : piecePlacement)
        {
            if (c == '/')
            {
                squareIndex -= 8;
            }
            else if (std::isdigit(c))
            {
                squareIndex += (c - '0');
            }
            else
            {
                uint64_t mask = (1ULL << squareIndex);
                switch (c)
                {
                case 'P':
                    board.whitePawns |= mask;
                    break;
                case 'N':
                    board.whiteKnights |= mask;
                    break;
                case 'B':
                    board.whiteBishops |= mask;
                    break;
                case 'R':
                    board.whiteRooks |= mask;
                    break;
                case 'Q':
                    board.whiteQueen |= mask;
                    break;
                case 'K':
                    board.whiteKing |= mask;
                    break;
                case 'p':
                    board.blackPawns |= mask;
                    break;
                case 'n':
                    board.blackKnights |= mask;
                    break;
                case 'b':
                    board.blackBishops |= mask;
                    break;
                case 'r':
                    board.blackRooks |= mask;
                    break;
                case 'q':
                    board.blackQueen |= mask;
                    break;
                case 'k':
                    board.blackKing |= mask;
                    break;
                default:
                    throw std::invalid_argument("Invalid piece char in FEN.");
                }
                squareIndex++;
            }
        }
    }

    // 2. Side to move
    board.whiteToMove = (parts[1] == "w");

    // 3. Castling
    {
        board.castlingRights = 0;
        std::string castlingStr = parts[2];
        if (castlingStr != "-")
        {
            if (castlingStr.find('K') != std::string::npos)
                board.castlingRights |= 0b1000;
            if (castlingStr.find('Q') != std::string::npos)
                board.castlingRights |= 0b0100;
            if (castlingStr.find('k') != std::string::npos)
                board.castlingRights |= 0b0010;
            if (castlingStr.find('q') != std::string::npos)
                board.castlingRights |= 0b0001;
        }
    }

    // 4. En passant
    {
        std::string epStr = parts[3];
        if (epStr != "-")
        {
            int file = epStr[0] - 'a';
            int rank = epStr[1] - '1';
            board.enPassantTarget = (1ULL << (rank * 8 + file));
        }
        else
        {
            board.enPassantTarget = 0ULL;
        }
    }

    // 5. Halfmove clock
    board.halfmoveClock = std::stoi(parts[4]);

    // 6. Fullmove
    board.fullmoveCounter = std::stoi(parts[5]);
}
