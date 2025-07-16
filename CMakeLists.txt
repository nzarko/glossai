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

# Main GUI executable
if(WIN32)
    add_executable(glossai main.cpp glossai.rc)
else()
    add_executable(glossai main.cpp)
endif()

target_link_libraries(glossai
    PRIVATE
    glossai_core
    glossai_ui
    Qt6::Core
    Qt6::Widgets
)

# Command-line executable (no Qt dependencies)
if(WIN32)
    add_executable(glossai_cmd cmd_main.cpp glossai_cmd.rc)
else()
    add_executable(glossai_cmd cmd_main.cpp)
endif()

target_link_libraries(glossai_cmd
    PRIVATE
    glossai_core
)

# Set output directory for both executables
set_target_properties(glossai glossai_cmd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS glossai glossai_cmd
    RUNTIME DESTINATION bin
)

# Add after your executable definitions

# if(WIN32)
#     # Find Qt installation path
#     get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
#     get_filename_component(QT_WINDEPLOYQT_EXECUTABLE ${QT_QMAKE_EXECUTABLE} PATH)
#     set(QT_WINDEPLOYQT_EXECUTABLE "${QT_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

#     # Deploy Qt libraries for the GUI application
#     add_custom_command(TARGET glossai POST_BUILD
#         COMMAND ${QT_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE:glossai>
#         COMMENT "Deploying Qt libraries")
# endif()