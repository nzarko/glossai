#include "interpreterwidget.h"
#include "../core/interpreter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QApplication>
#include <QFont>
#include <QScrollBar>

InterpreterWidget::InterpreterWidget(QWidget *parent)
    : QWidget(parent)
    , m_interpreter(std::make_unique<Interpreter>()) // Pure C++ interpreter
    , m_historyIndex(-1)
{
    setupUI();
    setupConnections();
    loadHistory();
    showWelcomeMessage();
    
    m_inputLine->setFocus();
}

InterpreterWidget::~InterpreterWidget()
{
    saveHistory();
}

void InterpreterWidget::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Left panel - Console
    m_consoleWidget = new QWidget;
    auto *consoleLayout = new QVBoxLayout(m_consoleWidget);
    
    // Output display
    m_outputDisplay = new QTextEdit;
    m_outputDisplay->setReadOnly(true);
    m_outputDisplay->setFont(QFont("Consolas", 10));
    m_outputDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: #1e1e1e;"
        "    color: #ffffff;"
        "    border: 1px solid #3e3e3e;"
        "    padding: 8px;"
        "    selection-background-color: #4CAF50;"
        "}"
    );
    
    // Input area
    auto *inputLayout = new QHBoxLayout;
    m_inputLine = new QLineEdit;
    m_inputLine->setFont(QFont("Consolas", 10));
    m_inputLine->setStyleSheet(
        "QLineEdit {"
        "    background-color: #2d2d2d;"
        "    color: #ffffff;"
        "    border: 2px solid #3e3e3e;"
        "    padding: 6px;"
        "    border-radius: 3px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #4CAF50;"
        "}"
    );
    m_inputLine->setPlaceholderText("Enter mathematical expression or command...");
    
    m_executeButton = new QPushButton("Execute");
    m_executeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 16px;"
        "    border-radius: 3px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
    );
    
    inputLayout->addWidget(m_inputLine);
    inputLayout->addWidget(m_executeButton);
    
    // Control buttons
    auto *buttonLayout = new QHBoxLayout;
    m_clearButton = new QPushButton("Clear Output");
    m_loadButton = new QPushButton("Load File");
    m_saveButton = new QPushButton("Save Session");
    m_helpButton = new QPushButton("Help");
    
    QString buttonStyle = 
        "QPushButton {"
        "    background-color: #555;"
        "    color: white;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    border-radius: 3px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #666;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #444;"
        "}";
    
    m_clearButton->setStyleSheet(buttonStyle);
    m_loadButton->setStyleSheet(buttonStyle);
    m_saveButton->setStyleSheet(buttonStyle);
    m_helpButton->setStyleSheet(buttonStyle);
    
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addWidget(m_loadButton);
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_helpButton);
    buttonLayout->addStretch();
    
    // Status label
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("color: #4CAF50; font-weight: bold; padding: 4px;");
    
    consoleLayout->addWidget(m_outputDisplay);
    consoleLayout->addLayout(inputLayout);
    consoleLayout->addLayout(buttonLayout);
    consoleLayout->addWidget(m_statusLabel);
    
    // Right panel - History and Variables
    m_sidePanel = new QWidget;
    auto *sidePanelLayout = new QVBoxLayout(m_sidePanel);
    
    m_rightSplitter = new QSplitter(Qt::Vertical);
    
    // History section
    auto *historyWidget = new QWidget;
    auto *historyLayout = new QVBoxLayout(historyWidget);
    
    auto *historyLabel = new QLabel("Command History:");
    historyLabel->setStyleSheet("font-weight: bold; color: #ffffff; margin: 4px;");
    historyLayout->addWidget(historyLabel);
    
    m_historyList = new QListWidget;
    m_historyList->setStyleSheet(
        "QListWidget {"
        "    background-color: #2d2d2d;"
        "    color: #ffffff;"
        "    border: 1px solid #3e3e3e;"
        "    border-radius: 3px;"
        "    padding: 4px;"
        "}"
        "QListWidget::item {"
        "    padding: 4px;"
        "    border-radius: 2px;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #455a64;"
        "}"
    );
    historyLayout->addWidget(m_historyList);
    
    // Variables section
    auto *variableWidget = new QWidget;
    auto *variableLayout = new QVBoxLayout(variableWidget);
    
    auto *varHeaderLayout = new QHBoxLayout;
    auto *variableLabel = new QLabel("Variables:");
    variableLabel->setStyleSheet("font-weight: bold; color: #ffffff; margin: 4px;");
    varHeaderLayout->addWidget(variableLabel);
    
    m_clearContextButton = new QPushButton("Clear");
    m_clearContextButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #f44336;"
        "    color: white;"
        "    border: none;"
        "    padding: 4px 8px;"
        "    border-radius: 3px;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #da190b;"
        "}"
    );
    varHeaderLayout->addWidget(m_clearContextButton);
    
    variableLayout->addLayout(varHeaderLayout);
    
    m_variableList = new QListWidget;
    m_variableList->setStyleSheet(m_historyList->styleSheet());
    variableLayout->addWidget(m_variableList);
    
    m_rightSplitter->addWidget(historyWidget);
    m_rightSplitter->addWidget(variableWidget);
    m_rightSplitter->setSizes({250, 250});
    
    sidePanelLayout->addWidget(m_rightSplitter);
    
    // Add to main splitter
    m_mainSplitter->addWidget(m_consoleWidget);
    m_mainSplitter->addWidget(m_sidePanel);
    m_mainSplitter->setSizes({700, 300});
    
    mainLayout->addWidget(m_mainSplitter);
    
    // Set minimum sizes
    m_consoleWidget->setMinimumWidth(500);
    m_sidePanel->setMinimumWidth(250);
    
    // Set overall styling
    setStyleSheet(
        "QWidget {"
        "    background-color: #263238;"
        "    color: #ffffff;"
        "}"
        "QSplitter::handle {"
        "    background-color: #37474f;"
        "    width: 3px;"
        "    height: 3px;"
        "}"
    );
}

