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
    void reset();
private:
    SymbolsTableManager stManager;
    Scope *currentScope = nullptr;

    std::stack<std::string> ids;
    std::stack<VariableTypes> variableTypes;
    std::stack<DataTypes> dataTypes;
    std::stack<std::pair<DataTypes, std::string>> literals;

    bool isAttribution = false;
    bool isInitialized = false;
    bool isUsed = false;

    std::vector<std::string> pendingIds;
    std::vector<std::pair<DataTypes, std::string>> pendingArgs;
    int sequenceCounter = 0;

    std::string currentFunctionName;
    DataTypes currentReturnType = DataTypes::VOID;
};

#endif
