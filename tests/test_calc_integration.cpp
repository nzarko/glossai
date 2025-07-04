#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>
#include <limits>
#include "interpreter.h"
#include "context.h"

// Define mathematical constants for Windows compatibility
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

// Test framework
class IntegrationTestRunner {
public:
    static int tests_run;
    static int tests_passed;
    
    static void assert_near(double expected, double actual, double tolerance, const std::string& test_name) {
        tests_run++;
        if (std::abs(expected - actual) <= tolerance) {
            tests_passed++;
            std::cout << "[PASS] " << test_name << std::endl;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
            std::cout << "  Expected: " << expected << " (±" << tolerance << ")" << std::endl;
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
        std::cout << "\n=== Integration Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Success rate: " << (tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0) << "%" << std::endl;
    }
};

int IntegrationTestRunner::tests_run = 0;
int IntegrationTestRunner::tests_passed = 0;

double parse_result(const std::string& result) {
    try {
        return std::stod(result);
    } catch (...) {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void test_mathematical_constants() {
    std::cout << "\n=== Testing Mathematical Constants ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test pi calculation using various methods
    auto result = interpreter.execute("4 * atan(1)");
    double pi_value = parse_result(result);
    IntegrationTestRunner::assert_near(M_PI, pi_value, 0.0001, "Pi calculation using atan");
    
    // Test e calculation
    result = interpreter.execute("exp(1)");
    double e_value = parse_result(result);
    IntegrationTestRunner::assert_near(M_E, e_value, 0.0001, "Euler's number using exp");
}

void test_trigonometric_functions() {
    std::cout << "\n=== Testing Trigonometric Functions ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test basic trigonometric identities
    auto result = interpreter.execute("pow(sin(0.5), 2) + pow(cos(0.5), 2)");
    double identity_result = parse_result(result);
    IntegrationTestRunner::assert_near(1.0, identity_result, 0.0001, "sin²x + cos²x = 1");
    
    // Test angle conversions (assuming radians)
    result = interpreter.execute("sin(3.14159265359 / 2)");
    double sin_90 = parse_result(result);
    IntegrationTestRunner::assert_near(1.0, sin_90, 0.0001, "sin(π/2) = 1");
    
    // Test tan function
    result = interpreter.execute("tan(3.14159265359 / 4)");
    double tan_45 = parse_result(result);
    IntegrationTestRunner::assert_near(1.0, tan_45, 0.0001, "tan(π/4) = 1");
}

void test_logarithmic_functions() {
    std::cout << "\n=== Testing Logarithmic Functions ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test natural logarithm
    auto result = interpreter.execute("log(exp(2))");
    double ln_e2 = parse_result(result);
    IntegrationTestRunner::assert_near(2.0, ln_e2, 0.0001, "ln(e²) = 2");
    
    // Test logarithm base 10
    result = interpreter.execute("log10(1000)");
    double log10_1000 = parse_result(result);
    IntegrationTestRunner::assert_near(3.0, log10_1000, 0.0001, "log₁₀(1000) = 3");
    
    // Test logarithm properties
    result = interpreter.execute("log(2) + log(3)");
    double log_sum = parse_result(result);
    result = interpreter.execute("log(6)");
    double log_product = parse_result(result);
    IntegrationTestRunner::assert_near(log_product, log_sum, 0.0001, "log(a) + log(b) = log(ab)");
}

void test_power_and_root_functions() {
    std::cout << "\n=== Testing Power and Root Functions ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test square root
    auto result = interpreter.execute("sqrt(144)");
    double sqrt_144 = parse_result(result);
    IntegrationTestRunner::assert_near(12.0, sqrt_144, 0.0001, "√144 = 12");
    
    // Test cube root using power
    result = interpreter.execute("pow(27, 1/3)");
    double cbrt_27 = parse_result(result);
    IntegrationTestRunner::assert_near(3.0, cbrt_27, 0.01, "∛27 = 3");
    
    // Test power properties
    result = interpreter.execute("pow(2, 3) * pow(2, 4)");
    double power_mult = parse_result(result);
    result = interpreter.execute("pow(2, 7)");
    double power_sum = parse_result(result);
    IntegrationTestRunner::assert_near(power_sum, power_mult, 0.0001, "a^m × a^n = a^(m+n)");
}

void test_numerical_stability() {
    std::cout << "\n=== Testing Numerical Stability ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test very small numbers
    auto result = interpreter.execute("1e-10 + 1e-10");
    double small_sum = parse_result(result);
    IntegrationTestRunner::assert_near(2e-10, small_sum, 1e-15, "Small number addition");
    
    // Test very large numbers
    result = interpreter.execute("1e10 * 1e5");
    double large_mult = parse_result(result);
    IntegrationTestRunner::assert_near(1e15, large_mult, 1e10, "Large number multiplication");
    
    // Test precision with many decimal places
    result = interpreter.execute("1/3 * 3");
    double precision_test = parse_result(result);
    IntegrationTestRunner::assert_near(1.0, precision_test, 0.0001, "Floating point precision test");
}

void test_complex_calculations() {
    std::cout << "\n=== Testing Complex Real-World Calculations ===" << std::endl;
    
    Interpreter interpreter;
    
    // Test compound interest formula: A = P(1 + r/n)^(nt)
    interpreter.execute("P = 1000");  // Principal
    interpreter.execute("r = 0.05");  // Annual interest rate (5%)
    interpreter.execute("n = 12");    // Compounding frequency (monthly)
    interpreter.execute("t = 10");    // Time in years
    
    auto result = interpreter.execute("P * pow(1 + r/n, n*t)");
    double compound_interest = parse_result(result);
    IntegrationTestRunner::assert_near(1643.62, compound_interest, 1.0, "Compound interest calculation");
    
    // Test quadratic formula: (-b ± √(b²-4ac)) / 2a
    // For equation: x² - 5x + 6 = 0 (solutions: x = 2, x = 3)
    interpreter.execute("a = 1");
    interpreter.execute("b = -5");
    interpreter.execute("c = 6");
    
    result = interpreter.execute("(-b + sqrt(b*b - 4*a*c)) / (2*a)");
    double root1 = parse_result(result);
    result = interpreter.execute("(-b - sqrt(b*b - 4*a*c)) / (2*a)");
    double root2 = parse_result(result);
    
    IntegrationTestRunner::assert_true((std::abs(root1 - 3.0) < 0.0001 && std::abs(root2 - 2.0) < 0.0001) ||
                                      (std::abs(root1 - 2.0) < 0.0001 && std::abs(root2 - 3.0) < 0.0001),
                                      "Quadratic formula solutions");
    
    // Test distance formula: d = √((x₂-x₁)² + (y₂-y₁)²)
    interpreter.execute("x1 = 0");
    interpreter.execute("y1 = 0");
    interpreter.execute("x2 = 3");
    interpreter.execute("y2 = 4");
    
    result = interpreter.execute("sqrt(pow(x2-x1, 2) + pow(y2-y1, 2))");
    double distance = parse_result(result);
    IntegrationTestRunner::assert_near(5.0, distance, 0.0001, "Distance formula (3-4-5 triangle)");
}

int main() {
    std::cout << "Running GlossAI Calculation Integration Tests..." << std::endl;
    
    try {
        test_mathematical_constants();
        test_trigonometric_functions();
        test_logarithmic_functions();
        test_power_and_root_functions();
        test_numerical_stability();
        test_complex_calculations();
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    IntegrationTestRunner::print_summary();
    
    // Return non-zero if any tests failed
    return (IntegrationTestRunner::tests_run == IntegrationTestRunner::tests_passed) ? 0 : 1;
}
