#include "MainWindow.h"

#include <QFont>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QKeySequence>
#include <QMenuBar>
#include <QPalette>
#include <QSplitter>
#include <QTextCharFormat>
#include <QTextCursor>
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
    setWindowTitle("I(an)DE - Compilador Françês");
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

    symbolsTable->setColumnCount(10);
    symbolsTable->setHorizontalHeaderLabels({
        "Symbol",
        "Type",
        "Function",
        "Data Type",
        "Array Size",
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

    const QString message = QString::fromStdString(result.message);
    const QStringList lines = message.split('\n');

    QTextCursor cursor = output->textCursor();
    cursor.movePosition(QTextCursor::End);

    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(output->palette().color(QPalette::Text));

    QTextCharFormat errorFormat = defaultFormat;
    errorFormat.setForeground(Qt::red);

    QTextCharFormat warningFormat = defaultFormat;
    warningFormat.setForeground(Qt::darkYellow);

    for (int i = 0; i < lines.size(); ++i)
    {
        const QString &line = lines.at(i);
        QTextCharFormat *format = &defaultFormat;

        if (line.startsWith("[ERRO"))
        {
            format = &errorFormat;
        }
        else if (line.startsWith("[AVISO]"))
        {
            format = &warningFormat;
        }

        cursor.setCharFormat(*format);
        cursor.insertText(line);
        if (i + 1 < lines.size())
        {
            cursor.insertText("\n");
        }
    }
    output->setTextCursor(cursor);

    updateSymbolsTable(result.symbols);
}

void MainWindow::updateSymbolsTable(const std::vector<SymbolRow> &symbols)
{
    symbolsTable->setRowCount(static_cast<int>(symbols.size()));

    for (int row = 0; row < static_cast<int>(symbols.size()); ++row)
    {
        const SymbolRow &symbol = symbols[static_cast<std::size_t>(row)];
        const QString arrSizeText = symbol.arrSize > 0 ? QString::number(symbol.arrSize) : "";
        const QString ownerText = symbol.ownerFunction.empty() ? "" : QString::fromStdString(symbol.ownerFunction);

        QString valueText;
        if(symbol.value.empty() && symbol.varType == VariableTypes::FUNCTION){
            valueText = "";
        }else{
            valueText = symbol.value.empty() ? "mem garbage" : QString::fromStdString(symbol.value);
        }

        symbolsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(symbol.symbol)));
        symbolsTable->setItem(row, 1, new QTableWidgetItem(variableTypeToString(symbol.varType)));
        symbolsTable->setItem(row, 2, new QTableWidgetItem(ownerText));
        symbolsTable->setItem(row, 3, new QTableWidgetItem(dataTypeToString(symbol.dataType)));
        symbolsTable->setItem(row, 4, new QTableWidgetItem(arrSizeText));
        symbolsTable->setItem(row, 5, new QTableWidgetItem(valueText));
        symbolsTable->setItem(row, 6, new QTableWidgetItem(symbol.isUsed ? "true" : "false"));
        symbolsTable->setItem(row, 7, new QTableWidgetItem(symbol.isInitialized ? "true" : "false"));
        symbolsTable->setItem(row, 8, new QTableWidgetItem(QString::number(symbol.sequence)));
        symbolsTable->setItem(row, 9, new QTableWidgetItem(symbol.scope == 0 ? "global" : QString::number(symbol.scope)));
    }
}
