#include <iostream>
#include <bitset>
#include "board.h"
#include "fen.h"
#include <fstream>

void saveFENToFile(const std::string &fen, const std::string &filePath) {
    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << fen;
        outFile.close();
    } else {
        std::cerr << "Error: Unable to write to " << filePath << "\n";
    }
}

int main() {
    Board board;

    // Corrected: Use generateFEN as a free function
    std::cout << "Initial position (FEN): " << generateFEN(board) << "\n\n";

    std::cout << "White Pawns (initial):\n";
    board.printBitboard(board.whitePawns);

    // Example move: e2 (12) to e4 (28)
    int fromSquare = 12;
    int toSquare = 28;

    std::cout << "Making move e2-e4 ...\n";
    board.makeMove(fromSquare, toSquare);

    std::cout << "Position after e2-e4 (FEN): " << generateFEN(board) << "\n\n";
    std::cout << "White Pawns (after e2-e4):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "\n";

    std::cout << "Undoing the move e2-e4 ...\n";
    board.undoMove();

    // Example capture: White pawn takes Black rook at h8 (square 63)
    fromSquare = 12; // Still e2 (pawn)
    toSquare = 63;   // h8 (Black's rook initial position)

    std::cout << "Making invalid move e2-h8 (testing capture)...\n";
    try {
        board.makeMove(fromSquare, toSquare);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    std::cout << "Position after invalid move (FEN): " << generateFEN(board) << "\n\n";
    std::cout << "White Pawns (after invalid move):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "Black Rooks (after invalid move):\n";
    board.printBitboard(board.blackRooks);
    std::cout << "\n";

    std::cout << "Undoing the invalid move ...\n";
    board.undoMove();

    std::cout << "Position after undo (FEN): " << generateFEN(board) << "\n\n";
    std::cout << "White Pawns (after undo):\n";
    board.printBitboard(board.whitePawns);
    std::cout << "Black Rooks (after undo):\n";
    board.printBitboard(board.blackRooks);
    std::cout << "\n";

    return 0;
}