/**
 * @file cmd_main.cpp
 * @brief Command-line interface for GlossAI mathematical expression interpreter
 *
 * This file provides a REPL (Read-Eval-Print Loop) interface for the GlossAI
 * interpreter, allowing users to interact with the mathematical expression
 * interpreter from the command line.
 */

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core/interpreter.h"
#include "core/lineparser.h"

namespace
{

bool executeFile(const std::string &filename, Interpreter &interpreter);

/**
 * @brief Global settings for REPL behavior
 */
struct REPLSettings {
    bool showIndentation = true;
    bool showLineNumbers = false;
    bool colorOutput = false;
};

REPLSettings g_replSettings;

/**
 * @brief Print the welcome message and basic usage instructions
 */
void printWelcome()
{
    std::cout << "======================================\n";
    std::cout << "       GlossAI Command Line Tool      \n";
    std::cout << "   Mathematical Expression Interpreter\n";
    std::cout << "======================================\n\n";
    std::cout << "Usage:\n";
    std::cout << "  - Enter mathematical expressions to evaluate\n";
    std::cout << "  - Type 'help' for more commands\n";
    std::cout << "  - Type 'quit' or 'exit' to quit\n";
    std::cout << "  - Use Ctrl+C to interrupt\n\n";
    std::cout << "Examples:\n";
    std::cout << "  > 2 + 3 * 4\n";
    std::cout << "  > sin(pi/2)\n";
    std::cout << "  > x = 10\n";
    std::cout << "  > x * 2 + 1\n\n";
}

    /**
     * @brief Print help information
     */
    // In cmd_main.cpp, update the printHelp function:
    // In cmd_main.cpp, update printHelp():
void printHelp()
{
    std::cout << "\nGlossAI Commands:\n";
    std::cout << "  help          - Show this help message\n";
    std::cout << "  quit, exit    - Exit the interpreter\n";
    std::cout << "  clear         - Clear all variables and functions\n";
    std::cout << "  vars          - Show all defined variables\n";
    std::cout << "  funcs         - Show all available built-in functions\n";
    std::cout << "  version       - Show version information\n";
    std::cout << "  load <file>   - Load and execute a .glo file\n";
    std::cout << "  indent        - Toggle auto-indentation (currently " << (g_replSettings.showIndentation ? "ON" : "OFF") << ")\n";
    std::cout << "  lines         - Toggle line numbers (currently " << (g_replSettings.showLineNumbers ? "ON" : "OFF") << ")\n";
    std::cout << "\nPrompts:\n";
    std::cout << "  >             - Ready for new statement\n";
    std::cout << "  ...           - Continuation line (multi-line statement)\n";
    std::cout << "\nMathematical Operations:\n";
    std::cout << "  +, -, *, /    - Basic arithmetic\n";
    std::cout << "  ^, **         - Exponentiation\n";
    std::cout << "  MOD, %        - Modulo operation (remainder)\n";
    std::cout << "  DIV           - Integer division\n";
    std::cout << "  sin, cos, tan - Trigonometric functions\n";
    std::cout << "  log, ln       - Logarithmic functions\n";
    std::cout << "  sqrt, cbrt    - Square and cube roots\n";
    std::cout << "  root(n,x)     - nth root of x\n";
    std::cout << "  pi, e         - Mathematical constants\n";
    std::cout << "  x = value     - Variable assignment\n";
    std::cout << "  print expr    - Print expressions\n";
    std::cout << "  if (cond) expr else expr - Conditional expression\n";
    std::cout << "  while (cond) statement   - While loop\n";
    std::cout << "\nMulti-line Example:\n";
    std::cout << "  > while (i < 3) {\n";
    std::cout << "  ...     print \"i =\", i\n";
    std::cout << "  ...     i = i + 1\n";
    std::cout << "  ... }\n";
    std::cout << "\nType 'funcs' for complete function list.\n\n";
}
    /**
     * @brief Print version information
     */
    void printVersion()
    {
        std::cout << "\nGlossAI v1.0.0\n";
        std::cout << "Mathematical Expression Interpreter\n";
        std::cout << "Built with C++17\n\n";
    }

    /**
     * @brief Trim whitespace from both ends of a string
     * @param str The string to trim
     * @return Trimmed string
     */
    std::string trim(const std::string &str)
    {
        auto start = str.begin();
        while (start != str.end() && std::isspace(*start))
        {
            start++;
        }

        auto end = str.end();
        do
        {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));

