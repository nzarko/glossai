#pragma once

#include <string>
#include <vector>
#include <memory>

class Lexer;
class Parser;
class Evaluator;
class Context;
class ASTNode;

/**
 * @brief Main GlossAI interpreter class
 * 
 * This class provides the main interface for the GlossAI language interpreter.
 * It coordinates the lexical analysis, parsing, and evaluation phases.
 * Uses only standard C++ - no Qt dependencies.
 */
class Interpreter
{
public:
    Interpreter();
    ~Interpreter();

    /**
     * @brief Execute a single line of GlossAI code
     * @param code The code to execute
     * @return The result of execution as a string
     */
    std::string execute(const std::string &code);

    /**
     * @brief Execute multiple lines of GlossAI code
     * @param lines The lines of code to execute
     * @return Vector of results for each line
     */
    std::vector<std::string> executeMultiple(const std::vector<std::string> &lines);

    /**
     * @brief Check if the given code is syntactically valid
     * @param code The code to validate
     * @return True if valid, false otherwise
     */
    bool isValidSyntax(const std::string &code);

    /**
     * @brief Get the last error message
     * @return Error message string
     */
    std::string getLastError() const;

    /**
     * @brief Clear the interpreter context
     */
    void clearContext();

    /**
     * @brief Get available functions and variables
     * @return Vector of available identifiers
     */
    std::vector<std::string> getAvailableIdentifiers() const;

    /**
     * @brief Get list of available built-in functions
     * @return Vector of built-in function names
     */
    std::vector<std::string> getBuiltinFunctions() const;

private:
    std::unique_ptr<Lexer> m_lexer;
    std::unique_ptr<Parser> m_parser;
    std::unique_ptr<Evaluator> m_evaluator;
    std::unique_ptr<Context> m_context;
    std::string m_lastError;
};
