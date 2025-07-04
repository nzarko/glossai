#pragma once

#include "ast.h"
#include <functional>
#include <unordered_map>

class Context;

/**
 * @brief Evaluates AST nodes and produces results
 * 
 * The evaluator implements the visitor pattern to traverse the AST
 * and compute the values of expressions and execute statements.
 * Uses standard C++ math library for calculations.
 */
class Evaluator : public ASTVisitor
{
public:
    Evaluator();
    
    /**
     * @brief Evaluate an AST node in the given context
     * @param node The AST node to evaluate
     * @param context The execution context
     * @return The result of evaluation
     */
    Value evaluate(ASTNode *node, Context *context);
    
    /**
     * @brief Get the last evaluation error
     * @return Error message string
     */
    std::string getLastError() const { return m_lastError; }

    // Visitor pattern implementation
    void visit(LiteralNode *node) override;
    void visit(IdentifierNode *node) override;
    void visit(BinaryOpNode *node) override;
    void visit(UnaryOpNode *node) override;
    void visit(FunctionCallNode *node) override;
    void visit(IfNode *node) override;
    void visit(WhileNode *node) override;
    void visit(ForNode *node) override;
    void visit(BlockNode *node) override;
    void visit(FunctionDefNode *node) override;
    void visit(ReturnNode *node) override;

private:
    // Helper methods
    Value evaluateBinaryOp(BinaryOperator op, const Value &left, const Value &right);
    Value evaluateUnaryOp(UnaryOperator op, const Value &operand);
    bool isTruthy(const Value &value);
    Value callBuiltinFunction(const std::string &name, const std::vector<Value> &args);
    
    // Built-in mathematical functions using std::cmath
    void setupBuiltinFunctions();
    Value builtinPrint(const std::vector<Value> &args);
    Value builtinSqrt(const std::vector<Value> &args);
    Value builtinPow(const std::vector<Value> &args);
    Value builtinAbs(const std::vector<Value> &args);
    Value builtinSin(const std::vector<Value> &args);
    Value builtinCos(const std::vector<Value> &args);
    Value builtinTan(const std::vector<Value> &args);
    Value builtinLog(const std::vector<Value> &args);
    Value builtinExp(const std::vector<Value> &args);
    Value builtinFloor(const std::vector<Value> &args);
    Value builtinCeil(const std::vector<Value> &args);
    Value builtinRound(const std::vector<Value> &args);
    Value builtinMin(const std::vector<Value> &args);
    Value builtinMax(const std::vector<Value> &args);
    
    Context *m_context;
    Value m_result;
    std::string m_lastError;
    std::unordered_map<std::string, std::function<Value(const std::vector<Value>&)>> m_builtinFunctions;
    
    // Control flow
    bool m_shouldReturn;
    Value m_returnValue;
};
