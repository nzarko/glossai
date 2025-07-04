# Tests CMakeLists.txt for GlossAI

# Enable testing
enable_testing()

# Find required packages
find_package(Threads REQUIRED)

# Include core library headers
include_directories(${CMAKE_SOURCE_DIR}/core)

# Define test executable for interpreter tests
add_executable(test_interpreter
    test_interpreter.cpp
)

# Link with the core library
target_link_libraries(test_interpreter
    glossai_core
    Threads::Threads
)

# Define test executable for calculation integration tests
add_executable(test_calc_integration
    test_calc_integration.cpp
)

# Link with the core library
target_link_libraries(test_calc_integration
    glossai_core
    Threads::Threads
)

# Add tests to CTest
add_test(NAME InterpreterTests COMMAND test_interpreter)
add_test(NAME CalculationIntegrationTests COMMAND test_calc_integration)

# Set test properties
set_tests_properties(InterpreterTests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(CalculationIntegrationTests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
