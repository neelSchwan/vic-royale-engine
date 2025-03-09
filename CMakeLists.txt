cmake_minimum_required(VERSION 3.10)
project(VicRoyale VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/board.cpp
    src/fen.cpp
)

# Header files
set(HEADERS
    src/board.h
    src/fen.h
)

# Create executable
add_executable(vic_royale ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(vic_royale PRIVATE src)

# Output directory
set_target_properties(vic_royale
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
) 