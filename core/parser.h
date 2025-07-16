#pragma once

#include "lexer.h"
#include "ast.h"
#include <memory>

/**
 * @brief Parser for the GlossAI language
 * 
 * Implements a recursive descent parser that builds an Abstract Syntax Tree (AST)
 * from a sequence of tokens. Pure C++ implementation.
 */
class Parser
{
public:
    Parser();
    
    /**
     * @brief Parse tokens into an AST
     * @param tokens Vector of tokens to parse
     * @return Root node of the AST, or nullptr on error
     */
    std::unique_ptr<ASTNode> parse(const std::vector<Token> &tokens);
    
    /**
     * @brief Get the last parse error
     * @return Error message string
     */
    std::string getLastError() const { return m_lastError; }

private:
    // Token management
    Token currentToken() const;
    Token peekToken(int offset = 1) const;
    void advance();
    bool match(TokenType type);
    bool consume(TokenType type, const std::string &errorMessage);
    
    // Grammar rules (recursive descent)
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<ASTNode> parsePower();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseCall();
    
    // Control flow
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseIfExpression();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parsePrintStatement();
    std::unique_ptr<ASTNode> parseBlock();
    
    // Function definitions
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    std::unique_ptr<ASTNode> parseReturnStatement();
    
    // Utilities
    bool isAtEnd() const;
    void synchronize(); // Error recovery
    
    std::vector<Token> m_tokens;
    size_t m_current;
    std::string m_lastError;
};
