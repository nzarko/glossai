#include "lexer.h"
#include <unordered_map>
#include <cctype>
#include <algorithm>

Lexer::Lexer()
    : m_position(0)
    , m_currentLine(1)
    , m_currentColumn(1)
    , m_currentChar('\0')
{
}

std::vector<Token> Lexer::tokenize(const std::string &input)
{
    m_input = input;
    m_position = 0;
    m_currentLine = 1;
    m_currentColumn = 1;
    m_currentChar = m_input.empty() ? '\0' : m_input[0];
    
    std::vector<Token> tokens;
    
    while (m_currentChar != '\0') {
        if (std::isspace(m_currentChar)) {
            skipWhitespace();
            continue;
        }
        
        if (isDigit(m_currentChar)) {
            tokens.push_back(readNumber());
        } else if (m_currentChar == '"' || m_currentChar == '\'') {
            tokens.push_back(readString());
        } else if (isAlpha(m_currentChar) || m_currentChar == '_') {
            tokens.push_back(readIdentifier());
        } else {
            tokens.push_back(readOperator());
        }
    }
    
    tokens.emplace_back(TokenType::EndOfFile, "", m_currentLine, m_currentColumn);
    return tokens;
}

void Lexer::skipWhitespace()
{
    while (std::isspace(m_currentChar)) {
        advance();
    }
}

Token Lexer::readNumber()
{
    std::string number;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    while (isDigit(m_currentChar) || m_currentChar == '.') {
        number += m_currentChar;
        advance();
    }
    
    return Token(TokenType::Number, number, startLine, startColumn);
}

Token Lexer::readString()
{
    char quote = m_currentChar;
    std::string string;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    advance(); // Skip opening quote
    
    while (m_currentChar != '\0' && m_currentChar != quote) {
        if (m_currentChar == '\\') {
            advance();
            if (m_currentChar != '\0') {
                switch (m_currentChar) {
                case 'n': string += '\n'; break;
                case 't': string += '\t'; break;
                case 'r': string += '\r'; break;
                case '\\': string += '\\'; break;
                case '"': string += '"'; break;
                case '\'': string += '\''; break;
                default: string += m_currentChar; break;
                }
                advance();
            }
        } else {
            string += m_currentChar;
            advance();
        }
    }
    
    if (m_currentChar == quote) {
        advance(); // Skip closing quote
    }
    
    return Token(TokenType::String, string, startLine, startColumn);
}

Token Lexer::readIdentifier()
{
    std::string identifier;
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    while (isAlphaNumeric(m_currentChar) || m_currentChar == '_') {
        identifier += m_currentChar;
        advance();
    }
    
    // Check for keywords
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"function", TokenType::Function},
        {"return", TokenType::Return},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not}
    };
    
    // Convert to lowercase for case-insensitive keyword matching
    std::string lowerIdentifier = identifier;
    std::transform(lowerIdentifier.begin(), lowerIdentifier.end(), lowerIdentifier.begin(), ::tolower);
    
    auto it = keywords.find(lowerIdentifier);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::Identifier;
    
    return Token(type, identifier, startLine, startColumn);
}

Token Lexer::readOperator()
{
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    char current = m_currentChar;
    
    advance();
    
    switch (current) {
    case '+': return Token(TokenType::Plus, "+", startLine, startColumn);
    case '-': return Token(TokenType::Minus, "-", startLine, startColumn);
    case '*': 
        if (m_currentChar == '*') {
            advance();
            return Token(TokenType::Power, "**", startLine, startColumn);
        }
        return Token(TokenType::Multiply, "*", startLine, startColumn);
    case '/': return Token(TokenType::Divide, "/", startLine, startColumn);
    case '=':
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::Equal, "==", startLine, startColumn);
        }
        return Token(TokenType::Assign, "=", startLine, startColumn);
    case '!':
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::NotEqual, "!=", startLine, startColumn);
        }
        return Token(TokenType::Not, "!", startLine, startColumn);
    case '<':
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::LessEqual, "<=", startLine, startColumn);
        }
        return Token(TokenType::Less, "<", startLine, startColumn);
    case '>':
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::GreaterEqual, ">=", startLine, startColumn);
        }
        return Token(TokenType::Greater, ">", startLine, startColumn);
    case '(': return Token(TokenType::LeftParen, "(", startLine, startColumn);
    case ')': return Token(TokenType::RightParen, ")", startLine, startColumn);
    case '{': return Token(TokenType::LeftBrace, "{", startLine, startColumn);
    case '}': return Token(TokenType::RightBrace, "}", startLine, startColumn);
    case ',': return Token(TokenType::Comma, ",", startLine, startColumn);
    case ';': return Token(TokenType::Semicolon, ";", startLine, startColumn);
    default:
        return Token(TokenType::Invalid, std::string(1, current), startLine, startColumn);
    }
}

bool Lexer::isDigit(char c) const
{
    return std::isdigit(c);
}

bool Lexer::isAlpha(char c) const
{
    return std::isalpha(c);
}

bool Lexer::isAlphaNumeric(char c) const
{
    return std::isalnum(c);
}

void Lexer::advance()
{
    if (m_currentChar == '\n') {
        m_currentLine++;
        m_currentColumn = 1;
    } else {
        m_currentColumn++;
    }
    
    m_position++;
    if (m_position >= m_input.length()) {
        m_currentChar = '\0';
    } else {
        m_currentChar = m_input[m_position];
    }
}

char Lexer::peekChar(int offset) const
{
    size_t pos = m_position + offset;
    if (pos >= m_input.length()) {
        return '\0';
    }
    return m_input[pos];
}
