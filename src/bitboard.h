#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>

/**
 * Various bitboard utility functions.
 * You can either make these static methods in the Board class or
 * keep them as free functions in a namespace, e.g. `namespace bb {...}`
 */

// Sets a bit in the given bitboard at the specified square
uint64_t setBit(uint64_t bitboard, int square);

// Clears a bit in the given bitboard at the specified square
uint64_t clearBit(uint64_t bitboard, int square);

// Counts the number of set bits in a given bitboard
uint64_t countBits(uint64_t bitboard);

/**
 * Finds the least-significant bit (LSB).
 * The LSB corresponds to bit position 0 in the bitboard.
 */
int findLSB(uint64_t bitboard);

/**
 * Finds the most-significant bit (MSB).
 * The MSB is bit position 63 in the bitboard.
 */
int findMSB(uint64_t bitboard);

// Example: get rank/file/diagonal masks
uint64_t getRankMask(int rank);
uint64_t getFileMask(int file);
uint64_t getDiagonalMask(int square);
uint64_t getAntiDiagonalMask(int square);

#endif // BITBOARD_H
