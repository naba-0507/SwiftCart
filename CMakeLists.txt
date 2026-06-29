cmake_minimum_required(VERSION 3.16)
project(SwiftCart VERSION 1.0 LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Detect SFML from Homebrew (macOS)
if(APPLE)
    execute_process(
        COMMAND brew --prefix sfml
        OUTPUT_VARIABLE HOMEBREW_SFML_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(HOMEBREW_SFML_PREFIX)
        message(STATUS "SFML found at: ${HOMEBREW_SFML_PREFIX}")
        list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_SFML_PREFIX}")
    else()
        message(FATAL_ERROR "SFML not found. Install with: brew install sfml")
    endif()
endif()

# SFML 2.x
find_package(SFML 2 REQUIRED COMPONENTS graphics window system)

# Collect sources
file(GLOB_RECURSE BACKEND_SOURCES
    src/*.cpp
)

list(FILTER BACKEND_SOURCES EXCLUDE REGEX ".*/main_console\\.cpp$")

file(GLOB_RECURSE GUI_SOURCES
    gui/*.cpp
)

add_executable(SwiftCart
    ${BACKEND_SOURCES}
    ${GUI_SOURCES}
)

# Include paths
target_include_directories(SwiftCart PRIVATE
    include
    gui
)

# Link correct SFML 2.x modules
target_link_libraries(SwiftCart PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# Windows: copy DLLs
if(WIN32)
    add_custom_command(TARGET SwiftCart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:SwiftCart>
            $<TARGET_FILE_DIR:SwiftCart>
        COMMAND_EXPAND_LISTS
    )
endif()

# Copy assets folder (only if it exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/gui/assets")
    add_custom_command(TARGET SwiftCart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/gui/assets
            $<TARGET_FILE_DIR:SwiftCart>/assets
    )
endif()

# Create data folder
add_custom_command(TARGET SwiftCart POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:SwiftCart>/data
)

message(STATUS "=== SwiftCart | ${CMAKE_SYSTEM_NAME} | Using SFML 3.x ===")