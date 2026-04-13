#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "CompilerService.h"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>

class MainWindow : public QWidget
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void compileSourceCode();

    QPlainTextEdit *editor;
    QPlainTextEdit *output;
    QPushButton *compileButton;
    CompilerService compilerService;
};

#endif
