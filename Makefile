# -------------------------
# Simple Makefile for a C++20 Chess Engine
# -------------------------

# Compiler and flags
CXX        = g++
CXXFLAGS   = -Wall -Wextra -std=c++20 -O2

# Target executable name
EXEC       = chess_engine

# Source files (adjust as needed)
SOURCES    = src/main.cpp \
             src/board.cpp \
             src/move_generator.cpp \
             src/search.cpp \
             src/evaluation.cpp

# Object files (automatic substitution: .cpp -> .o)
OBJECTS    = $(SOURCES:.cpp=.o)

# Default target: build the main chess engine
all: $(EXEC)

# Link step: combine all object files into the final executable
$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

# Compile each .cpp into a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean
