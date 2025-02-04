#ifndef FEN_H
#define FEN_H

#include <string>

class Board;

/**
 * Generate a FEN string from the current board position.
 * (Implementation in fen.cpp)
 */
std::string generateFEN(const Board& board);

/**
 * Set the board state from a given FEN string.
 * (Implementation in fen.cpp)
 */
void setBoardFromFEN(Board& board, const std::string& fenNotationStr);

#endif // FEN_H