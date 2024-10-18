cmake_minimum_required(VERSION 3.10)
project(SoftwareRenderer C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define build types
# For single-config generators like Makefiles, CMAKE_BUILD_TYPE is used
# For multi-config generators like Visual Studio, CMAKE_CONFIGURATION_TYPES is used
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(DEFAULT_BUILD_TYPE "Release")
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    endif()
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dev" CACHE STRING "Available build types" FORCE)
endif()

# Find SDL2 package
find_package(SDL2 REQUIRED)
if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found. Please install SDL2.")
endif()
include_directories(${SDL2_INCLUDE_DIRS})

# Use pkg-config to find SDL2_ttf and SDL2_image
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

if(NOT SDL2_TTF_FOUND)
    message(FATAL_ERROR "SDL2_ttf not found. Please install SDL2_ttf.")
endif()

if(NOT SDL2_IMAGE_FOUND)
    message(FATAL_ERROR "SDL2_image not found. Please install SDL2_image.")
endif()

# -------------------------------------------------------------------
# **Begin: Adding giflib (libgif) Integration Without pkg-config**

# Find the path to gif_lib.h
find_path(GIFLIB_INCLUDE_DIR
    NAMES gif_lib.h
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES gif
)

# Find the gif library
find_library(GIFLIB_LIBRARY
    NAMES gif giflib
    PATHS /usr/lib /usr/local/lib
    PATH_SUFFIXES gif
)

# Check if both include directory and library are found
if(NOT GIFLIB_INCLUDE_DIR)
    message(FATAL_ERROR "gif_lib.h not found. Please install libgif-dev.")
endif()

if(NOT GIFLIB_LIBRARY)
    message(FATAL_ERROR "libgif.so not found. Please install libgif-dev.")
endif()

# Optionally, you can set a version if needed
# set(GIFLIB_VERSION "5.1.9") # Example version

# Include giflib headers
include_directories(${GIFLIB_INCLUDE_DIR})

# -------------------------------------------------------------------
# **End: Adding giflib (libgif) Integration Without pkg-config**

# Add the src directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add all source files in the src directory
file(GLOB SOURCES "src/*.c")

# Add the executable
add_executable(game ${SOURCES})

# Link the libraries
target_link_libraries(game 
    ${SDL2_LIBRARIES} 
    ${SDL2_TTF_LIBRARIES} 
    ${SDL2_IMAGE_LIBRARIES}
    ${GIFLIB_LIBRARY}  # Link giflib library
)

# Add include directories
target_include_directories(game PRIVATE 
    ${SDL2_TTF_INCLUDE_DIRS} 
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${GIFLIB_INCLUDE_DIR}  # Include giflib directories
)

# Enable compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link to math library on Unix systems
if(UNIX AND NOT APPLE)
    target_link_libraries(game m)
endif()

# Compiler and Linker Options
if(MSVC)
    # Define build configurations for MSVC
    target_compile_options(game PRIVATE
        /W4
        $<$<CONFIG:Release>:/Ox;/GL>
        $<$<CONFIG:Debug>:/Od;/Zi>
        $<$<CONFIG:Dev>:/O2;/Zi>
        /WX
        /permissive-
        /wd4996
    )
    target_link_options(game PRIVATE
        $<$<CONFIG:Release>:/LTCG>
        $<$<CONFIG:Debug>:/DEBUG>         # Enable full debug info for Debug
        $<$<CONFIG:Dev>:/DEBUG>
    )
else()
    # Define build configurations for GCC/Clang
    target_compile_options(game PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wformat=2
        -Wno-unused-parameter
        -Wshadow
        -Wwrite-strings
        -Wstrict-prototypes
        -Wold-style-definition
        -Wredundant-decls
        -Wnested-externs
        -Wmissing-include-dirs
        ${SDL2_TTF_CFLAGS_OTHER}
        ${SDL2_IMAGE_CFLAGS_OTHER}
        # Configuration-specific flags
        $<$<CONFIG:Release>:-O3;-march=native;-flto>
        $<$<CONFIG:Debug>:-O0;-g>
        $<$<CONFIG:Dev>:-O2;-g;-march=native>
    )
    target_link_options(game PRIVATE
        # Configuration-specific linker flags
        $<$<CONFIG:Release>:-flto>
    )
    
    # Optionally enable Address Sanitizer for Debug builds
    # Uncomment the following lines if desired
    # if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    #     target_compile_options(game PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
    #     target_link_options(game PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    # endif()
    
    # Disable -Werror in Release and Dev to prevent build from failing on warnings
    target_compile_options(game PRIVATE
        $<$<CONFIG:Release>:-Wno-error>
        $<$<CONFIG:Dev>:-Wno-error>
    )
endif()

# Define custom build type descriptions (optional)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release Dev)

# Ensure CMake policies are up-to-date
cmake_policy(SET CMP0048 NEW)
