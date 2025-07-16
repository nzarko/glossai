#pragma once

#include "ast.h"
#include "context.h"
#include <memory>

/**
 * @brief Evaluates AST nodes and returns values
 */
class Evaluator : public ASTVisitor
{
public:
    Evaluator();
    ~Evaluator();
    
    /**
     * @brief Evaluate an AST node with given context
     * @param node The AST node to evaluate
     * @param context The execution context
     * @return The result value
     */
    Value evaluate(ASTNode* node, Context* context);
    
    // ASTVisitor interface
    void visit(LiteralNode* node) override;
    void visit(IdentifierNode* node) override;
    void visit(BinaryOpNode* node) override;
    void visit(UnaryOpNode* node) override;
    void visit(FunctionCallNode* node) override;
    void visit(IfNode* node) override;
    void visit(WhileNode* node) override;
    void visit(ForNode* node) override;
    void visit(BlockNode* node) override;
    void visit(FunctionDefNode* node) override;
    void visit(ReturnNode* node) override;
    void visit(PrintNode* node) override;

private:
    Context* m_context;
    Value m_result;
    bool m_hasReturned;
    
    Value evaluateBuiltinFunction(const std::string& name, const std::vector<Value>& args);
};