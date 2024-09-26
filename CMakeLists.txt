cmake_minimum_required(VERSION 3.10)
project(SoftwareRenderer C)
set(CMAKE_C_STANDARD 11)

# Find SDL2 package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add the src directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add all source files in the src directory
file(GLOB SOURCES "src/*.c")

# Add the executable
add_executable(game ${SOURCES})

# Link the libraries
target_link_libraries(game ${SDL2_LIBRARIES})
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
        $<$<CONFIG:RELEASE>:-O2>
        $<$<CONFIG:DEBUG>:-O0 -g>
    )
endif()

# Enable address sanitizer in Debug mode (GCC/Clang only)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(game PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(game PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
endif()