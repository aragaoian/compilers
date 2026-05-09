#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "CompilerService.h"

#include <QAction>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QToolBar>
#include <QWidget>

#include <vector>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void compileSourceCode();
    void setupActions();
    void setupLayout();
    void updateSymbolsTable(const std::vector<SymbolRow> &symbols);

    QPlainTextEdit *editor;
    QPlainTextEdit *output;
    QTableWidget *symbolsTable;
    QWidget *symbolsPanel;
    QAction *compileAction;
    QAction *toggleSymbolsAction;
    QToolBar *toolbar;
    CompilerService compilerService;
};

#endif
