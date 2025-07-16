# Core library - Pure C++ implementation (no Qt dependencies)
add_library(glossai_core STATIC
    interpreter.h
    interpreter.cpp
    lexer.h
    lexer.cpp
    parser.h
    parser.cpp
    ast.h
    ast.cpp
    evaluator.h
    evaluator.cpp
    context.h
    context.cpp
    lineparser.h
    lineparser.cpp
)

target_include_directories(glossai_core 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Use only standard C++ libraries
target_compile_features(glossai_core PUBLIC cxx_std_17)

# Optional: Find and link Boost if available (uncomment if you want to use Boost)
# find_package(Boost OPTIONAL_COMPONENTS system filesystem)
# if(Boost_FOUND)
#     target_link_libraries(glossai_core PRIVATE Boost::system Boost::filesystem)
#     target_compile_definitions(glossai_core PRIVATE USE_BOOST)
# endif()

# Link standard math library on Unix systems
if(UNIX)
    target_link_libraries(glossai_core PRIVATE m)
endif()