        return std::string(start, end + 1);
    }

    /**
     * @brief Convert string to lowercase
     * @param str The string to convert
     * @return Lowercase string
     */
    std::string toLowerCase(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    /**
     * @brief Check if the input is a special command
     * @param input The user input
     * @param interpreter The interpreter instance
     * @return True if command was handled, false if it should be evaluated
     */
    // In cmd_main.cpp, update the handleCommand function:
    bool handleCommand(const std::string &input, Interpreter &interpreter)
    {
        std::string cmd = toLowerCase(trim(input));

        if (cmd == "help")
        {
            printHelp();
            return true;
        }

        if (cmd == "quit" || cmd == "exit")
        {
            std::cout << "Goodbye!\n";
            return true;
        }

        if (cmd == "clear")
        {
            interpreter.clearContext();
            std::cout << "Context cleared.\n";
            return true;
        }

        // Handle 'load filename' command
        if (cmd.substr(0, 4) == "load") {
            if (cmd.length() > 5) {
                std::string filename = trim(cmd.substr(5));
                executeFile(filename, interpreter);
            } else {
                std::cout << "Usage: load <filename.glo>\n";
            }
            return true;
        }

        if (cmd == "vars")
        {
            auto identifiers = interpreter.getAvailableIdentifiers();
            if (identifiers.empty())
            {
                std::cout << "No variables defined.\n";
            }
            else
            {
                std::cout << "Defined variables:\n";
                for (const auto &id : identifiers)
                {
                    std::cout << "  " << id << "\n";
                }
            }
            return true;
        }

        if (cmd == "funcs")
        {
            auto functions = interpreter.getBuiltinFunctions();
            std::cout << "Available built-in functions:\n";

            // Group functions by category
            std::cout << "\nTrigonometric:\n";
            std::cout << "  sin(x), cos(x), tan(x)\n";
            std::cout << "  asin(x), acos(x), atan(x)\n";

            std::cout << "\nLogarithmic:\n";
            std::cout << "  log(x)    - Natural logarithm (ln)\n";
            std::cout << "  log10(x)  - Base-10 logarithm\n";
            std::cout << "  log2(x)   - Base-2 logarithm\n";
            std::cout << "  exp(x)    - e^x\n";

            std::cout << "\nRoots & Powers:\n";
            std::cout << "  sqrt(x)   - Square root\n";
            std::cout << "  cbrt(x)   - Cube root\n";
            std::cout << "  root(n,x) - nth root of x\n";
            std::cout << "  pow(x,y)  - x^y\n";

            std::cout << "\nUtility:\n";
            std::cout << "  abs(x)    - Absolute value\n";
            std::cout << "  min(x,y)  - Minimum of x and y\n";
            std::cout << "  max(x,y)  - Maximum of x and y\n";

            std::cout << "\nConstants:\n";
            std::cout << "  pi, e, tau, phi, sqrt2, sqrt3, ln2, ln10\n\n";

            return true;
        }

        if (cmd == "version")
        {
            printVersion();
            return true;
        }
        
        // Handle indentation toggle
        if (cmd == "indent") {
            g_replSettings.showIndentation = !g_replSettings.showIndentation;
            std::cout << "Indentation " << (g_replSettings.showIndentation ? "enabled" : "disabled") << std::endl;
            return true;
        }
        
        // Handle line numbers toggle
        if (cmd == "lines") {
            g_replSettings.showLineNumbers = !g_replSettings.showLineNumbers;
            std::cout << "Line numbers " << (g_replSettings.showLineNumbers ? "enabled" : "disabled") << std::endl;
            return true;
        }

        return false;
    }

    /**
     * @brief Process a single line of input
     * @param input The user input
     * @param interpreter The interpreter instance
     * @return True to continue, false to exit
     */
    bool processInput(const std::string &input, Interpreter &interpreter)
    {
        std::string trimmedInput = trim(input);

        // Skip empty lines
        if (trimmedInput.empty())
        {
            return true;
        }

        // Handle special commands
        if (handleCommand(trimmedInput, interpreter))
        {
            std::string cmd = toLowerCase(trimmedInput);
            return !(cmd == "quit" || cmd == "exit");
        }

        // Execute the mathematical expression
        try
        {
            std::string result = interpreter.execute(trimmedInput);
            if (!result.empty())
            {
                std::cout << "  = " << result << "\n";
            }

            // Check for errors
            std::string error = interpreter.getLastError();
            if (!error.empty())
            {
                std::cout << "Error: " << error << "\n";
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Error: " << e.what() << "\n";
        }
        catch (...)
        {
            std::cout << "Error: Unknown error occurred\n";
        }

        return true;
    }

    /**
     * @brief Run the interactive REPL loop
     * @param interpreter The interpreter instance
     */
    void runREPL(Interpreter &interpreter)
    {
        std::string input;
        LineParser lineParser;

        while (true)
        {
            // Show appropriate prompt
            if (lineParser.needsContinuation()) {
                std::cout << lineParser.getContinuationPrompt();
            } else {
                std::cout << "> ";
            }

            if (!std::getline(std::cin, input))
            {
                // Handle Ctrl+D (EOF)
                std::cout << "\nGoodbye!\n";
                break;
            }

            // Process the line
            bool isComplete = lineParser.processLine(input);
            
            if (isComplete) {
                // Get the complete code (with or without indentation)
                std::string code = g_replSettings.showIndentation ? 
                    lineParser.getIndentedCode() : lineParser.getCode();
                
                // Clear the line parser for the next statement
                lineParser.clear();
                
                // Process the complete statement
                if (!processInput(code, interpreter)) {
                    break;
                }
            }
            // If not complete, continue to next line (continuation)
        }
    }

    /**
     * @brief Execute a single expression from command line arguments
     * @param expression The expression to evaluate
     * @param interpreter The interpreter instance
     */
    void executeExpression(const std::string &expression, Interpreter &interpreter)
    {
        try {
            std::string result = interpreter.execute(expression);

            // Don't show result for print statements OR block statements
            if (!result.empty() && result != "null") {
                std::cout << "  = " << result << std::endl;
            }

            // Check for errors
            std::string error = interpreter.getLastError();
            if (!error.empty()) {
                std::cerr << "Error: " << error << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Error: Unknown error occurred" << std::endl;
        }
    }
    /**
     * @brief Print usage information
     * @param programName The program name
     */
    void printUsage(const char *programName)
    {
        std::cout << "Usage: " << programName << " [OPTIONS] [EXPRESSION|FILE]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help     Show this help message\n";
        std::cout << "  -v, --version  Show version information\n";
        std::cout << "  -i, --interactive  Start interactive mode (default if no expression)\n";
        std::cout << "\nExamples:\n";
        std::cout << "  " << programName << "                    # Start interactive mode\n";
        std::cout << "  " << programName << " \"2 + 3 * 4\"        # Evaluate expression\n";
        std::cout << "  " << programName
                  << " \"sin(pi/2)\"        # Evaluate trigonometric expression\n";
        std::cout << "  " << programName << " script.glo         # Execute GlossAI file\n";
        std::cout << "\nFile Format:\n";
        std::cout << "  GlossAI files (.glo) contain one expression per line\n";
        std::cout << "  Lines starting with # are comments and are ignored\n";
        std::cout << "  Empty lines are ignored\n";
        std::cout << "\nIn interactive mode, type 'help' for more commands.\n";
    }

    /**
     * @brief Execute a GlossAI file
     * @param filename The path to the .glo file
     * @param interpreter The interpreter instance
     * @return True if successful, false on error
     */
    bool executeFile(const std::string &filename, Interpreter &interpreter)
    {
        // Check if file exists
        if (!std::filesystem::exists(filename)) {
            std::cerr << "Error: File '" << filename << "' not found" << std::endl;
            return false;
        }

        // Open file
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
            return false;
        }

        std::cout << "Executing file: " << filename << std::endl;
        std::cout << "===========================================" << std::endl;

        std::string line;
        int lineNumber = 1;
        bool hasError = false;

        // Read and execute line by line
        while (std::getline(file, line)) {
            std::string trimmedLine = trim(line);

            // Skip empty lines and comments
            if (trimmedLine.empty() || trimmedLine[0] == '#') {
                lineNumber++;
                continue;
            }

            // Show line being executed (optional)
            std::cout << "Line " << lineNumber << ": " << trimmedLine << std::endl;

            try {
                std::string result = interpreter.execute(trimmedLine);

                // Show result if not empty
                if (!result.empty() && result != "null") {
                    std::cout << "  = " << result << std::endl;
                }

                // Check for errors
                std::string error = interpreter.getLastError();
                if (!error.empty()) {
                    std::cerr << "Error on line " << lineNumber << ": " << error << std::endl;
                    hasError = true;
                }
            } catch (const std::exception &e) {
                std::cerr << "Error on line " << lineNumber << ": " << e.what() << std::endl;
                hasError = true;
            }

            lineNumber++;
        }

        file.close();

        std::cout << "===========================================" << std::endl;
        if (hasError) {
            std::cout << "File execution completed with errors" << std::endl;
        } else {
            std::cout << "File execution completed successfully" << std::endl;
        }

        return !hasError;
    }

    /**
     * @brief Check if a file has .glo extension
     * @param filename The filename to check
     * @return True if file has .glo extension
     */
    bool isGlossAIFile(const std::string &filename)
    {
        if (filename.length() < 4)
            return false;

        std::string extension = filename.substr(filename.length() - 4);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        return extension == ".glo";
    }

} // anonymous namespace

/**
 * @brief Main entry point for the command-line interface
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit status
 */
int main(int argc, char *argv[])
{
    try {
        // Create interpreter instance
        Interpreter interpreter;

        // Parse command-line arguments
        if (argc == 1) {
            // No arguments - start interactive mode
            printWelcome();
            runREPL(interpreter);
        } else if (argc == 2) {
            std::string arg = argv[1];

            if (arg == "-h" || arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-v" || arg == "--version") {
                printVersion();
                return 0;
            } else if (arg == "-i" || arg == "--interactive") {
                printWelcome();
                runREPL(interpreter);
            } else if (isGlossAIFile(arg)) {
                // Execute GlossAI file
                bool success = executeFile(arg, interpreter);
                return success ? 0 : 1;
            } else {
                // Treat as expression to evaluate
                executeExpression(arg, interpreter);
            }
        } else {
            // Multiple arguments - combine as single expression
            std::string expression;
            for (int i = 1; i < argc; ++i) {
                if (i > 1)
                    expression += " ";
                expression += argv[i];
            }
            executeExpression(expression, interpreter);
        }

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown error occurred" << std::endl;
        return 1;
    }
}
