#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include "interpreter.h"
#include "context.h"

// Simple test framework
class TestRunner {
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
    
    static void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Success rate: " << (tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0) << "%" << std::endl;
    }
};

int TestRunner::tests_run = 0;
int TestRunner::tests_passed = 0;

void test_basic_arithmetic() {
    std::cout << "\n=== Testing Basic Arithmetic ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test addition
    auto result = interpreter.execute("2 + 3");
    TestRunner::assert_equal("5", result, "Simple addition");
    
    // Test subtraction
    result = interpreter.execute("10 - 4");
    TestRunner::assert_equal("6", result, "Simple subtraction");
    
    // Test multiplication
    result = interpreter.execute("3 * 7");
    TestRunner::assert_equal("21", result, "Simple multiplication");
    
    // Test division
    result = interpreter.execute("15 / 3");
    TestRunner::assert_equal("5", result, "Simple division");
    
    // Test parentheses
    result = interpreter.execute("(2 + 3) * 4");
    TestRunner::assert_equal("20", result, "Parentheses precedence");
    
    // Test operator precedence
    result = interpreter.execute("2 + 3 * 4");
    TestRunner::assert_equal("14", result, "Operator precedence");
}

void test_mathematical_functions() {
    std::cout << "\n=== Testing Mathematical Functions ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test sqrt function
    auto result = interpreter.execute("sqrt(16)");
    TestRunner::assert_equal("4", result, "Square root function");
    
    // Test pow function
    result = interpreter.execute("pow(2, 3)");
    TestRunner::assert_equal("8", result, "Power function");
    
    // Test sin function (approximately)
    result = interpreter.execute("sin(0)");
    TestRunner::assert_equal("0", result, "Sin of 0");
    
    // Test cos function (approximately)
    result = interpreter.execute("cos(0)");
    TestRunner::assert_equal("1", result, "Cos of 0");
}

void test_variables() {
    std::cout << "\n=== Testing Variables ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test variable assignment
    auto result = interpreter.execute("x = 5");
    TestRunner::assert_equal("5", result, "Variable assignment");
    
    // Test variable usage
    result = interpreter.execute("x + 3");
    TestRunner::assert_equal("8", result, "Variable usage");
    
    // Test variable update
    result = interpreter.execute("x = x * 2");
    TestRunner::assert_equal("10", result, "Variable update");
    
    // Test multiple variables
    interpreter.execute("y = 7");
    result = interpreter.execute("x + y");
    TestRunner::assert_equal("17", result, "Multiple variables");
}

void test_error_handling() {
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test division by zero
    auto result = interpreter.execute("5 / 0");
    TestRunner::assert_true(result.find("Error") != std::string::npos, "Division by zero error");
    
    // Test undefined variable
    result = interpreter.execute("undefined_var + 5");
    TestRunner::assert_true(result.find("Error") != std::string::npos, "Undefined variable error");
    
    // Test invalid syntax
    result = interpreter.execute("2 + + 3");
    TestRunner::assert_true(result.find("Error") != std::string::npos, "Invalid syntax error");
}

void test_complex_expressions() {
    std::cout << "\n=== Testing Complex Expressions ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test nested functions
    auto result = interpreter.execute("sqrt(pow(3, 2) + pow(4, 2))");
    TestRunner::assert_equal("5", result, "Pythagorean theorem");
    
    // Test complex arithmetic
    result = interpreter.execute("((5 + 3) * 2 - 4) / 3");
    TestRunner::assert_equal("4", result, "Complex arithmetic expression");
    
    // Test variables in complex expressions
    interpreter.execute("a = 2");
    interpreter.execute("b = 3");
    result = interpreter.execute("pow(a + b, 2) - 2 * a * b");
    TestRunner::assert_equal("13", result, "Variables in complex expressions");
}

int main() {
    std::cout << "Running GlossAI Interpreter Tests..." << std::endl;
    
    try {
        test_basic_arithmetic();
        test_mathematical_functions();
        test_variables();
        test_error_handling();
        test_complex_expressions();
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    TestRunner::print_summary();
    
    // Return non-zero if any tests failed
    return (TestRunner::tests_run == TestRunner::tests_passed) ? 0 : 1;
}
