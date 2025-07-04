#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"
#include "context.h"
#include "ast.h"
#include <stdexcept>

Interpreter::Interpreter()
    : m_lexer(std::make_unique<Lexer>())
    , m_parser(std::make_unique<Parser>())
    , m_evaluator(std::make_unique<Evaluator>())
    , m_context(std::make_unique<Context>())
{
}

Interpreter::~Interpreter() = default;

std::string Interpreter::execute(const std::string &code)
{
    try {
        m_lastError.clear();
        
        // Tokenize the input
        auto tokens = m_lexer->tokenize(code);
        
        // Parse into AST
        auto ast = m_parser->parse(tokens);
        if (!ast) {
            m_lastError = "Parse error: Invalid syntax";
            return std::string();
        }
        
        // Evaluate the AST
        auto result = m_evaluator->evaluate(ast.get(), m_context.get());
        return result.toString();
        
    } catch (const std::exception &e) {
        m_lastError = std::string("Runtime error: ") + e.what();
        return std::string();
    }
}

std::vector<std::string> Interpreter::executeMultiple(const std::vector<std::string> &lines)
{
    std::vector<std::string> results;
    for (const std::string &line : lines) {
        if (!line.empty()) {
            // Trim whitespace
            std::string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
            trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
            
            if (!trimmed.empty()) {
                results.push_back(execute(trimmed));
            }
        }
    }
    return results;
}

bool Interpreter::isValidSyntax(const std::string &code)
{
    try {
        auto tokens = m_lexer->tokenize(code);
        auto ast = m_parser->parse(tokens);
        return ast != nullptr;
    } catch (...) {
        return false;
    }
}

std::string Interpreter::getLastError() const
{
    return m_lastError;
}

void Interpreter::clearContext()
{
    m_context = std::make_unique<Context>();
}

std::vector<std::string> Interpreter::getAvailableIdentifiers() const
{
    return m_context->getIdentifiers();
}
