#include "evaluator.h"
#include "context.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

Evaluator::Evaluator()
    : m_context(nullptr)
    , m_shouldReturn(false)
{
    setupBuiltinFunctions();
}

Value Evaluator::evaluate(ASTNode *node, Context *context)
{
    m_context = context;
    m_result = Value();
    m_lastError.clear();
    m_shouldReturn = false;
    m_returnValue = Value();
    
    try {
        node->accept(this);
        return m_shouldReturn ? m_returnValue : m_result;
    } catch (const std::exception &e) {
        m_lastError = std::string("Runtime error: ") + e.what();
        return Value();
    }
}

void Evaluator::visit(LiteralNode *node)
{
    m_result = node->getValue();
}

void Evaluator::visit(IdentifierNode *node)
{
    std::string name = node->getName();
    if (m_context->hasVariable(name)) {
        m_result = m_context->getVariable(name);
    } else {
        m_lastError = "Undefined variable: " + name;
        throw std::runtime_error(m_lastError);
    }
}

void Evaluator::visit(BinaryOpNode *node)
{
    if (node->getOperator() == BinaryOperator::Assign) {
        // Special handling for assignment
        auto *identifierNode = dynamic_cast<IdentifierNode*>(node->getLeft());
        if (!identifierNode) {
            m_lastError = "Invalid assignment target";
            throw std::runtime_error(m_lastError);
        }
        
        node->getRight()->accept(this);
        Value value = m_result;
        
        m_context->setVariable(identifierNode->getName(), value);
        m_result = value;
        return;
    }
    
    node->getLeft()->accept(this);
    Value left = m_result;
    
    node->getRight()->accept(this);
    Value right = m_result;
    
    m_result = evaluateBinaryOp(node->getOperator(), left, right);
}

void Evaluator::visit(UnaryOpNode *node)
{
    node->getOperand()->accept(this);
    Value operand = m_result;
    
    m_result = evaluateUnaryOp(node->getOperator(), operand);
}

void Evaluator::visit(FunctionCallNode *node)
{
    auto *identifierNode = dynamic_cast<IdentifierNode*>(node->getFunction());
    if (!identifierNode) {
        m_lastError = "Invalid function call";
        throw std::runtime_error(m_lastError);
    }
    
    std::string functionName = identifierNode->getName();
    
    // Evaluate arguments
    std::vector<Value> args;
    for (const auto &argNode : node->getArguments()) {
        argNode->accept(this);
        args.push_back(m_result);
    }
    
    // Check for builtin functions first
    if (m_builtinFunctions.find(functionName) != m_builtinFunctions.end()) {
        m_result = m_builtinFunctions[functionName](args);
        return;
    }
    
    // Check for user-defined functions
    if (m_context->hasFunction(functionName)) {
        auto function = m_context->getFunction(functionName);
        
        if (args.size() != function.parameters.size()) {
            m_lastError = "Function " + functionName + " expects " + 
                         std::to_string(function.parameters.size()) + " arguments, got " + 
                         std::to_string(args.size());
            throw std::runtime_error(m_lastError);
        }
        
        // Create new scope for function execution
        m_context->pushScope();
        
        // Bind parameters
        for (size_t i = 0; i < function.parameters.size(); ++i) {
            m_context->setVariable(function.parameters[i], args[i]);
        }
        
        // Execute function body
        function.body->accept(this);
        Value result = m_shouldReturn ? m_returnValue : Value();
        
        // Clean up
        m_shouldReturn = false;
        m_returnValue = Value();
        m_context->popScope();
        
        m_result = result;
        return;
    }
    
    m_lastError = "Undefined function: " + functionName;
    throw std::runtime_error(m_lastError);
}

void Evaluator::visit(IfNode *node)
{
    node->getCondition()->accept(this);
    bool condition = isTruthy(m_result);
    
    if (condition) {
        node->getThenBranch()->accept(this);
    } else if (node->getElseBranch()) {
        node->getElseBranch()->accept(this);
    }
}

void Evaluator::visit(WhileNode *node)
{
    while (true) {
        node->getCondition()->accept(this);
        if (!isTruthy(m_result)) {
            break;
        }
        
        node->getBody()->accept(this);
        
        if (m_shouldReturn) {
            break;
        }
    }
}

void Evaluator::visit(ForNode *node)
{
    // Execute initializer
    if (node->getInit()) {
        node->getInit()->accept(this);
    }
    
    while (true) {
        // Check condition
        if (node->getCondition()) {
            node->getCondition()->accept(this);
            if (!isTruthy(m_result)) {
                break;
            }
        }
        
        // Execute body
        node->getBody()->accept(this);
        
        if (m_shouldReturn) {
            break;
        }
        
        // Execute update
        if (node->getUpdate()) {
            node->getUpdate()->accept(this);
        }
    }
}

void Evaluator::visit(BlockNode *node)
{
    m_context->pushScope();
    
    for (const auto &stmt : node->getStatements()) {
        stmt->accept(this);
        if (m_shouldReturn) {
            break;
        }
    }
    
    m_context->popScope();
}

void Evaluator::visit(FunctionDefNode *node)
{
    UserFunction function;
    function.name = node->getName();
    function.parameters = node->getParameters();
    function.body = node->getBody();
    
    m_context->setFunction(node->getName(), function);
    m_result = Value(); // Function definitions don't return values
}

void Evaluator::visit(ReturnNode *node)
{
    if (node->getValue()) {
        node->getValue()->accept(this);
        m_returnValue = m_result;
    } else {
        m_returnValue = Value();
    }
    m_shouldReturn = true;
}

