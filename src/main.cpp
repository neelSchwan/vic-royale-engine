#include <iostream>
#include <bitset>
#include "board.h"

int main() {
	Board board;

    std::cout << "White pawns:\n";
    board.printBitboard(board.whitePawns);

    std::cout << "Black pawns:\n";
    board.printBitboard(board.blackPawns);

    board.printBitboard(board.blackKing);
    board.printBitboard(board.whiteKing);

}