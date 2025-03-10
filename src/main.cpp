#include <iostream>
#include <bitset>
#include "board.h"
#include "fen.h"
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

// Converts a 0..63 square index into standard algebraic notation:
// 0 => "a1", 7 => "h1", 56 => "a8", 63 => "h8", etc.
std::string squareToAlgebraic(int square) {
    int rank = square / 8;
    int file = square % 8;
    char fileChar = 'a' + file;
    char rankChar = '1' + rank;
    return std::string(1, fileChar) + std::string(1, rankChar);
}

std::string pieceToString(int piece) {
    bool white = (piece > 0);
    int type   = std::abs(piece); // 1..6
    char c;
    switch (type) {
        case 1: c = 'P'; break; // pawn
        case 2: c = 'N'; break; // knight
        case 3: c = 'B'; break; // bishop
        case 4: c = 'R'; break; // rook
        case 5: c = 'Q'; break; // queen
        case 6: c = 'K'; break; // king
        default: c = '?'; break;
    }
    // Use uppercase for white, lowercase for black
    if (!white) c = std::tolower(c);
    return std::string(1, c);
}

int main()
{
    Board board;

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
    try
    {
        board.makeMove(fromSquare, toSquare);
    }
    catch (const std::exception &e)
    {
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

    // generate all pseudo-legal moves from the initial position.
    std::vector<Board::Move> moveList;
    board.generateMoves(moveList);

    std::cout << "Number of pseudo-legal moves in the initial position: "
              << moveList.size() << "\n\n";

    // print each in readable format: For example: "P e2 -> e4" or "N b1 -> c3"
    int count = 0;
    for (const auto &m : moveList)
    {
        int pieceCode = m.movedPiece; // e.g. 1 = White Pawn, 2 = White Knight, -6 = Black King, etc.
        std::string piece = pieceToString(pieceCode);
        std::string fromSq = squareToAlgebraic(m.fromSquare);
        std::string toSq = squareToAlgebraic(m.toSquare);

        // If there's a capture, let's show it
        std::string captureInfo;
        if (m.capturedPiece != 0)
        {
            captureInfo = " x " + pieceToString(m.capturedPiece);
        }

        // If there's promotion, let's show it
        std::string promoInfo;
        if (m.promotedPiece != 0)
        {
            promoInfo = " = " + pieceToString(m.promotedPiece);
        }

        std::string castleInfo;
        if (m.isCastling)
        {
            castleInfo = " (castling)";
        }

        std::cout << ++count << ") " << piece
                  << " " << fromSq << " -> " << toSq
                  << captureInfo << promoInfo << castleInfo << "\n";
    }
    std::cout << "\n";

    if (!moveList.empty())
    {
        Board::Move firstMove = moveList[0];
        board.makeMove(firstMove.fromSquare, firstMove.toSquare);

        std::cout << "After making move #1 (" << pieceToString(firstMove.movedPiece)
                  << " " << squareToAlgebraic(firstMove.fromSquare) << "->"
                  << squareToAlgebraic(firstMove.toSquare) << "):\n";

        // Generate new moves
        std::vector<Board::Move> newMoveList;
        board.generateMoves(newMoveList);
        std::cout << "Number of pseudo-legal moves now: " << newMoveList.size() << "\n";
    }

    return 0;
}