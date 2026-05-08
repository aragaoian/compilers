#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "Token.h"
#include "SemanticError.h"
#include "../symbols_table/SymbolsTableManager.h"
#include <stack>
#include <string>

class Semantico
{
public:
    void executeAction(int action, const Token *token);
private:
    SymbolsTableManager stManager();
    std::stack<std::string> stack;
};

#endif
