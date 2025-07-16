#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <memory>

// Forward declaration of the pure C++ interpreter
class Interpreter;

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLineEdit;
class QListWidget;
class QSplitter;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

/**
 * @brief Main widget for the GlossAI interpreter interface
 * 
 * Qt-based UI that interfaces with the pure C++ interpreter core.
 * Provides a console-like interface for mathematical expression evaluation.
 */
class InterpreterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InterpreterWidget(QWidget *parent = nullptr);
    ~InterpreterWidget();

public slots:
    /**
     * @brief Execute the current input
     */
    void executeCommand();
    
    /**
     * @brief Clear the output display
     */
    void clearOutput();
    
    /**
     * @brief Clear the interpreter context
     */
    void clearContext();
    
    /**
     * @brief Load code from a file
     */
    void loadFile();
    
    /**
     * @brief Save current session to a file
     */
    void saveSession();
    
    /**
     * @brief Show help information
     */
    void showHelp();

private slots:
    void onInputReturnPressed();
    void onHistoryItemDoubleClicked(QListWidgetItem *item);
    void updateVariableList();

private:
    void setupUI();
    void setupConnections();
    void addToHistory(const QString &command);
    void appendOutput(const QString &text, const QString &style = QString());
    void showError(const QString &error);
    void loadHistory();
    void saveHistory();
    void showWelcomeMessage();

    // UI components
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    
    // Left panel - Main console
    QWidget *m_consoleWidget;
    QTextEdit *m_outputDisplay;
    QLineEdit *m_inputLine;
    QPushButton *m_executeButton;
    QPushButton *m_clearButton;
    QPushButton *m_loadButton;
    QPushButton *m_saveButton;
    QPushButton *m_helpButton;
    QLabel *m_statusLabel;
    
    // Right panel - History and variables
    QWidget *m_sidePanel;
    QListWidget *m_historyList;
    QListWidget *m_variableList;
    QPushButton *m_clearContextButton;
    
    // Pure C++ interpreter engine (no Qt dependencies)
    std::unique_ptr<Interpreter> m_interpreter;
    
    // State
    QStringList m_commandHistory;
    int m_historyIndex;
    static const int MAX_HISTORY_SIZE = 100;
    
    // Helper methods for Qt/C++ string conversion
    QString stdToQt(const std::string &str) const;
    std::string qtToStd(const QString &str) const;
};
