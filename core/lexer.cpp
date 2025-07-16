#include "lexer.h"
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <stdexcept>

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
    int startLine = m_currentLine;
    int startColumn = m_currentColumn;
    
    advance(); // Skip opening quote
    std::string value;
    
    while (m_currentChar != '"' && m_currentChar != '\0') {
        if (m_currentChar == '\\') {
            advance();
            switch (m_currentChar) {
            case 'n': value += '\n'; break;
            case 't': value += '\t'; break;
            case 'r': value += '\r'; break;
            case '\\': value += '\\'; break;
            case '"': value += '"'; break;
            default: value += m_currentChar; break;
            }
        } else {
            value += m_currentChar;
        }
        advance();
    }
    
    if (m_currentChar == '"') {
        advance(); // Skip closing quote
    } else {
        throw std::runtime_error("Unterminated string literal");
    }
    
    return Token(TokenType::String, value, startLine, startColumn);
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

    // Convert to lowercase for case-insensitive matching
    std::string lowerIdentifier = identifier;
    std::transform(lowerIdentifier.begin(),
                   lowerIdentifier.end(),
                   lowerIdentifier.begin(),
                   ::tolower);

    // Mathematical constants - convert directly to numbers
    static const std::unordered_map<std::string, std::string> constants = {
        {"pi", "3.14159265358979323846"},
        {"e", "2.71828182845904523536"},
        {"tau", "6.28318530717958647692"},   // tau = 2*pi
        {"phi", "1.61803398874989484820"},   // Golden ratio
        {"sqrt2", "1.41421356237309504880"}, // √2
        {"sqrt3", "1.73205080756887729353"}, // √3
        {"ln2", "0.69314718055994530942"},   // ln(2)
        {"ln10", "2.30258509299404568402"}   // ln(10)
    };

    auto constIt = constants.find(lowerIdentifier);
    if (constIt != constants.end()) {
        return Token(TokenType::Number, constIt->second, startLine, startColumn);
    }

    // Check for keywords
    static const std::unordered_map<std::string, TokenType> keywords = {{"if", TokenType::If},
                                                                        {"else", TokenType::Else},
                                                                        {"while", TokenType::While},
                                                                        {"for", TokenType::For},
                                                                        {"function",
                                                                         TokenType::Function},
                                                                        {"procedure", TokenType::Procedure},
                                                                        {"return",
                                                                         TokenType::Return},
                                                                        {"print", TokenType::Print},
                                                                        {"true", TokenType::True},
                                                                        {"false", TokenType::False},
                                                                        {"and", TokenType::And},
                                                                        {"or", TokenType::Or},
                                                                        {"not", TokenType::Not},
                                                                        {"mod", TokenType::Mod},
                                                                        {"div", TokenType::Div}};

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
    case '+': 
        if (m_currentChar == '+') {
            advance();
            return Token(TokenType::Increment, "++", startLine, startColumn);
        }
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::PlusAssign, "+=", startLine, startColumn);
        }
        return Token(TokenType::Plus, "+", startLine, startColumn);
    case '-': 
        if (m_currentChar == '-') {
            advance();
            return Token(TokenType::Decrement, "--", startLine, startColumn);
        }
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::MinusAssign, "-=", startLine, startColumn);
        }
        return Token(TokenType::Minus, "-", startLine, startColumn);
    case '*': 
        if (m_currentChar == '*') {
            advance();
            return Token(TokenType::Power, "**", startLine, startColumn);
        }
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::MultiplyAssign, "*=", startLine, startColumn);
        }
        return Token(TokenType::Multiply, "*", startLine, startColumn);
    case '/': 
        if (m_currentChar == '=') {
            advance();
            return Token(TokenType::DivideAssign, "/=", startLine, startColumn);
        }
        return Token(TokenType::Divide, "/", startLine, startColumn);
    case '^': return Token(TokenType::Power, "^", startLine, startColumn);
    case '%': return Token(TokenType::Mod, "%", startLine, startColumn);
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
