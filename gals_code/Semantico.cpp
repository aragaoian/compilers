#include "Semantico.h"
#include "Constants.h"

#include <iostream>

static DataTypes dataTypeFromToken(TokenId tokenId)
{
    switch (tokenId)
    {
    case t_INT_KEY:
    case t_INT_VALUE_DECIMAL:
    case t_INT_VALUE_BINARY:
    case t_INT_VALUE_HEXADECIMAL:
        return DataTypes::INT;
    case t_FLOAT_KEY:
    case t_FLOAT_VALUE:
        return DataTypes::FLOAT;
    case t_BOOLEAN_KEY:
    case t_TRUE:
    case t_FALSE:
        return DataTypes::BOOLEAN;
    case t_CHAR_KEY:
    case t_CHAR_VALUE:
        return DataTypes::CHAR;
    case t_STRING_KEY:
    case t_STRING_VALUE:
        return DataTypes::STRING;
    case t_VOID:
        return DataTypes::VOID;
    default:
        return DataTypes::VOID;
    }
}

Semantico::Semantico()
{
    reset();
}

void Semantico::reset()
{
    stManager.reset();
    currentScope = stManager.getRootScope();

    while (!ids.empty()) {
        ids.pop();
    }
    while (!variableTypes.empty()) {
        variableTypes.pop();
    }
    while (!dataTypes.empty()) {
        dataTypes.pop();
    }
    while (!literals.empty()) {
        literals.pop();
    }

    isAttribution = false;
    isInitialized = false;
    isUsed = false;

    pendingIds.clear();
    pendingArgs.clear();
    sequenceCounter = 0;

    currentFunctionName.clear();
    currentReturnType = DataTypes::VOID;
}

void Semantico::executeAction(int action, const Token *token)
{
    std::cout << "Ação: " << action << ", Token: "  << token->getId() 
              << ", Lexema: " << token->getLexeme() << std::endl;

    switch (action)
    {
    case 1:
        ids.push(token->getLexeme());
        break;

    case 2:
        dataTypes.push(dataTypeFromToken(token->getId()));
        break;

    case 3:
        literals.push({dataTypeFromToken(token->getId()), token->getLexeme()});
        break;

    case 4:
        variableTypes.push(VariableTypes::SCALAR);
        break;
    
    case 5:
        variableTypes.push(VariableTypes::ARRAY);
        break;

    case 6:
        variableTypes.push(VariableTypes::PARAMETER);
        break;

    case 7:
        variableTypes.push(VariableTypes::ARGUMENT);
        break;

    case 8:
        variableTypes.push(VariableTypes::FUNCTION);
        break;

    case 9:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
        currentScope = stManager.enterScope(currentScope);
        break;

    case 10:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
        currentScope = stManager.exitScope(currentScope);
        break;

    case 11:
        isAttribution = true;
        break;

    case 12:
        isUsed = true;
        break;

    case 13:
        isInitialized = true;
        break;

    case 14:{
        MetaData mt;
        mt.varType = variableTypes.top(); 
        variableTypes.pop();
        mt.dataType = dataTypes.top(); 
        dataTypes.pop();

        if(isInitialized){
            mt.isInitialized = true;
            // TODO
            // review this
            // I believe assuming the value as a literal is wrong.
            // But i could be true after resolving an expression for exemple.
            std::pair<DataTypes, std::string> currLiteral = literals.top();
            literals.pop();
            if(currLiteral.first == mt.dataType){
                mt.value = currLiteral.second;
            }
        }

        std::string currId = ids.top();
        ids.pop();

        if(!stManager.insertSymbol(currId, mt, currentScope)) {
            throw ("Erro ao declarar a variável!");
        }
        break;
    }

    default:
        break;
    }
}

std::vector<SymbolRow> Semantico::getSymbolRows() const
{
    return stManager.collectSymbolsPreorder();
}

