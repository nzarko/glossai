#include "lineparser.h"
#include <algorithm>
#include <cctype>

LineParser::LineParser()
    : m_braceLevel(0)
    , m_parenLevel(0)
    , m_bracketLevel(0)
    , m_inString(false)
    , m_inComment(false)
{
}

bool LineParser::processLine(const std::string &line)
{
    // Add the line to our buffer
    m_lines.push_back(line);
    
    // Update nesting levels
    updateLevels(line);
    
    // Check if the statement is complete
    return isComplete();
}

std::string LineParser::getCode() const
{
    std::string result;
    for (size_t i = 0; i < m_lines.size(); ++i) {
        if (i > 0) result += "\n";
        result += m_lines[i];
    }
    return result;
}

void LineParser::clear()
{
    m_lines.clear();
    m_braceLevel = 0;
    m_parenLevel = 0;
    m_bracketLevel = 0;
    m_inString = false;
    m_inComment = false;
}

bool LineParser::needsContinuation() const
{
    // If we have no lines, we don't need continuation (ready for new input)
    if (m_lines.empty()) {
        return false;
    }
    
    // If we have lines but statement is incomplete, we need continuation
    return !isComplete();
}

std::string LineParser::getContinuationPrompt() const
{
    std::string prompt = "... ";
    
    // Add indentation based on brace level
    int indentLevel = m_braceLevel;
    for (int i = 0; i < indentLevel; ++i) {
        prompt += "    "; // 4 spaces per indent level
    }
    
    return prompt;
}

void LineParser::updateLevels(const std::string &line)
{
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        // Handle string literals
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
            continue;
        }
        
        if (inString) {
            if (c == stringChar && (i == 0 || line[i-1] != '\\')) {
                inString = false;
                stringChar = 0;
            }
            continue;
        }
        
        // Handle comments
        if (c == '#') {
            break; // Rest of line is comment
        }
        
        // Count nesting levels
        switch (c) {
            case '{': m_braceLevel++; break;
            case '}': m_braceLevel--; break;
            case '(': m_parenLevel++; break;
            case ')': m_parenLevel--; break;
            case '[': m_bracketLevel++; break;
            case ']': m_bracketLevel--; break;
        }
    }
}

bool LineParser::isComplete() const
{
    // If we don't have any lines, we're ready for input (not complete)
    if (m_lines.empty()) {
        return false;
    }
    
    // If we have unmatched brackets, we need continuation
    if (m_braceLevel > 0 || m_parenLevel > 0 || m_bracketLevel > 0) {
        return false;
    }
    
    // If brackets are balanced and we have content, we're complete
    return true;
}

std::string LineParser::trim(const std::string &str) const
{
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }
    
    auto end = str.end();
    do {
        end--;
    } while (end > start && std::isspace(*end));
    
    return std::string(start, end + 1);
}

std::string LineParser::getIndentedCode() const
{
    std::string result;
    int currentIndent = 0;
    
    for (size_t i = 0; i < m_lines.size(); ++i) {
        std::string line = m_lines[i];
        std::string trimmedLine = trim(line);
        
        // Skip empty lines
        if (trimmedLine.empty()) {
            result += "\n";
            continue;
        }
        
        // Decrease indent for closing braces
        if (trimmedLine[0] == '}') {
            currentIndent = std::max(0, currentIndent - 1);
        }
        
        // Add indentation
        if (i > 0) result += "\n";
        for (int j = 0; j < currentIndent; ++j) {
            result += "    "; // 4 spaces per level
        }
        result += trimmedLine;
        
        // Increase indent for opening braces
        if (trimmedLine.back() == '{') {
            currentIndent++;
        }
    }
    
    return result;
}
