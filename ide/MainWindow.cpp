#include "MainWindow.h"

#include <QFont>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QKeySequence>
#include <QMenuBar>
#include <QSplitter>
#include <QTableWidgetItem>
#include <QVBoxLayout>

static QString variableTypeToString(VariableTypes type)
{
    switch (type)
    {
    case VariableTypes::SCALAR:
        return "Scalar";
    case VariableTypes::ARRAY:
        return "Array";
    case VariableTypes::FUNCTION:
        return "Function";
    case VariableTypes::PARAMETER:
        return "Parameter";
    case VariableTypes::ARGUMENT:
        return "Argument";
    default:
        return "Unknown";
    }
}

static QString dataTypeToString(DataTypes type)
{
    switch (type)
    {
    case DataTypes::INT:
        return "Int";
    case DataTypes::FLOAT:
        return "Float";
    case DataTypes::BOOLEAN:
        return "Boolean";
    case DataTypes::CHAR:
        return "Char";
    case DataTypes::STRING:
        return "String";
    case DataTypes::VOID:
        return "Void";
    default:
        return "Unknown";
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      editor(new QPlainTextEdit(this)),
      output(new QPlainTextEdit(this)),
      symbolsTable(new QTableWidget(this)),
      symbolsPanel(new QWidget(this)),
      compileAction(new QAction("Compilar", this)),
      toggleSymbolsAction(new QAction("Tabela de Símbolos", this)),
      toolbar(new QToolBar(this))
{
    setWindowTitle("IDE M1 Parte 2 - Compilador");
    resize(980, 640);

    setupActions();
    setupLayout();
}

void MainWindow::setupActions()
{
    compileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    connect(compileAction, &QAction::triggered, this, [this]() { compileSourceCode(); });

    toggleSymbolsAction->setCheckable(true);
    toggleSymbolsAction->setChecked(true);
    connect(toggleSymbolsAction, &QAction::toggled, this, [this](bool visible) {
        symbolsPanel->setVisible(visible);
    });

    QMenu *buildMenu = menuBar()->addMenu("Compilar");
    buildMenu->addAction(compileAction);

    QMenu *viewMenu = menuBar()->addMenu("Visualizar");
    viewMenu->addAction(toggleSymbolsAction);

    toolbar->addAction(compileAction);
    toolbar->addAction(toggleSymbolsAction);
    addToolBar(toolbar);
}

void MainWindow::setupLayout()
{
    QFont monoFont("Monospace");
    monoFont.setStyleHint(QFont::TypeWriter);
    monoFont.setPointSize(14);

    editor->setFont(monoFont);
    editor->setPlaceholderText("Digite o codigo fonte aqui...");

    output->setFont(monoFont);
    output->setReadOnly(true);
    output->setPlaceholderText("Mensagens de compilação...");

    symbolsTable->setColumnCount(8);
    symbolsTable->setHorizontalHeaderLabels({
        "Symbol",
        "Type",
        "Data Type",
        "Value",
        "Used",
        "Initialized",
        "Sequence",
        "Scope"
    });
    symbolsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    symbolsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    symbolsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QLabel *editorLabel = new QLabel("Código", this);
    QLabel *symbolsLabel = new QLabel("Tabela de Símbolos", this);
    QLabel *outputLabel = new QLabel("Saída", this);

    QWidget *editorPanel = new QWidget(this);
    QVBoxLayout *editorLayout = new QVBoxLayout(editorPanel);
    editorLayout->addWidget(editorLabel);
    editorLayout->addWidget(editor);

    QVBoxLayout *symbolsLayout = new QVBoxLayout(symbolsPanel);
    symbolsLayout->addWidget(symbolsLabel);
    symbolsLayout->addWidget(symbolsTable);

    QWidget *outputPanel = new QWidget(this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputPanel);
    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(output);

    QSplitter *topSplitter = new QSplitter(Qt::Horizontal, this);
    topSplitter->addWidget(editorPanel);
    topSplitter->addWidget(symbolsPanel);
    topSplitter->setStretchFactor(0, 3);
    topSplitter->setStretchFactor(1, 2);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(outputPanel);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);
    setCentralWidget(centralWidget);
}

void MainWindow::compileSourceCode()
{
    output->clear();

    const std::string sourceCode = editor->toPlainText().toStdString();
    const CompilationResult result = compilerService.compile(sourceCode);

    output->appendPlainText(QString::fromStdString(result.message));
    updateSymbolsTable(result.symbols);
}

void MainWindow::updateSymbolsTable(const std::vector<SymbolRow> &symbols)
{
    symbolsTable->setRowCount(static_cast<int>(symbols.size()));

    for (int row = 0; row < static_cast<int>(symbols.size()); ++row)
    {
        const SymbolRow &symbol = symbols[static_cast<std::size_t>(row)];
        const QString valueText = symbol.value.empty() ? "mem garbage" : QString::fromStdString(symbol.value);

        symbolsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(symbol.symbol)));
        symbolsTable->setItem(row, 1, new QTableWidgetItem(variableTypeToString(symbol.varType)));
        symbolsTable->setItem(row, 2, new QTableWidgetItem(dataTypeToString(symbol.dataType)));
        symbolsTable->setItem(row, 3, new QTableWidgetItem(valueText));
        symbolsTable->setItem(row, 4, new QTableWidgetItem(symbol.isUsed ? "true" : "false"));
        symbolsTable->setItem(row, 5, new QTableWidgetItem(symbol.isInitialized ? "true" : "false"));
        symbolsTable->setItem(row, 6, new QTableWidgetItem(QString::number(symbol.sequence)));
        symbolsTable->setItem(row, 7, new QTableWidgetItem(symbol.scope == 0 ? "global" : QString::number(symbol.scope)));
    }
}
