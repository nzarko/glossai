# GlossAI - Mathematical Expression Interpreter

A modern C++ mathematical expression interpreter with a clean Qt-based graphical user interface. GlossAI features a pure C++ interpreter core with no Qt dependencies, ensuring excellent performance and portability.

![GlossAI](https://img.shields.io/badge/Language-C%2B%2B17-blue)
![Qt](https://img.shields.io/badge/UI-Qt6-green)
![CMake](https://img.shields.io/badge/Build-CMake-red)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🚀 Features

### Core Interpreter
- **Pure C++ Implementation**: Zero Qt dependencies in the core interpreter
- **Mathematical Functions**: Comprehensive support for trigonometric, logarithmic, and algebraic functions
- **Variables**: Support for variable assignment and manipulation
- **Expression Evaluation**: Real-time mathematical expression parsing and evaluation
- **Error Handling**: Robust error reporting and recovery

### Supported Mathematical Functions
- **Basic Operations**: `+`, `-`, `*`, `/`, `^` (power)
- **Trigonometric**: `sin()`, `cos()`, `tan()`, `asin()`, `acos()`, `atan()`
- **Logarithmic**: `log()` (natural log), `log10()` (base-10 log)
- **Exponential**: `exp()`, `pow(x,y)`
- **Root Functions**: `sqrt()`
- **Utility**: `abs()`, `ceil()`, `floor()`

### User Interface
- **Modern Qt6 Interface**: Clean, responsive design
- **Real-time Evaluation**: Instant expression evaluation
- **Variable Management**: Visual display of defined variables
- **History**: Command history with reusable expressions
- **Help System**: Built-in function reference
- **Syntax Highlighting**: Enhanced readability

## 🏗️ Architecture

```
📁 glossai/
├── 📁 core/           # Pure C++ interpreter (no Qt dependencies)
│   ├── interpreter.*  # Main interpreter interface
│   ├── lexer.*       # Tokenization and lexical analysis
│   ├── parser.*      # AST generation and parsing
│   ├── ast.*         # Abstract Syntax Tree implementation
│   ├── evaluator.*   # Expression evaluation engine
│   └── context.*     # Variable and function context
├── 📁 ui/            # Qt-based user interface
│   ├── interpreterwidget.*  # Main UI widget
│   └── interpreterwidget.ui # UI layout file
├── 📁 tests/         # Comprehensive test suite
│   ├── test_interpreter.cpp      # Basic functionality tests
│   └── test_calc_integration.cpp # Mathematical accuracy tests
└── 📄 main.cpp       # Application entry point
```

## 🛠️ Building

### Prerequisites
- **CMake** 3.16 or higher
- **Qt6** (Core and Widgets modules)
- **C++17** compatible compiler
  - Visual Studio 2019+ (Windows)
  - GCC 8+ (Linux)
  - Clang 7+ (macOS)

### Build Instructions

#### Windows (Visual Studio)
```bash
# Configure
cmake -B build -S . -G "Visual Studio 17 2022"

# Build
cmake --build build --config Release

# Run
build\bin\Release\glossai.exe
```

#### Linux/macOS
```bash
# Configure
cmake -B build -S . -G "Unix Makefiles"

# Build
cmake --build build

# Run
./build/bin/glossai
```

### CMake Options
```bash
# Specify Qt installation path (if needed)
cmake -B build -S . -DCMAKE_PREFIX_PATH="/path/to/Qt6"

# Build with tests
cmake --build build --target all
```

## 🧪 Testing

Run the comprehensive test suite:

```bash
# Build tests
cmake --build build --config Release

# Run all tests
ctest --test-dir build --config Release

# Run specific tests
build/tests/Release/test_interpreter.exe
build/tests/Release/test_calc_integration.exe
```

### Test Coverage
- **Basic Arithmetic**: Addition, subtraction, multiplication, division
- **Mathematical Functions**: Trigonometric, logarithmic, exponential functions
- **Variables**: Assignment, retrieval, and manipulation
- **Error Handling**: Invalid syntax, undefined variables, division by zero
- **Complex Expressions**: Multi-step calculations and nested function calls
- **Numerical Stability**: Precision testing with small and large numbers

## 📖 Usage Examples

### Basic Arithmetic
```
>> 2 + 3 * 4
14

>> (5 + 3) * 2
16
```

### Variables
```
>> x = 5
5

>> y = x * 2
10

>> x + y
15
```

### Mathematical Functions
```
>> sin(3.14159/2)
1

>> sqrt(16)
4

>> log(exp(2))
2

>> pow(2, 3)
8
```

### Complex Expressions
```
>> sqrt(pow(3,2) + pow(4,2))
5

>> a = 2; b = 3; sqrt(a^2 + b^2)
3.60555
```

## 🎯 Design Philosophy

### Clean Architecture
- **Separation of Concerns**: Core logic completely independent of UI
- **Testability**: Pure C++ core enables comprehensive unit testing
- **Portability**: Core can be used in console applications, web backends, or embedded systems
- **Maintainability**: Clear interfaces and modular design

### Performance
- **Efficient Parsing**: Custom lexer and parser optimized for mathematical expressions
- **Memory Management**: Smart pointers and RAII throughout
- **Standard Library**: Uses `std::cmath` for mathematical operations (extensible to other libraries)

## 🔧 Configuration

### Qt Path Configuration
If Qt is not in the standard location, set the path in your environment or CMake:

```bash
# Environment variable
export CMAKE_PREFIX_PATH="/path/to/Qt6"

# Or in CMakeLists.txt
set(CMAKE_PREFIX_PATH "/path/to/Qt6")
```

### Mathematical Library
The default implementation uses `std::cmath`. To use alternative libraries:

1. **Boost.Math**: Modify `evaluator.cpp` to include Boost headers
2. **Intel MKL**: Link against MKL libraries in CMakeLists.txt
3. **Custom Functions**: Extend the evaluator with domain-specific functions

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Coding Standards
- **C++17** features encouraged
- **Pure C++** in core library (no Qt dependencies)
- **Comprehensive tests** for new features
- **Documentation** for public APIs

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with modern C++17 and Qt6
- Inspired by mathematical expression parsers and interpreters
- Thanks to the Qt and C++ communities

## 📊 Project Status

- ✅ **Core Architecture**: Complete
- ✅ **Basic Operations**: Implemented
- ✅ **Mathematical Functions**: Comprehensive set available
- ✅ **Qt UI**: Modern interface complete
- ✅ **Test Suite**: Comprehensive coverage
- 🔄 **Advanced Features**: In development
- 🔄 **Documentation**: Expanding

---

**Built with ❤️ using C++17 and Qt6**
