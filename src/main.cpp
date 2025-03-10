// main.cpp
#include <iostream>
#include <fstream>
#include <exception>
#include <iomanip>
#include <unordered_set>
#include "board.h"
#include "fen.h"

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

<<<<<<< HEAD
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
=======
void printTestHeader(const std::string &testName)
{
    std::cout << "\n----------------------------------------\n";
    std::cout << "Test: " << testName << "\n";
    std::cout << "----------------------------------------\n";
}

void testZobristConsistency(Board &board)
{
    printTestHeader("Zobrist Hash Consistency");
>>>>>>> ed6e603c3ea48d617e8aff6178633c86cbed0521

    // Get initial position hash
    uint64_t initialHash = board.calculatePositionKey();
    std::cout << "Initial hash: 0x" << std::hex << initialHash << std::dec << "\n";

    // Make a move and back
    board.makeMove(12, 28); // e2-e4
    uint64_t afterMoveHash = board.calculatePositionKey();
    std::cout << "Hash after e2-e4: 0x" << std::hex << afterMoveHash << std::dec << "\n";

    board.undoMove();
    uint64_t afterUndoHash = board.calculatePositionKey();
    std::cout << "Hash after undo: 0x" << std::hex << afterUndoHash << std::dec << "\n";

<<<<<<< HEAD
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
=======
    if (initialHash == afterUndoHash)
    {
        std::cout << "✅ Zobrist hash consistency test passed\n";
>>>>>>> ed6e603c3ea48d617e8aff6178633c86cbed0521
    }
    else
    {
        std::cout << "❌ Zobrist hash consistency test failed\n";
    }
}

void testPositionEvaluation(Board &board)
{
    printTestHeader("Position Evaluation");

    // Test initial position (should be roughly equal)
    int initialScore = board.evaluatePosition();
    std::cout << "Initial position score: " << initialScore << "\n";

    // Make a move for White
    board.makeMove(12, 28); // e2-e4
    int scoreAfterE4 = board.evaluatePosition();
    std::cout << "Score after e2-e4: " << scoreAfterE4 << "\n";

    // Make a move for Black
    board.makeMove(52, 36); // e7-e5
    int scoreAfterE5 = board.evaluatePosition();
    std::cout << "Score after e7-e5: " << scoreAfterE5 << "\n";

    // Make another move for White
    board.makeMove(1, 16); // Nb1-c3
    int scoreAfterNc3 = board.evaluatePosition();
    std::cout << "Score after Nb1-c3: " << scoreAfterNc3 << "\n";

    // Undo all moves to return to initial position
    board.undoMove();
    board.undoMove();
    board.undoMove();
}

void testMoveGeneration(Board &board)
{
    printTestHeader("Move Generation");

    // Test initial position move counts at various depths
    for (int depth = 1; depth <= 3; depth++)
    {
        uint64_t nodes = perft(board, depth);
        std::cout << "Perft(" << depth << ") = " << nodes << " nodes\n";

        // Known correct values for initial position
        uint64_t expected[] = {20, 400, 8902};
        if (nodes == expected[depth - 1])
        {
            std::cout << "✅ Depth " << depth << " correct\n";
        }
        else
        {
            std::cout << "❌ Depth " << depth << " incorrect (expected " << expected[depth - 1] << ")\n";
        }
    }
}

void testPieceMovement(Board &board)
{
    printTestHeader("Piece Movement");

    // Test White pawn move
    std::cout << "Testing White pawn move (e2-e4)...\n";
    board.makeMove(12, 28); // e2-e4
    board.printBitboard(board.whitePawns);

<<<<<<< HEAD
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

=======
    // Test Black pawn move
    std::cout << "\nTesting Black pawn move (e7-e5)...\n";
    board.makeMove(52, 36); // e7-e5
    board.printBitboard(board.blackPawns);

    // Test White knight move
    std::cout << "\nTesting White knight move (Nb1-c3)...\n";
    board.makeMove(1, 16); // Nb1-c3
    board.printBitboard(board.whiteKnights);

    // Test Black knight move
    std::cout << "\nTesting Black knight move (Ng8-f6)...\n";
    board.makeMove(62, 45); // Ng8-f6
    board.printBitboard(board.blackKnights);

    // Test White bishop move
    std::cout << "\nTesting White bishop move (Bf1-c4)...\n";
    board.makeMove(5, 26); // Bf1-c4
    board.printBitboard(board.whiteBishops);

    // Undo all moves
    board.undoMove();
    board.undoMove();
    board.undoMove();
    board.undoMove();
    board.undoMove();
}

int main()
{
    try
    {
        Board board;

        // Run all tests
        testZobristConsistency(board);
        testPositionEvaluation(board);
        testMoveGeneration(board);
        testPieceMovement(board);

        // Test move validation
        printTestHeader("Move Validation");
        try
        {
            // Try an invalid move
            board.makeMove(0, 63); // Ra1-h8 (impossible)
            std::cout << "❌ Invalid move not caught\n";
        }
        catch (const std::exception &e)
        {
            std::cout << "✅ Invalid move caught: " << e.what() << "\n";
        }

        // Final state verification
        printTestHeader("Final State Verification");
        std::string finalFEN = generateFEN(board);
        std::cout << "Final FEN: " << finalFEN << "\n";
        if (finalFEN == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
        {
            std::cout << "✅ Board correctly reset to initial position\n";
        }
        else
        {
            std::cout << "❌ Board in unexpected state\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "❌ Unexpected error: " << e.what() << "\n";
        return 1;
    }
>>>>>>> ed6e603c3ea48d617e8aff6178633c86cbed0521
    return 0;
}
