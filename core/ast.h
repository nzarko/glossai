#pragma once

#include <string>
#include <vector>
#include <memory>

class ASTNode;

/**
 * @brief Represents a value in the interpreter
 * Pure C++ implementation using variant-like behavior
 */
class Value {
public:
    enum Type {
        Null,
        Boolean,
        Number,
        String
    };
    
    Value() : m_type(Null), m_number(0.0), m_boolean(false) {}
    Value(double num) : m_type(Number), m_number(num), m_boolean(false) {}
    Value(bool b) : m_type(Boolean), m_number(0.0), m_boolean(b) {}
    Value(const std::string& str) : m_type(String), m_number(0.0), m_boolean(false), m_string(str) {}
    
    Type getType() const { return m_type; }
    double toNumber() const;
    bool toBool() const;
    std::string toString() const;
    
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const { return !(*this == other); }
    bool operator<(const Value& other) const;
    bool operator>(const Value& other) const { return other < *this; }
    bool operator<=(const Value& other) const { return *this < other || *this == other; }
    bool operator>=(const Value& other) const { return other <= *this; }
    
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    
private:
    Type m_type;
    double m_number;
    bool m_boolean;
    std::string m_string;
};

/**
 * @brief Base class for all AST nodes
 * Pure C++ implementation
 */
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    
    /**
     * @brief Accept a visitor for the visitor pattern
     * @param visitor The visitor to accept
     */
    virtual void accept(class ASTVisitor *visitor) = 0;
    
    /**
     * @brief Get a string representation of the node (for debugging)
     */
    virtual std::string toString() const = 0;
};

/**
 * @brief Visitor interface for AST traversal
 */
class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(class LiteralNode *node) = 0;
    virtual void visit(class IdentifierNode *node) = 0;
    virtual void visit(class BinaryOpNode *node) = 0;
    virtual void visit(class UnaryOpNode *node) = 0;
    virtual void visit(class FunctionCallNode *node) = 0;
    virtual void visit(class IfNode *node) = 0;
    virtual void visit(class WhileNode *node) = 0;
    virtual void visit(class ForNode *node) = 0;
    virtual void visit(class BlockNode *node) = 0;
    virtual void visit(class FunctionDefNode *node) = 0;
    virtual void visit(class ReturnNode *node) = 0;
    virtual void visit(class PrintNode *node) = 0; 
};

/**
 * @brief Binary operators
 */
enum class BinaryOperator {
    Add, Subtract, Multiply, Divide, Power, Mod, Div,
    Equal, NotEqual, Less, Greater, LessEqual, GreaterEqual,
    And, Or, Assign, PlusAssign, MinusAssign, MultiplyAssign, DivideAssign
};

/**
 * @brief Unary operators
 */
enum class UnaryOperator {
    Negate, Not, PreIncrement, PostIncrement, PreDecrement, PostDecrement
};

/**
 * @brief Literal value node (numbers, strings, booleans)
 */
class LiteralNode : public ASTNode
{
public:
    explicit LiteralNode(const Value &value) : m_value(value) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override { return m_value.toString(); }
    
    Value getValue() const { return m_value; }

private:
    Value m_value;
};

/**
 * @brief Identifier node (variable names, function names)
 */
class IdentifierNode : public ASTNode
{
public:
    explicit IdentifierNode(const std::string &name) : m_name(name) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override { return m_name; }
    
    std::string getName() const { return m_name; }

private:
    std::string m_name;
};

/**
 * @brief Binary operation node
 */
class BinaryOpNode : public ASTNode
{
public:
    BinaryOpNode(std::unique_ptr<ASTNode> left, BinaryOperator op, std::unique_ptr<ASTNode> right)
        : m_left(std::move(left)), m_operator(op), m_right(std::move(right)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    ASTNode* getLeft() const { return m_left.get(); }
    ASTNode* getRight() const { return m_right.get(); }
    BinaryOperator getOperator() const { return m_operator; }

private:
    std::unique_ptr<ASTNode> m_left;
    BinaryOperator m_operator;
    std::unique_ptr<ASTNode> m_right;
};

/**
 * @brief Unary operation node
 */
class UnaryOpNode : public ASTNode
{
public:
    UnaryOpNode(UnaryOperator op, std::unique_ptr<ASTNode> operand)
        : m_operator(op), m_operand(std::move(operand)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    UnaryOperator getOperator() const { return m_operator; }
    ASTNode* getOperand() const { return m_operand.get(); }

private:
    UnaryOperator m_operator;
    std::unique_ptr<ASTNode> m_operand;
};

/**
 * @brief Function call node
 */
class FunctionCallNode : public ASTNode
{
public:
    FunctionCallNode(std::unique_ptr<ASTNode> function, std::vector<std::unique_ptr<ASTNode>> arguments)
        : m_function(std::move(function)), m_arguments(std::move(arguments)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    ASTNode* getFunction() const { return m_function.get(); }
    const std::vector<std::unique_ptr<ASTNode>>& getArguments() const { return m_arguments; }

private:
    std::unique_ptr<ASTNode> m_function;
    std::vector<std::unique_ptr<ASTNode>> m_arguments;
};

/**
 * @brief If statement node
 */
class IfNode : public ASTNode
{
public:
    IfNode(std::unique_ptr<ASTNode> condition,
           std::unique_ptr<ASTNode> thenBranch,
           std::unique_ptr<ASTNode> elseBranch = nullptr)
        : m_condition(std::move(condition))
        , m_thenBranch(std::move(thenBranch))
        , m_elseBranch(std::move(elseBranch))
    {}

