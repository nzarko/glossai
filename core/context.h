#pragma once

#include "ast.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class ASTNode;

/**
 * @brief Represents a user-defined function
 */
struct UserFunction {
    std::string name;
    std::vector<std::string> parameters;
    ASTNode *body; // Note: We don't own this pointer
};

/**
 * @brief Execution context for the GlossAI interpreter
 * 
 * Manages variables, functions, and scoping for the interpreter.
 * Supports nested scopes for blocks and function calls.
 * Pure C++ implementation using standard containers.
 */
class Context
{
public:
    Context();
    ~Context();

    // Variable management
    /**
     * @brief Set a variable in the current scope
     * @param name Variable name
     * @param value Variable value
     */
    void setVariable(const std::string &name, const Value &value);
    
    /**
     * @brief Get a variable value
     * @param name Variable name
     * @return Variable value
     */
    Value getVariable(const std::string &name) const;
    
    /**
     * @brief Check if a variable exists
     * @param name Variable name
     * @return True if variable exists
     */
    bool hasVariable(const std::string &name) const;
    
    /**
     * @brief Remove a variable from the current scope
     * @param name Variable name
     */
    void removeVariable(const std::string &name);

    // Function management
    /**
     * @brief Define a function
     * @param name Function name
     * @param function Function definition
     */
    void setFunction(const std::string &name, const UserFunction &function);
    
    /**
     * @brief Get a function definition
     * @param name Function name
     * @return Function definition
     */
    UserFunction getFunction(const std::string &name) const;
    
    /**
     * @brief Check if a function exists
     * @param name Function name
     * @return True if function exists
     */
    bool hasFunction(const std::string &name) const;
    
    /**
     * @brief Remove a function
     * @param name Function name
     */
    void removeFunction(const std::string &name);

    // Scope management
    /**
     * @brief Push a new scope (for blocks, function calls)
     */
    void pushScope();
    
    /**
     * @brief Pop the current scope
     */
    void popScope();
    
    /**
     * @brief Get the current scope depth
     * @return Number of nested scopes
     */
    int getScopeDepth() const;

    // Utility methods
    /**
     * @brief Get all available identifiers (variables and functions)
     * @return Vector of identifier names
     */
    std::vector<std::string> getIdentifiers() const;
    
    /**
     * @brief Clear all variables and functions
     */
    void clear();
    
    /**
     * @brief Get all variables in the current scope
     * @return Map of variable names to values
     */
    std::unordered_map<std::string, Value> getCurrentScopeVariables() const;
    
    /**
     * @brief Get all defined functions
     * @return Map of function names to definitions
     */
    std::unordered_map<std::string, UserFunction> getFunctions() const;

    // Line parsing functionality
    /**
     * @brief Add a line to the pending code buffer
     * @param line Line to add
     */
    void addPendingLine(const std::string &line);
    
    /**
     * @brief Get the accumulated pending code
     * @return Combined pending lines
     */
    std::string getPendingCode() const;
    
    /**
     * @brief Clear all pending lines
     */
    void clearPendingLines();
    
    /**
     * @brief Check if there are pending lines
     * @return True if there are pending lines
     */
    bool hasPendingLines() const;
    
    /**
     * @brief Get the current brace nesting level
     * @return Brace level (positive = unclosed braces)
     */
    int getBraceLevel() const;
    
    /**
     * @brief Get the current parenthesis nesting level
     * @return Parenthesis level (positive = unclosed parentheses)
     */
    int getParenLevel() const;

private:
    using VariableScope = std::unordered_map<std::string, Value>;
    
    std::vector<VariableScope> m_variableScopes;
    std::unordered_map<std::string, UserFunction> m_functions;
    std::vector<std::string> m_pendingLines;  // For multi-line parsing
    
    // Helper methods
    Value* findVariable(const std::string &name) const;
};