void InterpreterWidget::setupConnections()
{
    connect(m_executeButton, &QPushButton::clicked, this, &InterpreterWidget::executeCommand);
    connect(m_inputLine, &QLineEdit::returnPressed, this, &InterpreterWidget::onInputReturnPressed);
    connect(m_clearButton, &QPushButton::clicked, this, &InterpreterWidget::clearOutput);
    connect(m_loadButton, &QPushButton::clicked, this, &InterpreterWidget::loadFile);
    connect(m_saveButton, &QPushButton::clicked, this, &InterpreterWidget::saveSession);
    connect(m_helpButton, &QPushButton::clicked, this, &InterpreterWidget::showHelp);
    connect(m_clearContextButton, &QPushButton::clicked, this, &InterpreterWidget::clearContext);
    connect(m_historyList, &QListWidget::itemDoubleClicked, this, &InterpreterWidget::onHistoryItemDoubleClicked);
}

void InterpreterWidget::executeCommand()
{
    QString command = m_inputLine->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    
    // Add to history
    addToHistory(command);
    
    // Display command
    appendOutput(QString("> %1").arg(command), "color: #81C784; font-weight: bold;");
    
    // Execute command using pure C++ interpreter
    m_statusLabel->setText("Executing...");
    m_statusLabel->setStyleSheet("color: #FF9800; font-weight: bold; padding: 4px;");
    QApplication::processEvents();
    
    std::string result = m_interpreter->execute(qtToStd(command));
    
    if (!m_interpreter->getLastError().empty()) {
        showError(stdToQt(m_interpreter->getLastError()));
        m_statusLabel->setText("Error");
        m_statusLabel->setStyleSheet("color: #f44336; font-weight: bold; padding: 4px;");
    } else {
        if (!result.empty()) {
            appendOutput(stdToQt(result), "color: #E0E0E0;");
        }
        m_statusLabel->setText("Ready");
        m_statusLabel->setStyleSheet("color: #4CAF50; font-weight: bold; padding: 4px;");
    }
    
    // Update variable list
    updateVariableList();
    
    // Clear input
    m_inputLine->clear();
    m_historyIndex = -1;
}

void InterpreterWidget::clearOutput()
{
    m_outputDisplay->clear();
    showWelcomeMessage();
}

void InterpreterWidget::clearContext()
{
    m_interpreter->clearContext();
    updateVariableList();
    appendOutput("Context cleared. All variables and functions removed.", "color: #FF9800; font-style: italic;");
    m_statusLabel->setText("Context Cleared");
    m_statusLabel->setStyleSheet("color: #FF9800; font-weight: bold; padding: 4px;");
}

void InterpreterWidget::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Load GlossAI Script", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "GlossAI Files (*.gai *.txt *.math);;All Files (*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", QString("Could not open file: %1").arg(fileName));
        return;
    }
    
    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    
    appendOutput(QString("Loading file: %1").arg(QFileInfo(fileName).fileName()), "color: #2196F3; font-weight: bold;");
    
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty() && !trimmed.startsWith("#") && !trimmed.startsWith("//")) {
            appendOutput(QString("> %1").arg(trimmed), "color: #81C784; font-weight: bold;");
            
            std::string result = m_interpreter->execute(qtToStd(trimmed));
            if (!m_interpreter->getLastError().empty()) {
                showError(stdToQt(m_interpreter->getLastError()));
            } else if (!result.empty()) {
                appendOutput(stdToQt(result), "color: #E0E0E0;");
            }
        }
    }
    
    updateVariableList();
    appendOutput("File execution completed.", "color: #2196F3; font-style: italic;");
}

