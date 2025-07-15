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

// filepath: f:\sources\qt_prjs\glossai\core\interpreter.cpp
std::string Interpreter::execute(const std::string &code)
{
    try {
        m_lastError.clear();

        // Tokenize
        auto tokens = m_lexer->tokenize(code);

        // Parse
        auto ast = m_parser->parse(tokens);

        if (!ast) {
            m_lastError = m_parser->getLastError();
            return "";
        }

        // Check if this is a statement that shouldn't show output
        if (dynamic_cast<PrintNode *>(ast.get()) || 
            dynamic_cast<BlockNode *>(ast.get()) ||
            dynamic_cast<WhileNode *>(ast.get()) ||
            dynamic_cast<ForNode *>(ast.get()) ||
            dynamic_cast<IfNode *>(ast.get())) {
            // For statements that don't need output, just evaluate and return empty string
            m_evaluator->evaluate(ast.get(), m_context.get());
            return ""; // Don't show result
        }

        // Evaluate normally for other expressions
        auto result = m_evaluator->evaluate(ast.get(), m_context.get());

        // Return result as string
        return result.toString();

    } catch (const std::exception &e) {
        m_lastError = e.what();
        return "";
    } catch (...) {
        m_lastError = "Unknown error occurred during execution";
        return "";
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

std::vector<std::string> Interpreter::getBuiltinFunctions() const
{
    return {
        // Trigonometric functions
        "sin", "cos", "tan", "asin", "acos", "atan",
        
        // Logarithmic functions  
        "log", "log10", "log2", "ln", "exp",
        
        // Root functions
        "sqrt", "cbrt", "root",
        
        // Power and exponential
        "pow", "abs",
        
        // Utility functions
        "min", "max", "ceil", "floor", "round"
    };
}
