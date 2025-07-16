#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include <sstream>
#include "ast.h"

// Test framework for AST testing
class ASTTestRunner {
public:
    static int tests_run;
    static int tests_passed;
    
    static void assert_equal(const std::string& expected, const std::string& actual, const std::string& test_name) {
        tests_run++;
        if (expected == actual) {
            tests_passed++;
            std::cout << "[PASS] " << test_name << std::endl;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Actual:   " << actual << std::endl;
        }
    }
    
    static void assert_true(bool condition, const std::string& test_name) {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "[PASS] " << test_name << std::endl;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
        }
    }
    
    static void assert_double_equal(double expected, double actual, const std::string& test_name, double tolerance = 1e-9) {
        tests_run++;
        if (std::abs(expected - actual) < tolerance) {
            tests_passed++;
            std::cout << "[PASS] " << test_name << std::endl;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Actual:   " << actual << std::endl;
        }
    }
    
    static void print_summary() {
        std::cout << "\n=== AST Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Success rate: " << (tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0) << "%" << std::endl;
    }
};

int ASTTestRunner::tests_run = 0;
int ASTTestRunner::tests_passed = 0;

// Mock visitor for testing visitor pattern
class MockASTVisitor : public ASTVisitor {
public:
    std::string last_visited;
    
    void visit(LiteralNode *node) override { last_visited = "LiteralNode"; }
    void visit(IdentifierNode *node) override { last_visited = "IdentifierNode"; }
    void visit(BinaryOpNode *node) override { last_visited = "BinaryOpNode"; }
    void visit(UnaryOpNode *node) override { last_visited = "UnaryOpNode"; }
    void visit(FunctionCallNode *node) override { last_visited = "FunctionCallNode"; }
    void visit(IfNode *node) override { last_visited = "IfNode"; }
    void visit(WhileNode *node) override { last_visited = "WhileNode"; }
    void visit(ForNode *node) override { last_visited = "ForNode"; }
    void visit(BlockNode *node) override { last_visited = "BlockNode"; }
    void visit(FunctionDefNode *node) override { last_visited = "FunctionDefNode"; }
    void visit(ReturnNode *node) override { last_visited = "ReturnNode"; }
};

void test_value_constructors() {
    std::cout << "\n=== Testing Value Constructors ===" << std::endl;
    
    // Test default constructor (Null)
    Value nullValue;
    ASTTestRunner::assert_true(nullValue.getType() == Value::Null, "Default constructor creates Null type");
    
    // Test number constructor
    Value numValue(42.5);
    ASTTestRunner::assert_true(numValue.getType() == Value::Number, "Number constructor creates Number type");
    ASTTestRunner::assert_double_equal(42.5, numValue.toNumber(), "Number constructor stores correct value");
    
    // Test boolean constructor
    Value boolValue(true);
    ASTTestRunner::assert_true(boolValue.getType() == Value::Boolean, "Boolean constructor creates Boolean type");
    ASTTestRunner::assert_true(boolValue.toBool(), "Boolean constructor stores correct value");
    
    // Test string constructor
    Value strValue("hello");
    ASTTestRunner::assert_true(strValue.getType() == Value::String, "String constructor creates String type");
    ASTTestRunner::assert_equal("hello", strValue.toString(), "String constructor stores correct value");
}

void test_value_arithmetic_operations() {
    std::cout << "\n=== Testing Value Arithmetic Operations ===" << std::endl;
    
    Value a(10.0);
    Value b(3.0);
    
    // Test addition
    Value sum = a + b;
    ASTTestRunner::assert_double_equal(13.0, sum.toNumber(), "Addition: 10 + 3 = 13");
    
    // Test subtraction
    Value diff = a - b;
    ASTTestRunner::assert_double_equal(7.0, diff.toNumber(), "Subtraction: 10 - 3 = 7");
    
    // Test multiplication
    Value product = a * b;
    ASTTestRunner::assert_double_equal(30.0, product.toNumber(), "Multiplication: 10 * 3 = 30");
    
    // Test division
    Value quotient = a / b;
    ASTTestRunner::assert_double_equal(3.333333333333333, quotient.toNumber(), "Division: 10 / 3", 1e-10);
    
    // Test string concatenation
    Value str1("Hello ");
    Value str2("World");
    Value concat = str1 + str2;
    ASTTestRunner::assert_equal("Hello World", concat.toString(), "String concatenation");
}

