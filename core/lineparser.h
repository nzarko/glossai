#pragma once

#include <string>
#include <vector>

/**
 * @brief Handles line-by-line parsing similar to Python's interactive mode
 * 
 * This class determines when a line is complete or needs continuation,
 * handling multi-line constructs like blocks, functions, and control structures.
 */
class LineParser
{
public:
    LineParser();
    ~LineParser() = default;

    /**
     * @brief Process a line of input
     * @param line Input line
     * @return True if the statement is complete and ready to execute
     */
    bool processLine(const std::string &line);
    
    /**
     * @brief Get the accumulated code
     * @return Combined lines ready for parsing
     */
    std::string getCode() const;
    
    /**
     * @brief Clear the accumulated code
     */
    void clear();
    
    /**
     * @brief Check if we're in a multi-line construct
     * @return True if expecting more lines
     */
    bool needsContinuation() const;
    
    /**
     * @brief Get the continuation prompt
     * @return Prompt string for continuation lines
     */
    std::string getContinuationPrompt() const;
    
    /**
     * @brief Get the code with proper indentation
     * @return Formatted code with indentation
     */
    std::string getIndentedCode() const;

private:
    std::vector<std::string> m_lines;
    int m_braceLevel;
    int m_parenLevel;
    int m_bracketLevel;
    bool m_inString;
    bool m_inComment;
    
    // Helper methods
    void updateLevels(const std::string &line);
    bool isComplete() const;
    std::string trim(const std::string &str) const;
};
