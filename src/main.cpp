#include <iostream>
#include <bitset>
#include "board.h"
#include <fstream>

void saveFENToFile(const std::string &fen, const std::string &filePath)
{
    std::ofstream outFile(filePath);
    if (outFile.is_open())
    {
        outFile << fen;
        outFile.close();
    }
    else
    {
        std::cerr << "Error: Unable to write to " << filePath << "\n";
    }
}

int main()
{
    // Create a new Board
    Board board;

    // Print initial FEN to confirm the starting position
    std::cout << "Initial position (FEN): " << board.generateFEN() << "\n\n";

    // Print bitboard for white pawns as a simple example
    std::cout << "White Pawns (initial):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "\n";

    // Example move: e2 (square 12) to e4 (square 28) for White
    int fromSquare = 12; // e2 in 0-based index
    int toSquare = 28;   // e4 in 0-based index

    // Make the move
    std::cout << "Making move e2-e4 ...\n";
    board.makeMove(fromSquare, toSquare);

    // Print the FEN and bitboards after the move
    std::cout << "Position after e2-e4 (FEN): " << board.generateFEN() << "\n\n";
    std::cout << "White Pawns (after e2-e4):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "\n";

    // Undo the move
    std::cout << "Undoing the move e2-e4 ...\n";
    board.undoMove();

    // Example move: e2 (square 12) to e4 (square 28) for White
    fromSquare = 12; // e2 in 0-based index
    toSquare = 63;   // e4 in 0-based index

    // Make the move
    std::cout << "Making move take rook ...\n";
    board.makeMove(fromSquare, toSquare);

    // Print the FEN and bitboards after the move
    std::cout << "Position after taking (FEN): " << board.generateFEN() << "\n\n";
    std::cout << "White Pawns (after taking):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "Black Rook after taking:\n";
    board.printBitboard(board.blackRooks);
    std::cout << "\n";

    // Undo the move
    std::cout << "Undoing the move rook take ...\n";
    board.undoMove();

    // Print the FEN and bitboards after undo
    std::cout << "Position after undo (FEN): " << board.generateFEN() << "\n\n";
    std::cout << "White Pawns (after undo):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "Black Rooks (after undo):\n";
    board.printBitboard(board.blackRooks);
    std::cout << "\n";

    return 0;
}