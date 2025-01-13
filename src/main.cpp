#include <iostream>
#include <bitset>
#include "board.h"

int main() {
	Board board;

    board.setBoardFromFEN("rnbqk1nr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR b KQkq e3 1 5");
    std::cout << "White pawns:\n";
    board.printBitboard(board.whitePawns);

    std::cout << board.generateFEN();
    // std::cout << "Black pawns:\n";
    // board.printBitboard(board.blackPawns);

    // board.printBitboard(board.blackKing);
    // board.printBitboard(board.whiteKing);

    // std::cout << board.generateFEN();
}