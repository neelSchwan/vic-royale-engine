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

void printTestHeader(const std::string &testName)
{
    std::cout << "\n----------------------------------------\n";
    std::cout << "Test: " << testName << "\n";
    std::cout << "----------------------------------------\n";
}

void testZobristConsistency(Board &board)
{
    printTestHeader("Zobrist Hash Consistency");

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

    if (initialHash == afterUndoHash)
    {
        std::cout << "✅ Zobrist hash consistency test passed\n";
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
    for (int depth = 1; depth <= 5; depth++)
    {
        uint64_t nodes = perft(board, depth);
        std::cout << "Perft(" << depth << ") = " << nodes << " nodes\n";

        // Known correct values for initial position
        uint64_t expected[] = {20, 400, 8902, 197281, 4865609};
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
    return 0;
}
