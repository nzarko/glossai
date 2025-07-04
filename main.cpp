#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include "ui/interpreterwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("GlossAI - Mathematical Expression Interpreter");
    window.resize(1000, 700);
    
    auto *centralWidget = new QWidget;
    auto *layout = new QVBoxLayout(centralWidget);
    
    auto *interpreterWidget = new InterpreterWidget;
    layout->addWidget(interpreterWidget);
    
    window.setCentralWidget(centralWidget);
    window.show();
    
    return app.exec();
}