void InterpreterWidget::saveSession()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Session",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Text Files (*.txt);;All Files (*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", QString("Could not save file: %1").arg(fileName));
        return;
    }
    
    QTextStream out(&file);
    out << m_outputDisplay->toPlainText();
    
    m_statusLabel->setText("Session Saved");
    m_statusLabel->setStyleSheet("color: #4CAF50; font-weight: bold; padding: 4px;");
}

void InterpreterWidget::showHelp()
{
    QMessageBox::information(this, "GlossAI Help",
        "GlossAI Mathematical Expression Interpreter\n\n"
        "Basic Operations:\n"
        "• +, -, *, / : Basic arithmetic\n"
        "• ** : Exponentiation (2**3 = 8)\n"
        "• () : Grouping expressions\n\n"
        "Mathematical Functions:\n"
        "• sqrt(x) : Square root\n"
        "• pow(x, y) : x raised to power y\n"
        "• abs(x) : Absolute value\n"
        "• sin(x), cos(x), tan(x) : Trigonometric\n"
        "• log(x) : Natural logarithm\n"
        "• exp(x) : e raised to power x\n"
        "• floor(x), ceil(x), round(x)\n"
        "• min(a,b,...), max(a,b,...)\n\n"
        "Variables:\n"
        "• x = 5 : Assign value to variable\n"
        "• Use variables in expressions\n\n"
        "Examples:\n"
        "• 2 + 3 * 4\n"
        "• sqrt(16) + pow(2, 3)\n"
        "• x = 10; y = x * 2\n"
        "• sin(3.14159/2)"
    );
}

void InterpreterWidget::onInputReturnPressed()
{
    executeCommand();
}

void InterpreterWidget::onHistoryItemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        m_inputLine->setText(item->text());
        m_inputLine->setFocus();
    }
}

void InterpreterWidget::updateVariableList()
{
    m_variableList->clear();
    
    auto identifiers = m_interpreter->getAvailableIdentifiers();
    for (const std::string &id : identifiers) {
        m_variableList->addItem(stdToQt(id));
    }
}

void InterpreterWidget::addToHistory(const QString &command)
{
    if (m_commandHistory.isEmpty() || m_commandHistory.last() != command) {
        m_commandHistory.append(command);
        m_historyList->addItem(command);
        
        // Limit history size
        while (m_commandHistory.size() > MAX_HISTORY_SIZE) {
            m_commandHistory.removeFirst();
            delete m_historyList->takeItem(0);
        }
        
        // Scroll to bottom
        m_historyList->scrollToBottom();
    }
}

void InterpreterWidget::appendOutput(const QString &text, const QString &style)
{
    QString styledText = text;
    if (!style.isEmpty()) {
        styledText = QString("<span style=\"%1\">%2</span>").arg(style, text.toHtmlEscaped());
        m_outputDisplay->insertHtml(styledText + "<br>");
    } else {
        m_outputDisplay->insertPlainText(text + "\n");
    }
    
    // Scroll to bottom
    QTextCursor cursor = m_outputDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_outputDisplay->setTextCursor(cursor);
    m_outputDisplay->ensureCursorVisible();
}

void InterpreterWidget::showError(const QString &error)
{
    appendOutput(QString("Error: %1").arg(error), "color: #f44336; font-weight: bold;");
}

void InterpreterWidget::loadHistory()
{
    QString historyPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(historyPath);
    
    QFile file(historyPath + "/glossai_history.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                m_commandHistory.append(line);
                m_historyList->addItem(line);
            }
        }
    }
}

void InterpreterWidget::saveHistory()
{
    QString historyPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(historyPath);
    
    QFile file(historyPath + "/glossai_history.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &command : m_commandHistory) {
            out << command << "\n";
        }
    }
}

void InterpreterWidget::showWelcomeMessage()
{
    appendOutput("GlossAI Mathematical Expression Interpreter v1.0", "font-weight: bold; color: #2196F3; font-size: 14px;");
    appendOutput("Pure C++ Core with Qt Interface", "color: #81C784; font-style: italic;");
    appendOutput("", "");
    appendOutput("Enter mathematical expressions and press Enter to evaluate.", "color: #B0BEC5;");
    appendOutput("Use variables like: x = 5, then: x * 2", "color: #B0BEC5;");
    appendOutput("Available functions: sqrt, pow, abs, sin, cos, tan, log, exp, floor, ceil, round, min, max", "color: #B0BEC5;");
    appendOutput("Click 'Help' for more information.", "color: #B0BEC5;");
    appendOutput("", "");
}

QString InterpreterWidget::stdToQt(const std::string &str) const
{
    return QString::fromStdString(str);
}

std::string InterpreterWidget::qtToStd(const QString &str) const
{
    return str.toStdString();
}
