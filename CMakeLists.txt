cmake_minimum_required(VERSION 3.16)
project(GlossAI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt installation path
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/msvc2022_64")
set(Qt6_DIR "C:/Qt/6.8.1/msvc2022_64/lib/cmake/Qt6")

# Find required Qt components (only for UI)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt's MOC, RCC, and UIC for UI components
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add subdirectories
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(tests)

# Main executable
add_executable(glossai main.cpp)

target_link_libraries(glossai
    PRIVATE
    glossai_core
    glossai_ui
    Qt6::Core
    Qt6::Widgets
)

# Set output directory
set_target_properties(glossai PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS glossai
    RUNTIME DESTINATION bin
)
