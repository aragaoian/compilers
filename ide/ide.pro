QT += widgets

CONFIG += c++17

INCLUDEPATH += $$PWD $$PWD/../gals_code $$PWD/../symbols_table

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    CompilerService.cpp \
    ../gals_code/Constants.cpp \
    ../gals_code/Lexico.cpp \
    ../gals_code/Sintatico.cpp \
    ../gals_code/Semantico.cpp \
    ../symbols_table/SymbolsTableManager.cpp

HEADERS += \
    MainWindow.h \
    CompilerService.h \
    ../gals_code/AnalysisError.h \
    ../gals_code/Constants.h \
    ../gals_code/LexicalError.h \
    ../gals_code/Lexico.h \
    ../gals_code/SemanticError.h \
    ../gals_code/Semantico.h \
    ../gals_code/SyntacticError.h \
    ../gals_code/Sintatico.h \
    ../gals_code/Token.h \
    ../symbols_table/SymbolsTableManager.h

TARGET = ide
TEMPLATE = app
