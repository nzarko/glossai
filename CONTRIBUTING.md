# Contributing to GlossAI

Thank you for your interest in contributing to GlossAI! This document provides guidelines and information for contributors.

## 🚀 Getting Started

### Prerequisites
- C++17 compatible compiler
- Qt6 development libraries
- CMake 3.16+
- Git

### Setting Up Development Environment
1. Fork the repository
2. Clone your fork:
   ```bash
   git clone https://github.com/yourusername/glossai.git
   cd glossai
   ```
3. Build the project:
   ```bash
   cmake -B build -S .
   cmake --build build
   ```
4. Run tests to ensure everything works:
   ```bash
   ctest --test-dir build
   ```

## 🏗️ Architecture Guidelines

### Core Principles
- **Pure C++ Core**: The `core/` directory must remain Qt-free
- **Clean Interfaces**: Use abstract base classes and clear APIs
- **Memory Safety**: Use smart pointers and RAII
- **Standard Library**: Prefer `std::` over platform-specific alternatives

### Directory Structure
```
core/       # Pure C++ interpreter logic (NO Qt dependencies)
ui/         # Qt-based user interface components
tests/      # Unit and integration tests
```

## 📝 Coding Standards

### C++ Style
- **Standard**: C++17
- **Naming**: 
  - Classes: `PascalCase` (e.g., `ASTNode`)
  - Functions/Variables: `camelCase` (e.g., `getValue()`)
  - Members: `m_camelCase` (e.g., `m_value`)
  - Constants: `UPPER_CASE` (e.g., `MAX_DEPTH`)
- **Indentation**: 4 spaces (no tabs)
- **Braces**: Allman style for classes/functions, same line for control flow

### Example Code Style
```cpp
class ExampleNode : public ASTNode
{
public:
    ExampleNode(const std::string& name) 
        : m_name(name) {}
    
    std::string getName() const { return m_name; }
    
    void processData() {
        if (m_name.empty()) {
            throw std::invalid_argument("Name cannot be empty");
        }
        // Processing logic...
    }

private:
    std::string m_name;
};
```

## 🧪 Testing Requirements

### Test Coverage
All new features must include:
- **Unit tests** for core functionality
- **Integration tests** for mathematical accuracy
- **Error handling tests** for edge cases

### Test Guidelines
- Tests in `tests/` directory
- Use descriptive test names
- Test both success and failure cases
- Include mathematical edge cases (very large/small numbers, etc.)

### Running Tests
```bash
# Build and run all tests
cmake --build build
ctest --test-dir build

# Run specific test file
./build/tests/test_interpreter
```

## 🔧 Core Development

### Adding Mathematical Functions
1. **Evaluator**: Add function logic in `core/evaluator.cpp`
2. **Tests**: Add comprehensive tests for the new function
3. **Documentation**: Update README with function description

Example:
```cpp
// In evaluator.cpp
if (functionName == "newFunction") {
    if (args.size() != 1) {
        throw std::runtime_error("newFunction expects 1 argument");
    }
    double value = args[0].toNumber();
    return Value(your_calculation(value));
}
```

### Adding New AST Nodes
1. **Header**: Declare in `core/ast.h`
2. **Implementation**: Implement in `core/ast.cpp`
3. **Visitor**: Add to `ASTVisitor` interface
4. **Parser**: Add parsing logic in `core/parser.cpp`
5. **Evaluator**: Add evaluation logic in `core/evaluator.cpp`

## 🎨 UI Development

### Qt Guidelines
- Use Qt6 modern features
- Maintain consistent styling
- Ensure accessibility
- Test on different screen sizes

### UI Components
- Keep business logic in core, UI logic in UI layer
- Use proper string conversion between `std::string` and `QString`
- Handle Qt signals/slots appropriately

## 📋 Pull Request Process

### Before Submitting
1. **Build**: Ensure project builds without warnings
2. **Tests**: All tests must pass
3. **Style**: Follow coding standards
4. **Documentation**: Update relevant documentation

### PR Checklist
- [ ] Code follows project style guidelines
- [ ] All tests pass
- [ ] New features include tests
- [ ] Documentation updated if needed
- [ ] No unnecessary dependencies added
- [ ] Core remains Qt-free (if touching core)

### PR Description Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] All tests pass
- [ ] Manual testing completed

## Screenshots (if UI changes)
(Add screenshots here)
```

## 🐛 Bug Reports

### Bug Report Template
```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Enter expression: '...'
2. Click '...'
3. See error

**Expected behavior**
What you expected to happen.

**Screenshots**
If applicable, add screenshots.

**Environment:**
- OS: [e.g. Windows 10]
- Qt Version: [e.g. 6.8.1]
- Compiler: [e.g. MSVC 2022]
```

## 💡 Feature Requests

### Feature Request Template
```markdown
**Is your feature request related to a problem?**
A clear description of what the problem is.

**Describe the solution you'd like**
A clear description of what you want to happen.

**Describe alternatives you've considered**
Alternative solutions or features you've considered.

**Additional context**
Any other context or screenshots about the feature request.
```

## 🤝 Community Guidelines

### Code of Conduct
- Be respectful and inclusive
- Welcome newcomers
- Focus on constructive feedback
- Help others learn and grow

### Communication Channels
- **Issues**: Bug reports and feature requests
- **Discussions**: General questions and ideas
- **Pull Requests**: Code contributions

## 📚 Resources

### Learning Resources
- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [Modern C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [CMake Documentation](https://cmake.org/documentation/)

### Development Tools
- **IDE**: Visual Studio, Qt Creator, CLion, or VS Code
- **Debugging**: GDB, LLDB, or Visual Studio Debugger
- **Profiling**: Valgrind, AddressSanitizer

## 🙏 Recognition

Contributors will be acknowledged in:
- README.md contributors section
- Release notes for significant contributions
- Special recognition for major features

Thank you for contributing to GlossAI! 🚀
