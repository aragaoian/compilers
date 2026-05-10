#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "Token.h"
#include "SemanticError.h"
#include "../symbols_table/SymbolsTableManager.h"
#include <stack>
#include <string>
#include <utility>
#include <vector>

class Semantico
{
public:
    Semantico();
    void executeAction(int action, const Token *token);
    std::vector<SymbolRow> getSymbolRows() const;
    void logDeclaredButNotUsed();
    std::vector<std::string> getMessages() const;
    void reset();
private:
    void logInfo(const std::string &message, const Token *token);
    void logWarning(const std::string &message, const Token *token);

    SymbolsTableManager stManager;
    Scope *currentScope = nullptr;

    std::stack<std::string> ids;
    std::stack<VariableTypes> variableTypes;
    std::stack<DataTypes> dataTypes;
    std::stack<std::pair<DataTypes, std::string>> literals;
    std::stack<int> arrSizes;
    int pendingArraySize = 1;
    bool hasPendingArraySize = false;

    bool isAttribution = false;
    bool isInitialized = false;
    bool isUsed = false;

    bool skipNextBlockScope = false;
    bool skipNextBlockExit = false;

    int currFuncParamatersCounter = 0;
    int pendingIdsCount = 0;
    int pendingInitCount = 0;

    std::vector<std::string> messages;

    std::vector<std::string> pendingIds;
    std::vector<std::pair<DataTypes, std::string>> pendingArgs;
    int sequenceCounter = 0;

    std::string currentFunctionName;
    DataTypes currentReturnType = DataTypes::VOID;
};

#endif
