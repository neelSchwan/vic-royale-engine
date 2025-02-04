#include "bitboard.h"
#include <stdexcept>

// Sets a bit
uint64_t setBit(uint64_t bitboard, int square)
{
    return bitboard | (1ULL << square);
}

// Clears a bit
uint64_t clearBit(uint64_t bitboard, int square)
{
    return bitboard & ~(1ULL << square);
}

// Count bits
uint64_t countBits(uint64_t bitboard)
{
    // This is just a naive approach or you could use builtin popcount:
    // return __builtin_popcountll(bitboard);
    uint64_t count = 0ULL;
    while (bitboard)
    {
        bitboard &= (bitboard - 1);
        ++count;
    }
    return count;
}

// Find LSB
int findLSB(uint64_t bitboard)
{
    if (!bitboard) return -1; // no bits set
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(bitboard);
#else
    // fallback
    int idx = 0;
    while (!(bitboard & 1ULL))
    {
        bitboard >>= 1ULL;
        idx++;
    }
    return idx;
#endif
}

// Find MSB
int findMSB(uint64_t bitboard)
{
    if (!bitboard) return -1;
#if defined(__GNUC__) || defined(__clang__)
    return 63 - __builtin_clzll(bitboard);
#else
    // fallback
    int idx = 63;
    while (!(bitboard & (1ULL << 63)))
    {
        bitboard <<= 1ULL;
        idx--;
    }
    return idx;
#endif
}

// Example rank mask
uint64_t getRankMask(int rank)
{
    if (rank < 0 || rank > 7)
        throw std::out_of_range("Rank must be 0..7");
    return 0xFFULL << (rank * 8);
}

// Example file mask
uint64_t getFileMask(int file)
{
    if (file < 0 || file > 7)
        throw std::out_of_range("File must be 0..7");

    uint64_t mask = 0x0101010101010101ULL;
    return mask << file;
}

// Diagonal mask
uint64_t getDiagonalMask(int square)
{
    // For an 8x8 board, you can do something like:
    // On a standard board, each diagonal has squares (r,c) where r-c is constant.
    // ...
    // For brevity, just a placeholder:
    uint64_t mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    int r = rank, f = file;

    // Move up-left
    while (r < 8 && f >= 0)
    {
        mask |= (1ULL << (r*8 + f));
        r++; f--;
    }
    // Move down-right
    r = rank - 1; f = file + 1;
    while (r >= 0 && f < 8)
    {
        mask |= (1ULL << (r*8 + f));
        r--; f++;
    }
    return mask;
}

// Anti-diagonal mask
uint64_t getAntiDiagonalMask(int square)
{
    // Similar logic for anti-diagonal
    uint64_t mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    int r = rank, f = file;

    // Move up-right
    while (r < 8 && f < 8)
    {
        mask |= (1ULL << (r*8 + f));
        r++; f++;
    }
    // Move down-left
    r = rank - 1; f = file - 1;
    while (r >= 0 && f >= 0)
    {
        mask |= (1ULL << (r*8 + f));
        r--; f--;
    }
    return mask;
}
