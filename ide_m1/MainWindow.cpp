#include "MainWindow.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      editor(new QPlainTextEdit(this)),
      output(new QPlainTextEdit(this)),
      compileButton(new QPushButton("Compilar", this))
{
    setWindowTitle("IDE M1 Parte 2 - Compilador");
    resize(980, 640);

    QFont monoFont("Monospace");
    monoFont.setStyleHint(QFont::TypeWriter);
    monoFont.setPointSize(14);

    editor->setFont(monoFont);
    editor->setPlaceholderText("Digite o codigo fonte aqui...");

    output->setFont(monoFont);
    output->setReadOnly(true);
    output->setPlaceholderText("Mensagens de compilação...");

    QLabel *editorLabel = new QLabel("Código", this);
    QLabel *outputLabel = new QLabel("Saída", this);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(editorLabel);
    leftLayout->addWidget(editor);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(outputLabel);
    rightLayout->addWidget(output);

    QHBoxLayout *panelsLayout = new QHBoxLayout();
    panelsLayout->addLayout(leftLayout, 3);
    panelsLayout->addLayout(rightLayout, 2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(panelsLayout);
    mainLayout->addWidget(compileButton);

    connect(compileButton, &QPushButton::clicked, this, [this]() { compileSourceCode(); });
}

void MainWindow::compileSourceCode()
{
    output->clear();

    const std::string sourceCode = editor->toPlainText().toStdString();
    const CompilationResult result = compilerService.compile(sourceCode);

    output->appendPlainText(QString::fromStdString(result.message));
}