void test_value_comparison_operations() {
    std::cout << "\n=== Testing Value Comparison Operations ===" << std::endl;
    
    Value a(5.0);
    Value b(10.0);
    Value c(5.0);
    
    // Test equality
    ASTTestRunner::assert_true(a == c, "Equality: 5 == 5");
    ASTTestRunner::assert_true(!(a == b), "Inequality: 5 != 10");
    
    // Test less than
    ASTTestRunner::assert_true(a < b, "Less than: 5 < 10");
    ASTTestRunner::assert_true(!(b < a), "Not less than: 10 not < 5");
    
    // Test greater than
    ASTTestRunner::assert_true(b > a, "Greater than: 10 > 5");
    ASTTestRunner::assert_true(!(a > b), "Not greater than: 5 not > 10");
    
    // Test less than or equal
    ASTTestRunner::assert_true(a <= c, "Less than or equal: 5 <= 5");
    ASTTestRunner::assert_true(a <= b, "Less than or equal: 5 <= 10");
    
    // Test greater than or equal
    ASTTestRunner::assert_true(a >= c, "Greater than or equal: 5 >= 5");
    ASTTestRunner::assert_true(b >= a, "Greater than or equal: 10 >= 5");
}

void test_literal_node() {
    std::cout << "\n=== Testing LiteralNode ===" << std::endl;
    
    // Test number literal
    Value numVal(42.0);
    auto numLiteral = std::make_unique<LiteralNode>(numVal);
    ASTTestRunner::assert_double_equal(42.0, numLiteral->getValue().toNumber(), "LiteralNode stores number value");
    ASTTestRunner::assert_equal("42", numLiteral->toString(), "LiteralNode toString for number");
    
    // Test string literal
    Value strVal("test");
    auto strLiteral = std::make_unique<LiteralNode>(strVal);
    ASTTestRunner::assert_equal("test", strLiteral->getValue().toString(), "LiteralNode stores string value");
    
    // Test visitor pattern
    MockASTVisitor visitor;
    numLiteral->accept(&visitor);
    ASTTestRunner::assert_equal("LiteralNode", visitor.last_visited, "LiteralNode accept calls correct visitor method");
}

void test_identifier_node() {
    std::cout << "\n=== Testing IdentifierNode ===" << std::endl;
    
    auto identifier = std::make_unique<IdentifierNode>("variable_name");
    ASTTestRunner::assert_equal("variable_name", identifier->getName(), "IdentifierNode stores name");
    ASTTestRunner::assert_equal("variable_name", identifier->toString(), "IdentifierNode toString");
    
    // Test visitor pattern
    MockASTVisitor visitor;
    identifier->accept(&visitor);
    ASTTestRunner::assert_equal("IdentifierNode", visitor.last_visited, "IdentifierNode accept calls correct visitor method");
}

void test_binary_op_node() {
    std::cout << "\n=== Testing BinaryOpNode ===" << std::endl;
    
    // Create operands
    auto left = std::make_unique<LiteralNode>(Value(5.0));
    auto right = std::make_unique<LiteralNode>(Value(3.0));
    
    // Test addition node
    auto addNode = std::make_unique<BinaryOpNode>(std::move(left), BinaryOperator::Add, std::move(right));
    ASTTestRunner::assert_true(addNode->getOperator() == BinaryOperator::Add, "BinaryOpNode stores operator");
    ASTTestRunner::assert_true(addNode->getLeft() != nullptr, "BinaryOpNode stores left operand");
    ASTTestRunner::assert_true(addNode->getRight() != nullptr, "BinaryOpNode stores right operand");
    
    // Test visitor pattern
    MockASTVisitor visitor;
    addNode->accept(&visitor);
    ASTTestRunner::assert_equal("BinaryOpNode", visitor.last_visited, "BinaryOpNode accept calls correct visitor method");
    
    // Test mathematical operations
    auto left2 = std::make_unique<LiteralNode>(Value(10.0));
    auto right2 = std::make_unique<LiteralNode>(Value(2.0));
    auto divNode = std::make_unique<BinaryOpNode>(std::move(left2), BinaryOperator::Divide, std::move(right2));
    ASTTestRunner::assert_true(divNode->getOperator() == BinaryOperator::Divide, "BinaryOpNode division operator");
}

void test_unary_op_node() {
    std::cout << "\n=== Testing UnaryOpNode ===" << std::endl;
    
    auto operand = std::make_unique<LiteralNode>(Value(5.0));
    auto negateNode = std::make_unique<UnaryOpNode>(UnaryOperator::Negate, std::move(operand));
    
    ASTTestRunner::assert_true(negateNode->getOperator() == UnaryOperator::Negate, "UnaryOpNode stores operator");
    ASTTestRunner::assert_true(negateNode->getOperand() != nullptr, "UnaryOpNode stores operand");
    
    // Test visitor pattern
    MockASTVisitor visitor;
    negateNode->accept(&visitor);
    ASTTestRunner::assert_equal("UnaryOpNode", visitor.last_visited, "UnaryOpNode accept calls correct visitor method");
}

