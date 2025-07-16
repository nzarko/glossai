#include "evaluator.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

Evaluator::Evaluator()
    : m_context(nullptr), m_result(), m_hasReturned(false)
{
}

Evaluator::~Evaluator() = default;

Value Evaluator::evaluate(ASTNode *node, Context *context)
{
    if (!node)
    {
        throw std::runtime_error("Cannot evaluate null AST node");
    }

    m_context = context;
    m_hasReturned = false;
    m_result = Value();

    node->accept(this);
    return m_result;
}

void Evaluator::visit(LiteralNode *node)
{
    m_result = node->getValue();
}

void Evaluator::visit(IdentifierNode *node)
{
    std::string name = node->getName();

    // Check for mathematical constants first
    if (name == "pi")
    {
        m_result = Value(M_PI);
        return;
    }
    else if (name == "e")
    {
        m_result = Value(M_E);
        return;
    }

    // Check for variables in context
    if (m_context && m_context->hasVariable(name))
    {
        m_result = m_context->getVariable(name);
        return;
    }

    // Variable not found
    throw std::runtime_error("Undefined variable: " + name);
}

void Evaluator::visit(BinaryOpNode *node)
{
    // Handle assignment operators (without evaluating left operand)
    if (node->getOperator() == BinaryOperator::Assign || 
        node->getOperator() == BinaryOperator::PlusAssign ||
        node->getOperator() == BinaryOperator::MinusAssign ||
        node->getOperator() == BinaryOperator::MultiplyAssign ||
        node->getOperator() == BinaryOperator::DivideAssign) {
        
        if (auto *identNode = dynamic_cast<IdentifierNode *>(node->getLeft())) {
            Value right = evaluate(node->getRight(), m_context);
            
            if (node->getOperator() == BinaryOperator::Assign) {
                if (m_context) {
                    m_context->setVariable(identNode->getName(), right);
                    m_result = right;
                } else {
                    throw std::runtime_error("No context for variable assignment");
                }
            } else {
                // For compound assignments, get the current value
                Value current;
                if (m_context && m_context->hasVariable(identNode->getName())) {
                    current = m_context->getVariable(identNode->getName());
                } else {
                    throw std::runtime_error("Variable not found for compound assignment: " + identNode->getName());
                }
                
                Value newValue;
                if (node->getOperator() == BinaryOperator::PlusAssign) {
                    newValue = current + right;
                } else if (node->getOperator() == BinaryOperator::MinusAssign) {
                    newValue = current - right;
                } else if (node->getOperator() == BinaryOperator::MultiplyAssign) {
                    newValue = current * right;
                } else if (node->getOperator() == BinaryOperator::DivideAssign) {
                    newValue = current / right;
                }
                
                if (m_context) {
                    m_context->setVariable(identNode->getName(), newValue);
                    m_result = newValue;
                } else {
                    throw std::runtime_error("No context for variable assignment");
                }
            }
        } else {
            throw std::runtime_error("Invalid assignment target");
        }
        return;
    }

    // Evaluate left operand for other operations
    Value left = evaluate(node->getLeft(), m_context);

    // Handle short-circuit evaluation for logical operators
    if (node->getOperator() == BinaryOperator::And) {
        if (!left.toBool()) {
            m_result = Value(false);
            return;
        }
        Value right = evaluate(node->getRight(), m_context);
        m_result = Value(right.toBool());
        return;
    }

    if (node->getOperator() == BinaryOperator::Or) {
        if (left.toBool()) {
            m_result = Value(true);
            return;
        }
        Value right = evaluate(node->getRight(), m_context);
        m_result = Value(right.toBool());
        return;
    }

    // Evaluate right operand for other operators
    Value right = evaluate(node->getRight(), m_context);

    switch (node->getOperator()) {
    case BinaryOperator::Add:
        m_result = left + right;
        break;
    case BinaryOperator::Subtract:
        m_result = left - right;
        break;
    case BinaryOperator::Multiply:
        m_result = left * right;
        break;
    case BinaryOperator::Divide:
        if (right.toNumber() == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        m_result = left / right;
        break;
    case BinaryOperator::Power:
        m_result = Value(std::pow(left.toNumber(), right.toNumber()));
        break;
    case BinaryOperator::Mod: {
        double leftVal = left.toNumber();
        double rightVal = right.toNumber();
        if (rightVal == 0.0) {
            throw std::runtime_error("Modulo by zero");
        }
        // Use fmod for floating-point modulo
        m_result = Value(std::fmod(leftVal, rightVal));
    } break;
    case BinaryOperator::Div: {
        double leftVal = left.toNumber();
        double rightVal = right.toNumber();
        if (rightVal == 0.0) {
            throw std::runtime_error("Integer division by zero");
        }
        // Integer division - truncate towards zero
        m_result = Value(std::trunc(leftVal / rightVal));
    } break;
    case BinaryOperator::Equal:
        m_result = Value(left.toNumber() == right.toNumber());
        break;
    case BinaryOperator::NotEqual:
        m_result = Value(left.toNumber() != right.toNumber());
        break;
    case BinaryOperator::Less:
        m_result = Value(left.toNumber() < right.toNumber());
        break;
    case BinaryOperator::Greater:
        m_result = Value(left.toNumber() > right.toNumber());
        break;
    case BinaryOperator::LessEqual:
        m_result = Value(left.toNumber() <= right.toNumber());
        break;
    case BinaryOperator::GreaterEqual:
        m_result = Value(left.toNumber() >= right.toNumber());
        break;
    default:
        throw std::runtime_error("Unsupported binary operator");
    }
}

void Evaluator::visit(UnaryOpNode *node)
{
    switch (node->getOperator())
    {
    case UnaryOperator::Negate: {
        Value operand = evaluate(node->getOperand(), m_context);
        m_result = Value(-operand.toNumber());
        break;
    }
    case UnaryOperator::Not: {
        Value operand = evaluate(node->getOperand(), m_context);
        m_result = Value(!operand.toBool());
        break;
    }
    case UnaryOperator::PreIncrement: {
        // Pre-increment: ++x
        if (auto *identNode = dynamic_cast<IdentifierNode *>(node->getOperand())) {
            if (m_context && m_context->hasVariable(identNode->getName())) {
                Value current = m_context->getVariable(identNode->getName());
                Value newValue = Value(current.toNumber() + 1.0);
                m_context->setVariable(identNode->getName(), newValue);
                m_result = newValue;
            } else {
                throw std::runtime_error("Variable not found for pre-increment: " + identNode->getName());
            }
        } else {
            throw std::runtime_error("Pre-increment can only be applied to variables");
        }
        break;
    }
    case UnaryOperator::PostIncrement: {
        // Post-increment: x++
        if (auto *identNode = dynamic_cast<IdentifierNode *>(node->getOperand())) {
            if (m_context && m_context->hasVariable(identNode->getName())) {
                Value current = m_context->getVariable(identNode->getName());
                Value newValue = Value(current.toNumber() + 1.0);
                m_context->setVariable(identNode->getName(), newValue);
                m_result = current; // Return original value
            } else {
                throw std::runtime_error("Variable not found for post-increment: " + identNode->getName());
            }
        } else {
            throw std::runtime_error("Post-increment can only be applied to variables");
        }
        break;
    }
    case UnaryOperator::PreDecrement: {
        // Pre-decrement: --x
        if (auto *identNode = dynamic_cast<IdentifierNode *>(node->getOperand())) {
            if (m_context && m_context->hasVariable(identNode->getName())) {
                Value current = m_context->getVariable(identNode->getName());
                Value newValue = Value(current.toNumber() - 1.0);
                m_context->setVariable(identNode->getName(), newValue);
                m_result = newValue;
            } else {
                throw std::runtime_error("Variable not found for pre-decrement: " + identNode->getName());
            }
        } else {
            throw std::runtime_error("Pre-decrement can only be applied to variables");
        }
        break;
    }
    case UnaryOperator::PostDecrement: {
        // Post-decrement: x--
        if (auto *identNode = dynamic_cast<IdentifierNode *>(node->getOperand())) {
            if (m_context && m_context->hasVariable(identNode->getName())) {
                Value current = m_context->getVariable(identNode->getName());
                Value newValue = Value(current.toNumber() - 1.0);
                m_context->setVariable(identNode->getName(), newValue);
                m_result = current; // Return original value
            } else {
                throw std::runtime_error("Variable not found for post-decrement: " + identNode->getName());
            }
        } else {
            throw std::runtime_error("Post-decrement can only be applied to variables");
        }
        break;
    }
    default:
        throw std::runtime_error("Unsupported unary operator");
    }
}

void Evaluator::visit(FunctionCallNode *node)
{
    // Get function name
    IdentifierNode *funcNode = dynamic_cast<IdentifierNode *>(node->getFunction());
    if (!funcNode)
    {
        throw std::runtime_error("Invalid function call");
    }

    std::string funcName = funcNode->getName();

    // Evaluate arguments
    std::vector<Value> args;
    for (const auto &arg : node->getArguments())
    {
        args.push_back(evaluate(arg.get(), m_context));
    }

    // Check for built-in functions
    m_result = evaluateBuiltinFunction(funcName, args);
}

Value Evaluator::evaluateBuiltinFunction(const std::string &name, const std::vector<Value> &args)
{
    // Mathematical functions
    if (name == "sin" && args.size() == 1)
    {
        return Value(std::sin(args[0].toNumber()));
    }
    else if (name == "cos" && args.size() == 1)
    {
        return Value(std::cos(args[0].toNumber()));
    }
    else if (name == "tan" && args.size() == 1)
    {
        return Value(std::tan(args[0].toNumber()));
    }
    else if (name == "sqrt" && args.size() == 1)
    {
        double val = args[0].toNumber();
        if (val < 0)
        {
            throw std::runtime_error("Square root of negative number");
        }
        return Value(std::sqrt(val));
    }
    else if (name == "log" && args.size() == 1)
    {
        double val = args[0].toNumber();
        if (val <= 0)
        {
            throw std::runtime_error("Logarithm of non-positive number");
        }
        return Value(std::log(val));
    }
    else if (name == "log10" && args.size() == 1)
    {
        double val = args[0].toNumber();
        if (val <= 0)
        {
            throw std::runtime_error("Log10 of non-positive number");
        }
        return Value(std::log10(val));
    }
    else if (name == "exp" && args.size() == 1)
    {
        return Value(std::exp(args[0].toNumber()));
    }
    else if (name == "abs" && args.size() == 1)
    {
        return Value(std::abs(args[0].toNumber()));
    }
    else if (name == "pow" && args.size() == 2)
    {
        return Value(std::pow(args[0].toNumber(), args[1].toNumber()));
    }
    else if (name == "min" && args.size() == 2)
    {
        return Value(std::min(args[0].toNumber(), args[1].toNumber()));
    }
    else if (name == "max" && args.size() == 2)
    {
        return Value(std::max(args[0].toNumber(), args[1].toNumber()));
    } // In evaluator.cpp, add to evaluateBuiltinFunction():

    else if (name == "root" && args.size() == 2)
    {
        double n = args[0].toNumber(); // The root degree (2 for square root, 3 for cube root, etc.)
        double x = args[1].toNumber(); // The number to take the root of

        if (n == 0)
        {
            throw std::runtime_error("Root degree cannot be zero");
        }
        if (x < 0 && std::fmod(n, 2) == 0)
        {
            throw std::runtime_error("Even root of negative number");
        }

        // nth root of x = x^(1/n)
        return Value(std::pow(x, 1.0 / n));
    }
    else if (name == "cbrt" && args.size() == 1)
    {
        // Cube root (alternative to root(3, x))
        return Value(std::cbrt(args[0].toNumber()));
    }

    throw std::runtime_error("Unknown function: " + name + " with " + std::to_string(args.size()) + " arguments");
}

// Placeholder implementations for other node types
void Evaluator::visit(IfNode *node)
{
    Value condition = evaluate(node->getCondition(), m_context);

    if (condition.toBool()) {
        m_result = evaluate(node->getThenBranch(), m_context);
    } else if (node->getElseBranch()) {
        m_result = evaluate(node->getElseBranch(), m_context);
    } else {
        m_result = Value(); // null/void result for if without else
    }
}

void Evaluator::visit(WhileNode *node)
{
    m_result = Value(); // Default to null

    while (true)
    {
        Value condition = evaluate(node->getCondition(), m_context);
        if (!condition.toBool())
        {
            break;
        }

        m_result = evaluate(node->getBody(), m_context);

        if (m_hasReturned)
        {
            break;
        }
    }
    
    // While loops should not return the condition value
    // Keep the last body result or null if no iterations
}

void Evaluator::visit(ForNode *node)
{
    // Simplified for now
    throw std::runtime_error("For loops not yet implemented");
}

void Evaluator::visit(BlockNode *node)
{
    Value lastResult;

    for (const auto &statement : node->getStatements()) {
        lastResult = evaluate(statement.get(), m_context);

        // Check for early return
        if (m_hasReturned) {
            m_result = lastResult;
            return;
        }
    }

    m_result = lastResult;
}

void Evaluator::visit(FunctionDefNode *node)
{
    // Function definitions would be handled here
    throw std::runtime_error("Function definitions not yet implemented");
}

void Evaluator::visit(ReturnNode *node)
{
    if (node->getValue())
    {
        m_result = evaluate(node->getValue(), m_context);
    }
    else
    {
        m_result = Value();
    }
    m_hasReturned = true;
}

#include "evaluator.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <sstream>       // ← ADD THIS INSTEAD

// ...existing code...

void Evaluator::visit(PrintNode *node)
{
    std::string output;
    
    for (const auto& expr : node->getExpressions()) {
        Value result = evaluate(expr.get(), m_context);
        output += result.toString();
    }
    
    // Output to console (you might want to customize this)
    std::cout << output << std::endl;
    
    // Print statements don't return a value, but we need to set something
    m_result = Value();
}
