#include "context.h"
#include <algorithm>

Context::Context()
{
    // Push initial global scope
    pushScope();
}

Context::~Context() = default;

void Context::setVariable(const std::string &name, const Value &value)
{
    if (!m_variableScopes.empty()) {
        m_variableScopes.back()[name] = value;
    }
}

Value Context::getVariable(const std::string &name) const
{
    Value *var = findVariable(name);
    if (var) {
        return *var;
    }
    return Value();
}

bool Context::hasVariable(const std::string &name) const
{
    return findVariable(name) != nullptr;
}

void Context::removeVariable(const std::string &name)
{
    // Remove from current scope only
    if (!m_variableScopes.empty()) {
        m_variableScopes.back().erase(name);
    }
}

void Context::setFunction(const std::string &name, const UserFunction &function)
{
    m_functions[name] = function;
}

UserFunction Context::getFunction(const std::string &name) const
{
    auto it = m_functions.find(name);
    if (it != m_functions.end()) {
        return it->second;
    }
    return UserFunction(); // Return empty function if not found
}

bool Context::hasFunction(const std::string &name) const
{
    return m_functions.find(name) != m_functions.end();
}

void Context::removeFunction(const std::string &name)
{
    m_functions.erase(name);
}

void Context::pushScope()
{
    m_variableScopes.emplace_back();
}

void Context::popScope()
{
    if (m_variableScopes.size() > 1) { // Always keep at least the global scope
        m_variableScopes.pop_back();
    }
}

int Context::getScopeDepth() const
{
    return static_cast<int>(m_variableScopes.size());
}

std::vector<std::string> Context::getIdentifiers() const
{
    std::vector<std::string> identifiers;
    
    // Add all variables from all scopes
    for (const auto &scope : m_variableScopes) {
        for (const auto &pair : scope) {
            if (std::find(identifiers.begin(), identifiers.end(), pair.first) == identifiers.end()) {
                identifiers.push_back(pair.first);
            }
        }
    }
    
    // Add all functions
    for (const auto &pair : m_functions) {
        if (std::find(identifiers.begin(), identifiers.end(), pair.first) == identifiers.end()) {
            identifiers.push_back(pair.first);
        }
    }
    
    std::sort(identifiers.begin(), identifiers.end());
    return identifiers;
}

void Context::clear()
{
    m_variableScopes.clear();
    m_functions.clear();
    pushScope(); // Restore global scope
}

std::unordered_map<std::string, Value> Context::getCurrentScopeVariables() const
{
    if (!m_variableScopes.empty()) {
        return m_variableScopes.back();
    }
    return std::unordered_map<std::string, Value>();
}

std::unordered_map<std::string, UserFunction> Context::getFunctions() const
{
    return m_functions;
}

Value* Context::findVariable(const std::string &name) const
{
    // Search from most recent scope to global scope
    for (auto it = m_variableScopes.rbegin(); it != m_variableScopes.rend(); ++it) {
        auto var_it = it->find(name);
        if (var_it != it->end()) {
            return const_cast<Value*>(&var_it->second);
        }
    }
    return nullptr;
}

// Line parsing functionality
void Context::addPendingLine(const std::string &line)
{
    m_pendingLines.push_back(line);
}

std::string Context::getPendingCode() const
{
    std::string result;
    for (size_t i = 0; i < m_pendingLines.size(); ++i) {
        if (i > 0) result += "\n";
        result += m_pendingLines[i];
    }
    return result;
}

void Context::clearPendingLines()
{
    m_pendingLines.clear();
}

bool Context::hasPendingLines() const
{
    return !m_pendingLines.empty();
}

int Context::getBraceLevel() const
{
    int level = 0;
    for (const auto &line : m_pendingLines) {
        for (char c : line) {
            if (c == '{') level++;
            else if (c == '}') level--;
        }
    }
    return level;
}

int Context::getParenLevel() const
{
    int level = 0;
    for (const auto &line : m_pendingLines) {
        for (char c : line) {
            if (c == '(') level++;
            else if (c == ')') level--;
        }
    }
    return level;
}