void test_function_call_node() {
    std::cout << "\n=== Testing FunctionCallNode ===" << std::endl;
    
    // Create function name and arguments for mathematical function
    auto funcName = std::make_unique<IdentifierNode>("sqrt");
    std::vector<std::unique_ptr<ASTNode>> args;
    args.push_back(std::make_unique<LiteralNode>(Value(16.0)));
    
    auto funcCall = std::make_unique<FunctionCallNode>(std::move(funcName), std::move(args));
    ASTTestRunner::assert_true(funcCall->getFunction() != nullptr, "FunctionCallNode stores function");
    ASTTestRunner::assert_true(funcCall->getArguments().size() == 1, "FunctionCallNode stores arguments");
    
    // Test mathematical function with multiple arguments
    auto funcName2 = std::make_unique<IdentifierNode>("pow");
    std::vector<std::unique_ptr<ASTNode>> args2;
    args2.push_back(std::make_unique<LiteralNode>(Value(2.0)));
    args2.push_back(std::make_unique<LiteralNode>(Value(3.0)));
    
    auto powCall = std::make_unique<FunctionCallNode>(std::move(funcName2), std::move(args2));
    ASTTestRunner::assert_true(powCall->getArguments().size() == 2, "FunctionCallNode pow function with 2 args");
    
    // Test visitor pattern
    MockASTVisitor visitor;
    funcCall->accept(&visitor);
    ASTTestRunner::assert_equal("FunctionCallNode", visitor.last_visited, "FunctionCallNode accept calls correct visitor method");
}

void test_control_flow_nodes() {
    std::cout << "\n=== Testing Control Flow Nodes ===" << std::endl;
    
    // Test IfNode
    auto condition = std::make_unique<LiteralNode>(Value(true));
    auto thenBranch = std::make_unique<LiteralNode>(Value(1.0));
    auto elseBranch = std::make_unique<LiteralNode>(Value(0.0));
    
    auto ifNode = std::make_unique<IfNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    ASTTestRunner::assert_true(ifNode->getCondition() != nullptr, "IfNode stores condition");
    ASTTestRunner::assert_true(ifNode->getThenBranch() != nullptr, "IfNode stores then branch");
    ASTTestRunner::assert_true(ifNode->getElseBranch() != nullptr, "IfNode stores else branch");
    
    // Test WhileNode
    auto whileCondition = std::make_unique<LiteralNode>(Value(true));
    auto whileBody = std::make_unique<LiteralNode>(Value(1.0));
    
    auto whileNode = std::make_unique<WhileNode>(std::move(whileCondition), std::move(whileBody));
    ASTTestRunner::assert_true(whileNode->getCondition() != nullptr, "WhileNode stores condition");
    ASTTestRunner::assert_true(whileNode->getBody() != nullptr, "WhileNode stores body");
    
    // Test visitor patterns
    MockASTVisitor visitor;
    ifNode->accept(&visitor);
    ASTTestRunner::assert_equal("IfNode", visitor.last_visited, "IfNode accept calls correct visitor method");
    
    whileNode->accept(&visitor);
    ASTTestRunner::assert_equal("WhileNode", visitor.last_visited, "WhileNode accept calls correct visitor method");
}

