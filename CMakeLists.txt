cmake_minimum_required(VERSION 3.10)
project(SoftwareRenderer C)
set(CMAKE_C_STANDARD 11)

# Find SDL2 package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Use pkg-config to find SDL2_ttf and SDL2_image
include(FindPkgConfig)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

if(NOT SDL2_TTF_FOUND)
    message(FATAL_ERROR "SDL2_ttf not found. Please install SDL2_ttf.")
endif()

if(NOT SDL2_IMAGE_FOUND)
    message(FATAL_ERROR "SDL2_image not found. Please install SDL2_image.")
endif()

# Add the src directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add all source files in the src directory
file(GLOB SOURCES "src/*.c")

# Add the executable
add_executable(game ${SOURCES})

# Link the libraries
target_link_libraries(game ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

# Add include directories
target_include_directories(game PRIVATE ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# On some systems, you might need to link to m (math library)
if(UNIX AND NOT APPLE)
    target_link_libraries(game m)
endif()

# Add compiler warnings and other flags
if(MSVC)
    target_compile_options(game PRIVATE
        /W4 /WX /permissive- /wd4996
        $<$<CONFIG:RELEASE>:/O2>
        $<$<CONFIG:DEBUG>:/Od /Zi>
    )
else()
    target_compile_options(game PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wformat=2 -Wno-unused-parameter -Wshadow
        -Wwrite-strings -Wstrict-prototypes -Wold-style-definition
        -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
        ${SDL2_TTF_CFLAGS_OTHER}
        ${SDL2_IMAGE_CFLAGS_OTHER}
        $<$<CONFIG:RELEASE>:-O2>
        $<$<CONFIG:DEBUG>:-O0 -g>
    )
endif()

# Enable address sanitizer in Debug mode (GCC/Clang only)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(game PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(game PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
endif()