#include <iostream>
#include <bitset>
#include "board.h"
#include <fstream>

void saveFENToFile(const std::string& fen, const std::string& filePath) {
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
    board.setBoardFromFEN("8/8/8/4p1K1/2k1P3/8/8/8 b - - 0 1");
    std::string fen = board.generateFEN();
    saveFENToFile(fen, "src/fen.txt");
    return 0;
}