Value Evaluator::evaluateBinaryOp(BinaryOperator op, const Value &left, const Value &right)
{
    switch (op) {
    case BinaryOperator::Add:
        return left + right;
        
    case BinaryOperator::Subtract:
        return left - right;
        
    case BinaryOperator::Multiply:
        return left * right;
        
    case BinaryOperator::Divide:
        return left / right;
        
    case BinaryOperator::Power:
        return Value(std::pow(left.toNumber(), right.toNumber()));
        
    case BinaryOperator::Equal:
        return Value(left == right);
        
    case BinaryOperator::NotEqual:
        return Value(left != right);
        
    case BinaryOperator::Less:
        return Value(left < right);
        
    case BinaryOperator::Greater:
        return Value(left > right);
        
    case BinaryOperator::LessEqual:
        return Value(left <= right);
        
    case BinaryOperator::GreaterEqual:
        return Value(left >= right);
        
    case BinaryOperator::And:
        return Value(isTruthy(left) && isTruthy(right));
        
    case BinaryOperator::Or:
        return Value(isTruthy(left) || isTruthy(right));
        
    default:
        m_lastError = "Unknown binary operator";
        throw std::runtime_error(m_lastError);
    }
}

Value Evaluator::evaluateUnaryOp(UnaryOperator op, const Value &operand)
{
    switch (op) {
    case UnaryOperator::Negate:
        return Value(-operand.toNumber());
        
    case UnaryOperator::Not:
        return Value(!isTruthy(operand));
        
    default:
        m_lastError = "Unknown unary operator";
        throw std::runtime_error(m_lastError);
    }
}

bool Evaluator::isTruthy(const Value &value)
{
    return value.toBool();
}

void Evaluator::setupBuiltinFunctions()
{
    m_builtinFunctions["print"] = [this](const std::vector<Value> &args) { return builtinPrint(args); };
    m_builtinFunctions["sqrt"] = [this](const std::vector<Value> &args) { return builtinSqrt(args); };
    m_builtinFunctions["pow"] = [this](const std::vector<Value> &args) { return builtinPow(args); };
    m_builtinFunctions["abs"] = [this](const std::vector<Value> &args) { return builtinAbs(args); };
    m_builtinFunctions["sin"] = [this](const std::vector<Value> &args) { return builtinSin(args); };
    m_builtinFunctions["cos"] = [this](const std::vector<Value> &args) { return builtinCos(args); };
    m_builtinFunctions["tan"] = [this](const std::vector<Value> &args) { return builtinTan(args); };
    m_builtinFunctions["log"] = [this](const std::vector<Value> &args) { return builtinLog(args); };
    m_builtinFunctions["exp"] = [this](const std::vector<Value> &args) { return builtinExp(args); };
    m_builtinFunctions["floor"] = [this](const std::vector<Value> &args) { return builtinFloor(args); };
    m_builtinFunctions["ceil"] = [this](const std::vector<Value> &args) { return builtinCeil(args); };
    m_builtinFunctions["round"] = [this](const std::vector<Value> &args) { return builtinRound(args); };
    m_builtinFunctions["min"] = [this](const std::vector<Value> &args) { return builtinMin(args); };
    m_builtinFunctions["max"] = [this](const std::vector<Value> &args) { return builtinMax(args); };
}

Value Evaluator::builtinPrint(const std::vector<Value> &args)
{
    std::string output;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) output += " ";
        output += args[i].toString();
    }
    std::cout << output << std::endl;
    return Value(output);
}

Value Evaluator::builtinSqrt(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "sqrt() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::sqrt(args[0].toNumber()));
}

Value Evaluator::builtinPow(const std::vector<Value> &args)
{
    if (args.size() != 2) {
        m_lastError = "pow() takes exactly 2 arguments";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::pow(args[0].toNumber(), args[1].toNumber()));
}

Value Evaluator::builtinAbs(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "abs() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::abs(args[0].toNumber()));
}

Value Evaluator::builtinSin(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "sin() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::sin(args[0].toNumber()));
}

Value Evaluator::builtinCos(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "cos() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::cos(args[0].toNumber()));
}

Value Evaluator::builtinTan(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "tan() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::tan(args[0].toNumber()));
}

Value Evaluator::builtinLog(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "log() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::log(args[0].toNumber()));
}

Value Evaluator::builtinExp(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "exp() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::exp(args[0].toNumber()));
}

Value Evaluator::builtinFloor(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "floor() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::floor(args[0].toNumber()));
}

Value Evaluator::builtinCeil(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "ceil() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::ceil(args[0].toNumber()));
}

Value Evaluator::builtinRound(const std::vector<Value> &args)
{
    if (args.size() != 1) {
        m_lastError = "round() takes exactly 1 argument";
        throw std::runtime_error(m_lastError);
    }
    return Value(std::round(args[0].toNumber()));
}

Value Evaluator::builtinMin(const std::vector<Value> &args)
{
    if (args.empty()) {
        m_lastError = "min() requires at least 1 argument";
        throw std::runtime_error(m_lastError);
    }
    
    double minVal = args[0].toNumber();
    for (size_t i = 1; i < args.size(); ++i) {
        minVal = std::min(minVal, args[i].toNumber());
    }
    return Value(minVal);
}

Value Evaluator::builtinMax(const std::vector<Value> &args)
{
    if (args.empty()) {
        m_lastError = "max() requires at least 1 argument";
        throw std::runtime_error(m_lastError);
    }
    
    double maxVal = args[0].toNumber();
    for (size_t i = 1; i < args.size(); ++i) {
        maxVal = std::max(maxVal, args[i].toNumber());
    }
    return Value(maxVal);
}
