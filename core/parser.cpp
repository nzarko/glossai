#include "parser.h"
#include <stdexcept>

Parser::Parser()
    : m_current(0)
{
}

std::unique_ptr<ASTNode> Parser::parse(const std::vector<Token> &tokens)
{
    m_tokens = tokens;
    m_current = 0;
    m_lastError.clear();
    
    try {
        return parseStatement();
    } catch (const std::exception &e) {
        m_lastError = e.what();
        return nullptr;
    }
}

Token Parser::currentToken() const
{
    if (isAtEnd()) {
        return m_tokens.back(); // Should be EndOfFile token
    }
    return m_tokens[m_current];
}

Token Parser::peekToken(int offset) const
{
    size_t pos = m_current + offset;
    if (pos >= m_tokens.size()) {
        return m_tokens.back(); // EndOfFile token
    }
    return m_tokens[pos];
}

void Parser::advance()
{
    if (!isAtEnd()) {
        m_current++;
    }
}

bool Parser::match(TokenType type)
{
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::consume(TokenType type, const std::string &errorMessage)
{
    if (currentToken().type == type) {
        advance();
        return true;
    }
    m_lastError = errorMessage;
    throw std::runtime_error(errorMessage);
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    if (match(TokenType::If)) {
        return parseIfStatement();
    }
    if (match(TokenType::While)) {
        return parseWhileStatement();
    }
    if (match(TokenType::For)) {
        return parseForStatement();
    }
    if (match(TokenType::Function)) {
        return parseFunctionDeclaration();
    }
    if (match(TokenType::Return)) {
        return parseReturnStatement();
    }
    if (match(TokenType::LeftBrace)) {
        return parseBlock();
    }
    
    return parseExpression();
}

std::unique_ptr<ASTNode> Parser::parseExpression()
{
    return parseAssignment();
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    auto expr = parseLogicalOr();
    
    if (match(TokenType::Assign)) {
        auto value = parseAssignment();
        return std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::Assign, std::move(value));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr()
{
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::Or)) {
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::Or, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd()
{
    auto expr = parseEquality();
    
    while (match(TokenType::And)) {
        auto right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::And, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseEquality()
{
    auto expr = parseComparison();
    
    while (currentToken().type == TokenType::Equal || currentToken().type == TokenType::NotEqual) {
        TokenType op = currentToken().type;
        advance();
        auto right = parseComparison();
        
        BinaryOperator binOp = (op == TokenType::Equal) ? BinaryOperator::Equal : BinaryOperator::NotEqual;
        expr = std::make_unique<BinaryOpNode>(std::move(expr), binOp, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseComparison()
{
    auto expr = parseTerm();
    
    while (currentToken().type == TokenType::Less || currentToken().type == TokenType::Greater ||
           currentToken().type == TokenType::LessEqual || currentToken().type == TokenType::GreaterEqual) {
        TokenType op = currentToken().type;
        advance();
        auto right = parseTerm();
        
        BinaryOperator binOp;
        switch (op) {
        case TokenType::Less: binOp = BinaryOperator::Less; break;
        case TokenType::Greater: binOp = BinaryOperator::Greater; break;
        case TokenType::LessEqual: binOp = BinaryOperator::LessEqual; break;
        case TokenType::GreaterEqual: binOp = BinaryOperator::GreaterEqual; break;
        default: binOp = BinaryOperator::Less; break;
        }
        
        expr = std::make_unique<BinaryOpNode>(std::move(expr), binOp, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseTerm()
{
    auto expr = parseFactor();
    
    while (currentToken().type == TokenType::Plus || currentToken().type == TokenType::Minus) {
        TokenType op = currentToken().type;
        advance();
        auto right = parseFactor();
        
        BinaryOperator binOp = (op == TokenType::Plus) ? BinaryOperator::Add : BinaryOperator::Subtract;
        expr = std::make_unique<BinaryOpNode>(std::move(expr), binOp, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
    auto expr = parseUnary();
    
    while (currentToken().type == TokenType::Multiply || currentToken().type == TokenType::Divide) {
        TokenType op = currentToken().type;
        advance();
        auto right = parseUnary();
        
        BinaryOperator binOp = (op == TokenType::Multiply) ? BinaryOperator::Multiply : BinaryOperator::Divide;
        expr = std::make_unique<BinaryOpNode>(std::move(expr), binOp, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseUnary()
{
    if (currentToken().type == TokenType::Minus || currentToken().type == TokenType::Not) {
        TokenType op = currentToken().type;
        advance();
        auto expr = parseUnary();
        
        UnaryOperator unOp = (op == TokenType::Minus) ? UnaryOperator::Negate : UnaryOperator::Not;
        return std::make_unique<UnaryOpNode>(unOp, std::move(expr));
    }
    
    return parsePower();
}

std::unique_ptr<ASTNode> Parser::parsePower()
{
    auto expr = parseCall();
    
    if (match(TokenType::Power)) {
        auto right = parseUnary(); // Right associative
        return std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::Power, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseCall()
{
    auto expr = parsePrimary();
    
    while (match(TokenType::LeftParen)) {
        std::vector<std::unique_ptr<ASTNode>> arguments;
        
        if (currentToken().type != TokenType::RightParen) {
            do {
                arguments.push_back(parseExpression());
            } while (match(TokenType::Comma));
        }
        
        consume(TokenType::RightParen, "Expected ')' after function arguments");
        expr = std::make_unique<FunctionCallNode>(std::move(expr), std::move(arguments));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    if (match(TokenType::True)) {
        return std::make_unique<LiteralNode>(Value(true));
    }
    
    if (match(TokenType::False)) {
        return std::make_unique<LiteralNode>(Value(false));
    }
    
    if (currentToken().type == TokenType::Number) {
        std::string value = currentToken().value;
        advance();
        return std::make_unique<LiteralNode>(Value(std::stod(value)));
    }
    
    if (currentToken().type == TokenType::String) {
        std::string value = currentToken().value;
        advance();
        return std::make_unique<LiteralNode>(Value(value));
    }
    
    if (currentToken().type == TokenType::Identifier) {
        std::string name = currentToken().value;
        advance();
        return std::make_unique<IdentifierNode>(name);
    }
    
    if (match(TokenType::LeftParen)) {
        auto expr = parseExpression();
        consume(TokenType::RightParen, "Expected ')' after expression");
        return expr;
    }
    
    m_lastError = "Unexpected token: " + currentToken().value;
    throw std::runtime_error(m_lastError);
}

std::unique_ptr<ASTNode> Parser::parseIfStatement()
{
    consume(TokenType::LeftParen, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "Expected ')' after if condition");
    
    auto thenBranch = parseStatement();
    std::unique_ptr<ASTNode> elseBranch = nullptr;
    
    if (match(TokenType::Else)) {
        elseBranch = parseStatement();
    }
    
    return std::make_unique<IfNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement()
{
    consume(TokenType::LeftParen, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "Expected ')' after while condition");
    
    auto body = parseStatement();
    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseForStatement()
{
    consume(TokenType::LeftParen, "Expected '(' after 'for'");
    
    auto init = parseStatement();
    consume(TokenType::Semicolon, "Expected ';' after for loop initializer");
    
    auto condition = parseExpression();
    consume(TokenType::Semicolon, "Expected ';' after for loop condition");
    
    auto update = parseExpression();
    consume(TokenType::RightParen, "Expected ')' after for loop clauses");
    
    auto body = parseStatement();
    return std::make_unique<ForNode>(std::move(init), std::move(condition), std::move(update), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseBlock()
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (currentToken().type != TokenType::RightBrace && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    
    consume(TokenType::RightBrace, "Expected '}' after block");
    return std::make_unique<BlockNode>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration()
{
    consume(TokenType::Identifier, "Expected function name");
    std::string name = m_tokens[m_current - 1].value;
    
    consume(TokenType::LeftParen, "Expected '(' after function name");
    
    std::vector<std::string> parameters;
    if (currentToken().type != TokenType::RightParen) {
        do {
            consume(TokenType::Identifier, "Expected parameter name");
            parameters.push_back(m_tokens[m_current - 1].value);
        } while (match(TokenType::Comma));
    }
    
    consume(TokenType::RightParen, "Expected ')' after parameters");
    
    auto body = parseStatement();
    return std::make_unique<FunctionDefNode>(name, parameters, std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseReturnStatement()
{
    std::unique_ptr<ASTNode> value = nullptr;
    
    if (currentToken().type != TokenType::Semicolon && !isAtEnd()) {
        value = parseExpression();
    }
    
    return std::make_unique<ReturnNode>(std::move(value));
}

bool Parser::isAtEnd() const
{
    return m_current >= m_tokens.size() || currentToken().type == TokenType::EndOfFile;
}

void Parser::synchronize()
{
    advance();
    
    while (!isAtEnd()) {
        if (m_tokens[m_current - 1].type == TokenType::Semicolon) {
            return;
        }
        
        switch (currentToken().type) {
        case TokenType::If:
        case TokenType::While:
        case TokenType::For:
        case TokenType::Function:
        case TokenType::Return:
            return;
        default:
            break;
        }
        
        advance();
    }
}