    void accept(ASTVisitor *visitor) override { visitor->visit(this); }

    std::string toString() const override;
    

    ASTNode *getCondition() const { return m_condition.get(); }
    ASTNode *getThenBranch() const { return m_thenBranch.get(); }
    ASTNode *getElseBranch() const { return m_elseBranch.get(); }

private:
    std::unique_ptr<ASTNode> m_condition;
    std::unique_ptr<ASTNode> m_thenBranch;
    std::unique_ptr<ASTNode> m_elseBranch;
};

/**
 * @brief While loop node
 */
class WhileNode : public ASTNode
{
public:
    WhileNode(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> body)
        : m_condition(std::move(condition)), m_body(std::move(body)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    ASTNode* getCondition() const { return m_condition.get(); }
    ASTNode* getBody() const { return m_body.get(); }

private:
    std::unique_ptr<ASTNode> m_condition;
    std::unique_ptr<ASTNode> m_body;
};

/**
 * @brief For loop node
 */
class ForNode : public ASTNode
{
public:
    ForNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> condition, 
            std::unique_ptr<ASTNode> update, std::unique_ptr<ASTNode> body)
        : m_init(std::move(init)), m_condition(std::move(condition))
        , m_update(std::move(update)), m_body(std::move(body)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    ASTNode* getInit() const { return m_init.get(); }
    ASTNode* getCondition() const { return m_condition.get(); }
    ASTNode* getUpdate() const { return m_update.get(); }
    ASTNode* getBody() const { return m_body.get(); }

private:
    std::unique_ptr<ASTNode> m_init;
    std::unique_ptr<ASTNode> m_condition;
    std::unique_ptr<ASTNode> m_update;
    std::unique_ptr<ASTNode> m_body;
};

/**
 * @brief Block statement node
 */
class BlockNode : public ASTNode
{
public:
    explicit BlockNode(std::vector<std::unique_ptr<ASTNode>> statements)
        : m_statements(std::move(statements)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    const std::vector<std::unique_ptr<ASTNode>>& getStatements() const { return m_statements; }

private:
    std::vector<std::unique_ptr<ASTNode>> m_statements;
};

/**
 * @brief Function definition node
 */
class FunctionDefNode : public ASTNode
{
public:
    FunctionDefNode(const std::string &name, const std::vector<std::string> &parameters, std::unique_ptr<ASTNode> body)
        : m_name(name), m_parameters(parameters), m_body(std::move(body)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    std::string getName() const { return m_name; }
    std::vector<std::string> getParameters() const { return m_parameters; }
    ASTNode* getBody() const { return m_body.get(); }

private:
    std::string m_name;
    std::vector<std::string> m_parameters;
    std::unique_ptr<ASTNode> m_body;
};

/**
 * @brief Return statement node
 */
class ReturnNode : public ASTNode
{
public:
    explicit ReturnNode(std::unique_ptr<ASTNode> value = nullptr)
        : m_value(std::move(value)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    ASTNode* getValue() const { return m_value.get(); }

private:
    std::unique_ptr<ASTNode> m_value;
};


/**
 * @brief Print statement node
 */
class PrintNode : public ASTNode
{
public:
    explicit PrintNode(std::vector<std::unique_ptr<ASTNode>> expressions)
        : m_expressions(std::move(expressions)) {}
    
    void accept(ASTVisitor *visitor) override { visitor->visit(this); }
    std::string toString() const override;
    
    const std::vector<std::unique_ptr<ASTNode>>& getExpressions() const { return m_expressions; }

private:
    std::vector<std::unique_ptr<ASTNode>> m_expressions;
};