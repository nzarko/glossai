#include "ast.h"
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// Value implementation
double Value::toNumber() const {
    switch (m_type) {
    case Number: return m_number;
    case Boolean: return m_boolean ? 1.0 : 0.0;
    case String: {
        try {
            return std::stod(m_string);
        } catch (...) {
            return 0.0;
        }
    }
    case Null: return 0.0;
    }
    return 0.0;
}

bool Value::toBool() const {
    switch (m_type) {
    case Boolean: return m_boolean;
    case Number: return m_number != 0.0;
    case String: return !m_string.empty();
    case Null: return false;
    }
    return false;
}

std::string Value::toString() const {
    switch (m_type) {
    case String: return m_string;
    case Number: {
        std::ostringstream oss;
        oss << m_number;
        return oss.str();
    }
    case Boolean: return m_boolean ? "true" : "false";
    case Null: return "null";
    }
    return "";
}

bool Value::operator==(const Value& other) const {
    if (m_type != other.m_type) {
        return false;
    }
    switch (m_type) {
    case Number: return m_number == other.m_number;
    case Boolean: return m_boolean == other.m_boolean;
    case String: return m_string == other.m_string;
    case Null: return true;
    }
    return false;
}

bool Value::operator<(const Value& other) const {
    if (m_type == Number && other.m_type == Number) {
        return m_number < other.m_number;
    }
    if (m_type == String && other.m_type == String) {
        return m_string < other.m_string;
    }
    return toNumber() < other.toNumber();
}

Value Value::operator+(const Value& other) const {
    if (m_type == String || other.m_type == String) {
        return Value(toString() + other.toString());
    }
    return Value(toNumber() + other.toNumber());
}

Value Value::operator-(const Value& other) const {
    return Value(toNumber() - other.toNumber());
}

Value Value::operator*(const Value& other) const {
    return Value(toNumber() * other.toNumber());
}

Value Value::operator/(const Value& other) const {
    double divisor = other.toNumber();
    if (divisor == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return Value(toNumber() / divisor);
}

// AST Node implementations
std::string BinaryOpNode::toString() const
{
    static const std::unordered_map<BinaryOperator, std::string> opStrings = {
        {BinaryOperator::Add, "+"},
        {BinaryOperator::Subtract, "-"},
        {BinaryOperator::Multiply, "*"},
        {BinaryOperator::Divide, "/"},
        {BinaryOperator::Power, "**"},
        {BinaryOperator::Equal, "=="},
        {BinaryOperator::NotEqual, "!="},
        {BinaryOperator::Less, "<"},
        {BinaryOperator::Greater, ">"},
        {BinaryOperator::LessEqual, "<="},
        {BinaryOperator::GreaterEqual, ">="},
        {BinaryOperator::And, "and"},
        {BinaryOperator::Or, "or"},
        {BinaryOperator::Assign, "="}
    };
    
    auto it = opStrings.find(m_operator);
    std::string opStr = (it != opStrings.end()) ? it->second : "?";
    
    return "(" + m_left->toString() + " " + opStr + " " + m_right->toString() + ")";
}

std::string UnaryOpNode::toString() const
{
    static const std::unordered_map<UnaryOperator, std::string> opStrings = {
        {UnaryOperator::Negate, "-"},
        {UnaryOperator::Not, "not"}
    };
    
    auto it = opStrings.find(m_operator);
    std::string opStr = (it != opStrings.end()) ? it->second : "?";
    
    return "(" + opStr + m_operand->toString() + ")";
}

std::string FunctionCallNode::toString() const
{
    std::string result = m_function->toString() + "(";
    for (size_t i = 0; i < m_arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += m_arguments[i]->toString();
    }
    result += ")";
    return result;
}

std::string IfNode::toString() const
{
    std::string result = "if (" + m_condition->toString() + ") " + m_thenBranch->toString();
    
    if (m_elseBranch) {
        result += " else " + m_elseBranch->toString();
    }
    
    return result;
}

std::string WhileNode::toString() const
{
    return "while (" + m_condition->toString() + ") " + m_body->toString();
}

std::string ForNode::toString() const
{
    return "for (" + m_init->toString() + "; " + m_condition->toString() + 
           "; " + m_update->toString() + ") " + m_body->toString();
}

std::string BlockNode::toString() const
{
    std::string result = "{ ";
    for (size_t i = 0; i < m_statements.size(); ++i) {
        if (i > 0) result += "; ";
        result += m_statements[i]->toString();
    }
    result += " }";
    return result;
}

std::string FunctionDefNode::toString() const
{
    std::string result = "function " + m_name + "(";
    for (size_t i = 0; i < m_parameters.size(); ++i) {
        if (i > 0) result += ", ";
        result += m_parameters[i];
    }
    result += ") " + m_body->toString();
    return result;
}

std::string ReturnNode::toString() const
{
    if (m_value) {
        return "return " + m_value->toString();
    }
    return "return";
}
