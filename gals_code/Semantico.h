#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "Token.h"
#include "SemanticError.h"
#include "../symbols_table/SymbolsTableManager.h"
#include <stack>
#include <string>
#include <utility>
#include <vector>

enum class Operators{
    SUM,
    SUB,
    MUL,
    DIV,
    OR,
    AND,
    NOT
};

class Semantico
{
public:
    Semantico();
    void executeAction(int action, const Token *token);
    std::vector<SymbolRow> getSymbolRows() const;
    void logDeclaredButNotUsed();
    void logNonInitializedVariables();
    std::vector<std::string> getMessages() const;
    void reset();
private:
    void logInfo(const std::string &message, const Token *token);
    void logWarning(const std::string &message, const Token *token);
    std::vector<std::string> messages;

    SymbolsTableManager stManager;
    Scope *currentScope = nullptr;

    std::stack<std::string> ids;
    std::stack<VariableTypes> variableTypes;
    std::stack<DataTypes> dataTypes;
    std::stack<std::pair<DataTypes, std::string>> literals;
    std::stack<std::pair<DataTypes, std::string>> declLiterals;
    std::stack<int> arrSizes;
    int pendingArraySize = 1;
    bool hasPendingArraySize = false;

    bool isAttribution = false;
    bool isInitialized = false;
    bool isUsed = false;
    bool inInitContext = false;

    bool skipCodeBlock = false;

    int pendingIdsCount = 0;
    int pendingInitCount = 0;
    std::vector<std::string> pendingIds;

    int currFuncParamatersCounter = 0;
    std::string currentFunctionName;
    DataTypes currentReturnType = DataTypes::VOID;
    std::vector<std::pair<DataTypes, std::string>> pendingArgs;

    std::stack<Operators> operators;

};

#endif