void test_complex_mathematical_expressions() {
    std::cout << "\n=== Testing Complex Mathematical Expressions ===" << std::endl;
    
    // Test nested arithmetic: (5 + 3) * 2
    auto left_inner = std::make_unique<LiteralNode>(Value(5.0));
    auto right_inner = std::make_unique<LiteralNode>(Value(3.0));
    auto addition = std::make_unique<BinaryOpNode>(std::move(left_inner), BinaryOperator::Add, std::move(right_inner));
    
    auto multiplier = std::make_unique<LiteralNode>(Value(2.0));
    auto multiplication = std::make_unique<BinaryOpNode>(std::move(addition), BinaryOperator::Multiply, std::move(multiplier));
    
    ASTTestRunner::assert_true(multiplication->getOperator() == BinaryOperator::Multiply, "Complex expression operator");
    ASTTestRunner::assert_true(multiplication->getLeft() != nullptr, "Complex expression left operand");
    ASTTestRunner::assert_true(multiplication->getRight() != nullptr, "Complex expression right operand");
    
    // Test function call with arithmetic: sqrt(pow(3, 2) + pow(4, 2))
    auto base1 = std::make_unique<LiteralNode>(Value(3.0));
    auto exp1 = std::make_unique<LiteralNode>(Value(2.0));
    std::vector<std::unique_ptr<ASTNode>> pow1Args;
    pow1Args.push_back(std::move(base1));
    pow1Args.push_back(std::move(exp1));
    auto pow1Func = std::make_unique<IdentifierNode>("pow");
    auto pow1Call = std::make_unique<FunctionCallNode>(std::move(pow1Func), std::move(pow1Args));
    
    auto base2 = std::make_unique<LiteralNode>(Value(4.0));
    auto exp2 = std::make_unique<LiteralNode>(Value(2.0));
    std::vector<std::unique_ptr<ASTNode>> pow2Args;
    pow2Args.push_back(std::move(base2));
    pow2Args.push_back(std::move(exp2));
    auto pow2Func = std::make_unique<IdentifierNode>("pow");
    auto pow2Call = std::make_unique<FunctionCallNode>(std::move(pow2Func), std::move(pow2Args));
    
    auto sumOfSquares = std::make_unique<BinaryOpNode>(std::move(pow1Call), BinaryOperator::Add, std::move(pow2Call));
    
    std::vector<std::unique_ptr<ASTNode>> sqrtArgs;
    sqrtArgs.push_back(std::move(sumOfSquares));
    auto sqrtFunc = std::make_unique<IdentifierNode>("sqrt");
    auto sqrtCall = std::make_unique<FunctionCallNode>(std::move(sqrtFunc), std::move(sqrtArgs));
    
    ASTTestRunner::assert_true(sqrtCall->getArguments().size() == 1, "Complex sqrt expression has one argument");
    ASTTestRunner::assert_true(sqrtCall->getFunction() != nullptr, "Complex sqrt expression has function");
}

void test_block_and_function_nodes() {
    std::cout << "\n=== Testing Block and Function Nodes ===" << std::endl;
    
    // Test BlockNode
    std::vector<std::unique_ptr<ASTNode>> statements;
    statements.push_back(std::make_unique<LiteralNode>(Value(1.0)));
    statements.push_back(std::make_unique<LiteralNode>(Value(2.0)));
    
    auto block = std::make_unique<BlockNode>(std::move(statements));
    ASTTestRunner::assert_true(block->getStatements().size() == 2, "BlockNode stores statements");
    
    // Test FunctionDefNode
    std::vector<std::string> params = {"x", "y"};
    auto body = std::make_unique<LiteralNode>(Value(0.0));
    auto funcDef = std::make_unique<FunctionDefNode>("add", params, std::move(body));
    
    ASTTestRunner::assert_equal("add", funcDef->getName(), "FunctionDefNode stores name");
    ASTTestRunner::assert_true(funcDef->getParameters().size() == 2, "FunctionDefNode stores parameters");
    ASTTestRunner::assert_true(funcDef->getBody() != nullptr, "FunctionDefNode stores body");
    
    // Test ReturnNode
    auto returnValue = std::make_unique<LiteralNode>(Value(42.0));
    auto returnNode = std::make_unique<ReturnNode>(std::move(returnValue));
    ASTTestRunner::assert_true(returnNode->getValue() != nullptr, "ReturnNode stores value");
    
    // Test visitor patterns
    MockASTVisitor visitor;
    block->accept(&visitor);
    ASTTestRunner::assert_equal("BlockNode", visitor.last_visited, "BlockNode accept calls correct visitor method");
    
    funcDef->accept(&visitor);
    ASTTestRunner::assert_equal("FunctionDefNode", visitor.last_visited, "FunctionDefNode accept calls correct visitor method");
    
    returnNode->accept(&visitor);
    ASTTestRunner::assert_equal("ReturnNode", visitor.last_visited, "ReturnNode accept calls correct visitor method");
}

// Main test runner function
inline void run_ast_tests() {
    std::cout << "Running GlossAI AST Tests..." << std::endl;
    
    try {
        test_value_constructors();
        test_value_arithmetic_operations();
        test_value_comparison_operations();
        test_literal_node();
        test_identifier_node();
        test_binary_op_node();
        test_unary_op_node();
        test_function_call_node();
        test_control_flow_nodes();
        test_complex_mathematical_expressions();
        test_block_and_function_nodes();
    } catch (const std::exception& e) {
        std::cout << "Exception caught during AST tests: " << e.what() << std::endl;
    }
    
    ASTTestRunner::print_summary();
}