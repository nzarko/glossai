#pragma once

#include <string>
#include <vector>

/**
 * @brief Token types for the GlossAI language
 */
enum class TokenType {
    // Literals
    Number,
    String,
    Identifier,
    
    // Operators
    Plus,
    Minus,
    Multiply,
    Divide,
    Power,
    Assign,
    
    // Comparison
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    
    // Logical
    And,
    Or,
    Not,
    
    // Delimiters
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Semicolon,
    
    // Keywords
    If,
    Else,
    While,
    For,
    Function,
    Return,
    True,
    False,
    
    // Special
    EndOfFile,
    Invalid
};

/**
 * @brief Represents a single token in the source code
 */
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t = TokenType::Invalid, const std::string &v = std::string(), int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
};

/**
 * @brief Lexical analyzer for the GlossAI language
 * Pure C++ implementation using only standard library
 */
class Lexer
{
public:
    Lexer();
    
    /**
     * @brief Tokenize the input string
     * @param input The source code to tokenize
     * @return Vector of tokens
     */
    std::vector<Token> tokenize(const std::string &input);
    
    /**
     * @brief Get the current line number
     */
    int getCurrentLine() const { return m_currentLine; }
    
    /**
     * @brief Get the current column number
     */
    int getCurrentColumn() const { return m_currentColumn; }

private:
    void skipWhitespace();
    Token readNumber();
    Token readString();
    Token readIdentifier();
    Token readOperator();
    
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    
    std::string m_input;
    size_t m_position;
    int m_currentLine;
    int m_currentColumn;
    char m_currentChar;
    
    void advance();
    char peekChar(int offset = 1) const;
};
