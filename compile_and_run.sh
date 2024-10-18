#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Default build type and Valgrind flag
BUILD_TYPE="Release"
USE_VALGRIND=false

# Parse arguments
for arg in "$@"
do
    case $arg in
        --dev)
            BUILD_TYPE="Dev"
            shift
            ;;
        --valgrind)
            USE_VALGRIND=true
            BUILD_TYPE="Debug"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--dev] [--valgrind]"
            echo "  --dev       Build with Dev configuration (optimized for development)"
            echo "  --valgrind  Run the program with Valgrind after building"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $arg${NC}"
            echo "Use --help or -h for usage information."
            exit 1
            ;;
    esac
done

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Run CMake with the specified build type
echo "Running CMake with build type: ${BUILD_TYPE}..."
if ! cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..; then
    echo -e "${RED}CMake configuration failed. Exiting.${NC}"
    exit 1
fi

# Run make (using all available cores)
echo "Building project..."
if ! make -j$(nproc); then
    echo -e "${RED}Build failed. Exiting.${NC}"
    exit 1
fi

# If we've made it this far, the build was successful
echo -e "${GREEN}Build successful.${NC}"
cd ..

# Run the executable
if [ "$USE_VALGRIND" = true ]; then
    echo "Running Software Renderer with Valgrind..."
    valgrind --leak-check=full --track-origins=yes ./build/game
else
    echo "Running Software Renderer..."
    ./build/game
fi
