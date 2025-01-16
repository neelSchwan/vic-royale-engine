# -------------------------
# Simple Makefile for a C++20 Chess Engine
# -------------------------

CXX        = g++
CXXFLAGS   = -Wall -Wextra -std=c++20 -O2

EXEC       = chess_engine

# Source files
SOURCES    = src/main.cpp \
             src/board.cpp \
             #src/move_generator.cpp \
             #src/search.cpp \
             #src/evaluation.cpp

OBJECTS    = $(SOURCES:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean
