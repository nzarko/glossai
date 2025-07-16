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
    if (m_current >= m_tokens.size()) {
        // Return safe default with proper Token constructor parameters
        return Token{TokenType::EndOfFile, "", 0, 0}; // type, value, line, column
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
    if (currentToken().type == TokenType::If) {
        return parseIfStatement();
    }

    if (currentToken().type == TokenType::While) {
        return parseWhileStatement();
    }

    if (currentToken().type == TokenType::Print) {
        return parsePrintStatement();
    }

    if (currentToken().type == TokenType::LeftBrace) {
        return parseBlock();
    }

    // Otherwise, parse as expression statement
    auto expr = parseExpression();
    
    // Don't consume semicolon here either, let parseBlock handle it
    return expr;
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
    
    if (match(TokenType::PlusAssign)) {
        auto value = parseAssignment();
        return std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::PlusAssign, std::move(value));
    }
    
    if (match(TokenType::MinusAssign)) {
        auto value = parseAssignment();
        return std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::MinusAssign, std::move(value));
    }
    
    if (match(TokenType::MultiplyAssign)) {
        auto value = parseAssignment();
        return std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::MultiplyAssign, std::move(value));
    }
    
    if (match(TokenType::DivideAssign)) {
        auto value = parseAssignment();
        return std::make_unique<BinaryOpNode>(std::move(expr), BinaryOperator::DivideAssign, std::move(value));
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

    while (currentToken().type == TokenType::Multiply || currentToken().type == TokenType::Divide
           || currentToken().type == TokenType::Mod || currentToken().type == TokenType::Div) {
        TokenType op = currentToken().type;
        advance();
        auto right = parseUnary();

        BinaryOperator binOp;
        switch (op) {
        case TokenType::Multiply:
            binOp = BinaryOperator::Multiply;
            break;
        case TokenType::Divide:
            binOp = BinaryOperator::Divide;
            break;
        case TokenType::Mod:
            binOp = BinaryOperator::Mod;
            break;
        case TokenType::Div:
            binOp = BinaryOperator::Div;
            break;
        default:
            binOp = BinaryOperator::Multiply; // fallback
            break;
        }

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
    
    // Handle prefix increment/decrement
    if (currentToken().type == TokenType::Increment) {
        advance();
        auto expr = parseUnary();
        return std::make_unique<UnaryOpNode>(UnaryOperator::PreIncrement, std::move(expr));
    }
    
    if (currentToken().type == TokenType::Decrement) {
        advance();
        auto expr = parseUnary();
        return std::make_unique<UnaryOpNode>(UnaryOperator::PreDecrement, std::move(expr));
    }
    
    return parsePostfix();
}

std::unique_ptr<ASTNode> Parser::parsePostfix()
{
    auto expr = parsePower();
    
    // Handle postfix increment/decrement
    if (currentToken().type == TokenType::Increment) {
        advance();
        return std::make_unique<UnaryOpNode>(UnaryOperator::PostIncrement, std::move(expr));
    }
    
    if (currentToken().type == TokenType::Decrement) {
        advance();
        return std::make_unique<UnaryOpNode>(UnaryOperator::PostDecrement, std::move(expr));
    }
    
    return expr;
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
        
        // Check for function call
        if (currentToken().type == TokenType::LeftParen) {
            advance(); // consume '('
            std::vector<std::unique_ptr<ASTNode>> args;
            
            if (currentToken().type != TokenType::RightParen) {
                do {
                    args.push_back(parseExpression());
                    if (currentToken().type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
            }
            
            consume(TokenType::RightParen, "Expected ')' after function arguments");
            
            // Create IdentifierNode first, then FunctionCallNode
            auto nameNode = std::make_unique<IdentifierNode>(name);
            return std::make_unique<FunctionCallNode>(std::move(nameNode), std::move(args));
        }
        
        return std::make_unique<IdentifierNode>(name);
    }
    
    // Handle if expressions
    if (currentToken().type == TokenType::If) {
        advance(); // consume 'if'
        return parseIfExpression();
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
    consume(TokenType::If, "Expected 'if'");
    consume(TokenType::LeftParen, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "Expected ')' after if condition");

    auto thenBranch = parseStatement();
    std::unique_ptr<ASTNode> elseBranch = nullptr;

    if (match(TokenType::Else)) {
        elseBranch = parseStatement();
    }

    return std::make_unique<IfNode>(std::move(condition),
                                    std::move(thenBranch),
                                    std::move(elseBranch));
}

std::unique_ptr<ASTNode> Parser::parseIfExpression()
{
    consume(TokenType::LeftParen, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "Expected ')' after if condition");

    auto thenBranch = parseExpression();

    consume(TokenType::Else, "Expected 'else' in if expression");

    // Check if the else branch is another if statement
    std::unique_ptr<ASTNode> elseBranch;
    if (currentToken().type == TokenType::If) {
        advance();                        // consume 'if'
        elseBranch = parseIfExpression(); // Recursive call for nested if
    } else {
        elseBranch = parseExpression(); // Regular expression
    }

    return std::make_unique<IfNode>(std::move(condition),
                                    std::move(thenBranch),
                                    std::move(elseBranch));
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement()
{
    consume(TokenType::While, "Expected 'while'");
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

std::unique_ptr<ASTNode> Parser::parsePrintStatement()
{
    consume(TokenType::Print, "Expected 'print'");
    std::vector<std::unique_ptr<ASTNode>> expressions;
    
    // Parse comma-separated expressions
    do {
        expressions.push_back(parseExpression());
        
        if (currentToken().type == TokenType::Comma) {
            advance(); // consume comma
        } else {
            break;
        }
    } while (currentToken().type != TokenType::EndOfFile);
    
    return std::make_unique<PrintNode>(std::move(expressions));
}

std::unique_ptr<ASTNode> Parser::parseBlock()
{
    consume(TokenType::LeftBrace, "Expected '{'");

    std::vector<std::unique_ptr<ASTNode>> statements;

    while (currentToken().type != TokenType::RightBrace
           && currentToken().type != TokenType::EndOfFile) {
        
        statements.push_back(parseStatement());
        
        // Handle semicolon between statements
        if (currentToken().type == TokenType::Semicolon) {
            advance(); // consume semicolon
        }
        // If the next token is not '}' and not EOF, and we didn't consume a semicolon,
        // that might be okay for some statements (like if, while, blocks)
    }

    consume(TokenType::RightBrace, "Expected '}'");

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
    return m_current >= m_tokens.size()
           || (m_current < m_tokens.size() && m_tokens[m_current].type == TokenType::EndOfFile);
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd()) {
        if (m_tokens[m_current - 1].type == TokenType::Semicolon) {
            return;
        }

        // Add bounds check before accessing currentToken
        if (m_current >= m_tokens.size()) {
            break;
        }

        switch (currentToken().type) {
        case TokenType::If:
        case TokenType::While:
        case TokenType::For:
        case TokenType::Function:
            return;
        default:
            advance();
            break;
        }
    }
}
