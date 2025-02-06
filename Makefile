# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Output binary
TARGET = chess

# Source files
SRC = src/main.cpp src/board.cpp src/fen.cpp src/bitboard.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Default rule
all: $(TARGET)

# Rule to link the target
$(TARGET): $(OBJ)
	@echo "Linking objects to create binary: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile each source file
%.o: %.cpp
	@echo "Compiling: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean
