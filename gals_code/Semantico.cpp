#include "Semantico.h"
#include "Constants.h"

#include <algorithm>
#include <iostream>

#define GUARDA_ID 1
#define GUARDA_TIPO_DADO 2
#define GUARDA_LITERAL 3
#define TIPO_VARIAVEL 4
#define TIPO_VETOR 5
#define TIPO_PARAMETRO 6
#define TIPO_ARGUMENTO 7
#define TIPO_FUNCAO 8
#define ENTRA_ESCOPO 9
#define SAI_ESCOPO 10
#define INICIA_ATRIBUICAO 11
#define USO_ID 12
#define MARCA_INICIALIZACAO 13
#define DECLARA 14
#define TAMANHO_VETOR 15
#define ENTRA_ESCOPO_PARAMETROS 16
#define SAI_ESCOPO_PARAMETROS 17
#define CONTA_ID_PENDENTE 18
#define CONTA_INIT_PENDENTE 19
#define INICIA_CONTEXTO_INIT 20
#define FINALIZA_CONTEXTO_INIT 21

static std::string varTypeEnumToString(VariableTypes varType){
    switch (varType)
    {
    case VariableTypes::SCALAR:
        return "Variável";
    case VariableTypes::ARRAY:
        return "Variável";
    case VariableTypes::FUNCTION:
        return "Função";
    case VariableTypes::PARAMETER:
        return "Parâmetro";
    case VariableTypes::ARGUMENT:
        return "Argumento";
    default:
        return "Desconhecido";
    }
}

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

enum class CompatibilityResult { Ok, Warn, Error };

static CompatibilityResult checkAssignmentCompatibility(DataTypes target, DataTypes source)
{
    if (target == source) {
        return CompatibilityResult::Ok;
    }

    switch (target)
    {
    case DataTypes::INT:
        return source == DataTypes::FLOAT ? CompatibilityResult::Warn : CompatibilityResult::Error;
    case DataTypes::FLOAT:
        return source == DataTypes::INT ? CompatibilityResult::Ok : CompatibilityResult::Error;
    case DataTypes::CHAR:
        if (source == DataTypes::INT) {
            return CompatibilityResult::Warn;
        }
        return source == DataTypes::STRING ? CompatibilityResult::Ok : CompatibilityResult::Error;
    case DataTypes::STRING:
        if (source == DataTypes::INT || source == DataTypes::FLOAT || source == DataTypes::CHAR || source == DataTypes::STRING) {
            return CompatibilityResult::Ok;
        }
        return CompatibilityResult::Error;
    case DataTypes::BOOLEAN:
        return source == DataTypes::BOOLEAN ? CompatibilityResult::Ok : CompatibilityResult::Error;
    case DataTypes::VOID:
    default:
        return CompatibilityResult::Error;
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
    while (!declLiterals.empty()) {
        declLiterals.pop();
    }
    while(!arrSizes.empty()){
        arrSizes.pop();
    }
    pendingArraySize = 1;
    hasPendingArraySize = false;

    isAttribution = false;
    isInitialized = false;
    isUsed = false;
    inInitContext = false;
    pendingIdsCount = 0;
    pendingInitCount = 0;
    skipNextBlockScope = false;
    skipNextBlockExit = false;
    currFuncParamatersCounter = 0;

    pendingIds.clear();
    pendingArgs.clear();

    currentFunctionName.clear();
    currentReturnType = DataTypes::VOID;

    messages.clear();
}

std::vector<std::string> Semantico::getMessages() const
{
    return messages;
}

void Semantico::logInfo(const std::string &message, const Token *token)
{
    std::string formatted = "[INFO] " + message;
    if (token != nullptr) {
        formatted += " (pos " + std::to_string(token->getPosition()) + ")";
    }
    messages.push_back(formatted);
}

void Semantico::logWarning(const std::string &message, const Token *token)
{
    std::string formatted = "[AVISO] " + message;
    if (token != nullptr) {
        formatted += " (pos " + std::to_string(token->getPosition()) + ")";
    }
    messages.push_back(formatted);
}

void Semantico::executeAction(int action, const Token *token)
{
    std::cout << "Ação: " << action << ", Token: "  << token->getId() 
              << ", Lexema: " << token->getLexeme() << std::endl;

    switch (action)
    {
    case GUARDA_ID:
        ids.push(token->getLexeme());
        break;

    case GUARDA_TIPO_DADO:
        dataTypes.push(dataTypeFromToken(token->getId()));
        break;

    case GUARDA_LITERAL:
        if (inInitContext) {
            declLiterals.push({dataTypeFromToken(token->getId()), token->getLexeme()});
        } else {
            literals.push({dataTypeFromToken(token->getId()), token->getLexeme()});
        }
        break;

    case TIPO_VARIAVEL:
        variableTypes.push(VariableTypes::SCALAR);
        break;
    
    case TIPO_VETOR:
        variableTypes.push(VariableTypes::ARRAY);
        if (!hasPendingArraySize) {
            throw SemanticError("Tamanho do vetor ausente!", token->getPosition());
        }
        arrSizes.push(pendingArraySize);
        pendingArraySize = 1;
        hasPendingArraySize = false;
        break;

        
    case TIPO_PARAMETRO:
        variableTypes.push(VariableTypes::PARAMETER);
        currFuncParamatersCounter++;
        break;

    case TIPO_ARGUMENTO:
        variableTypes.push(VariableTypes::ARGUMENT);
        break;

    case TIPO_FUNCAO:
        variableTypes.push(VariableTypes::FUNCTION);
        if (!ids.empty()) {
            currentFunctionName = ids.top();
        }
        break;

    case ENTRA_ESCOPO:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
        if (skipNextBlockScope) {
            skipNextBlockScope = false;
            skipNextBlockExit = true;
            break;
        }
        currentScope = stManager.enterScope(currentScope);
        break;

    case SAI_ESCOPO:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
        if (skipNextBlockExit) {
            skipNextBlockExit = false;
            break;
        }
        currentScope = stManager.exitScope(currentScope);
        break;

    case INICIA_ATRIBUICAO:
        isAttribution = true;
        break;

    case USO_ID:{
        std::string id = ids.top();
        ids.pop();


        if(!stManager.validateVariableScope(id, currentScope)){
            std::string formattedError = "Variável " + id + " está fora do escopo!"; 
            throw SemanticError(formattedError, token->getPosition());
        }

        if(!stManager.useSymbol(id, currentScope)){
            throw SemanticError("Variável não pode ser utilizada!", token->getPosition());
        }

        MetaData mt = stManager.returnMetaData(id, currentScope);
        
        if(!mt.value.empty()){
            if (inInitContext) {
                declLiterals.push({mt.dataType, mt.value});
            } else {
                literals.push({mt.dataType, mt.value});
            }
        }

        break;
    }

    case MARCA_INICIALIZACAO:
    isInitialized = true;
        break;

    case DECLARA:{
        if (!variableTypes.empty() && variableTypes.top() == VariableTypes::PARAMETER) {
            for (int i = 0; i < currFuncParamatersCounter; i++) {
                if (variableTypes.empty() || variableTypes.top() != VariableTypes::PARAMETER) {
                    throw SemanticError("Parametro invalido.", token->getPosition());
                }
                variableTypes.pop();

                if (variableTypes.empty()) {
                    throw SemanticError("Parametro invalido.", token->getPosition());
                }
                VariableTypes paramKind = variableTypes.top();
                variableTypes.pop();

                if (dataTypes.empty()) {
                    throw SemanticError("Parametro invalido.", token->getPosition());
                }

                MetaData pmt;
                pmt.varType = VariableTypes::PARAMETER;
                pmt.dataType = dataTypes.top();
                dataTypes.pop();
                pmt.ownerFunction = currentFunctionName;
                pmt.sequence = i+1;

                if (paramKind == VariableTypes::ARRAY) {
                    if (arrSizes.empty()) {
                        throw SemanticError("Tamanho do vetor ausente!", token->getPosition());
                    }
                    pmt.arrSize = arrSizes.top();
                    arrSizes.pop();
                }

                if (isInitialized && !declLiterals.empty()) {
                    pmt.isInitialized = true;
                    if (paramKind == VariableTypes::ARRAY) {
                        int arrSize = pmt.arrSize;
                        if (static_cast<int>(declLiterals.size()) < arrSize) {
                            throw SemanticError("Inicialização do vetor incompleta!", token->getPosition());
                        }
                        if (static_cast<int>(declLiterals.size()) > arrSize) {
                            throw SemanticError("Inicialização do vetor excede o tamanho!", token->getPosition());
                        }

                        for (int j = 0; j < arrSize; j++) {
                            std::pair<DataTypes, std::string> currLiteral = declLiterals.top();
                            declLiterals.pop();
                            CompatibilityResult comp = checkAssignmentCompatibility(pmt.dataType, currLiteral.first);
                            if (comp == CompatibilityResult::Error) {
                                throw SemanticError("O tipo não é compatível.", token->getPosition());
                            }
                            if (comp == CompatibilityResult::Warn) {
                                logWarning("Compatibilidade de tipos com aviso.", token);
                            }
                            pmt.value += currLiteral.second + (arrSize - 1 != j ? ";" : "");
                        }
                    } else {
                        if (declLiterals.size() == 1) {
                            std::pair<DataTypes, std::string> currLiteral = declLiterals.top();
                            declLiterals.pop();
                            CompatibilityResult comp = checkAssignmentCompatibility(pmt.dataType, currLiteral.first);
                            if (comp == CompatibilityResult::Error) {
                                throw SemanticError("O tipo não é compatível.", token->getPosition());
                            }
                            if (comp == CompatibilityResult::Warn) {
                                logWarning("Compatibilidade de tipos com aviso.", token);
                            }
                            pmt.value = currLiteral.second;
                        } else {
                            while (!declLiterals.empty()) {
                                declLiterals.pop();
                            }
                        }
                    }
                }

                if (ids.empty()) {
                    throw SemanticError("Parametro invalido.", token->getPosition());
                }
                std::string paramId = ids.top();
                ids.pop();

                if (!stManager.insertSymbol(paramId, pmt, currentScope)) {
                    throw SemanticError("Parametro ja declarado no escopo!", token->getPosition());
                }
            }

            currFuncParamatersCounter = 0;
            isInitialized = false;
        }

        MetaData mt;
        mt.varType = variableTypes.top(); 
        variableTypes.pop();
        mt.dataType = dataTypes.top();
        dataTypes.pop();

        if (mt.varType == VariableTypes::ARRAY) {
            if (arrSizes.empty()) {
                throw SemanticError("Tamanho do vetor ausente!", token->getPosition());
            }
            mt.arrSize = arrSizes.top();
            arrSizes.pop();
        }

        if (mt.varType == VariableTypes::FUNCTION) {
            mt.isInitialized = isInitialized;
        }

        if (mt.varType != VariableTypes::FUNCTION) {
            int idCount = pendingIdsCount > 0 ? pendingIdsCount : 1;
            int initCount = isInitialized ? pendingInitCount : 0;

            std::vector<std::string> idsOrdered;
            idsOrdered.reserve(static_cast<std::size_t>(idCount));
            for (int i = 0; i < idCount; i++) {
                if (ids.empty()) {
                    throw SemanticError("Declaração inválida!", token->getPosition());
                }
                idsOrdered.push_back(ids.top());
                ids.pop();
            }
            std::reverse(idsOrdered.begin(), idsOrdered.end());

            if (mt.varType == VariableTypes::ARRAY) {
                std::vector<std::string> initGroups;
                int groups = std::min(initCount, idCount);
                initGroups.reserve(static_cast<std::size_t>(groups));
                for (int i = 0; i < groups; i++) {
                    if (static_cast<int>(declLiterals.size()) < mt.arrSize) {
                        throw SemanticError("Inicialização do vetor incompleta!", token->getPosition());
                    }

                    if(initCount > idCount){
                        throw SemanticError("Quantidade de inicializadores de vetor maior que quantidade de IDs", token->getPosition());
                    }

                    std::vector<std::string> groupValues;
                    groupValues.reserve(static_cast<std::size_t>(mt.arrSize));
                    for (int j = 0; j < mt.arrSize; j++) {
                        std::pair<DataTypes, std::string> currLiteral = declLiterals.top();
                        declLiterals.pop();
                        CompatibilityResult comp = checkAssignmentCompatibility(mt.dataType, currLiteral.first);
                        if (comp == CompatibilityResult::Error) {
                            throw SemanticError("O tipo não é compatível.", token->getPosition());
                        }
                        if (comp == CompatibilityResult::Warn) {
                            logWarning("Compatibilidade de tipos com aviso.", token);
                        }
                        groupValues.push_back(currLiteral.second);
                    }
                    std::reverse(groupValues.begin(), groupValues.end());

                    std::string groupValue;
                    for (std::size_t j = 0; j < groupValues.size(); j++) {
                        groupValue += groupValues[j];
                        if (j + 1 < groupValues.size()) {
                            groupValue += ";";
                        }
                    }
                    initGroups.push_back(groupValue);
                }
                if (!declLiterals.empty()) {
                    throw SemanticError("Inicialização do vetor excede o tamanho!", token->getPosition());
                }
                std::reverse(initGroups.begin(), initGroups.end());

                for (int i = 0; i < idCount; i++) {
                    MetaData vmt = mt;
                    if (isInitialized && i < initCount) {
                        vmt.isInitialized = true;
                        if (static_cast<std::size_t>(i) < initGroups.size()) {
                            vmt.value = initGroups[static_cast<std::size_t>(i)];
                        }
                    }

                    if(!stManager.insertSymbol(idsOrdered[static_cast<std::size_t>(i)], vmt, currentScope)) {
                        throw SemanticError("Variável já declarada no escopo!", token->getPosition());
                    }
                }
            } else {
                std::vector<std::pair<DataTypes, std::string>> initValues;
                int values = std::min(initCount, idCount);
                initValues.reserve(static_cast<std::size_t>(values));
                for (int i = 0; i < values && !declLiterals.empty(); i++) {
                    initValues.push_back(declLiterals.top());
                    declLiterals.pop();
                }
                std::reverse(initValues.begin(), initValues.end());

                for (int i = 0; i < idCount; i++) {
                    MetaData vmt = mt;
                    if(initCount > idCount){
                        throw SemanticError("Quantidade de inicializadores maior que quantidade de IDs", token->getPosition());
                    }

                    if (isInitialized && i < initCount) {
                        vmt.isInitialized = true;
                        if (static_cast<std::size_t>(i) < initValues.size()) {
                            const auto &currLiteral = initValues[static_cast<std::size_t>(i)];
                            CompatibilityResult comp = checkAssignmentCompatibility(vmt.dataType, currLiteral.first);
                            if (comp == CompatibilityResult::Error) {
                                throw SemanticError("O tipo não é compatível.", token->getPosition());
                            }
                            if (comp == CompatibilityResult::Warn) {
                                logWarning("Compatibilidade de tipos com aviso.", token);
                            }
                            vmt.value = currLiteral.second;
                        }
                    }

                    if(!stManager.insertSymbol(idsOrdered[static_cast<std::size_t>(i)], vmt, currentScope)) {
                        throw SemanticError("Variável já declarada no escopo!", token->getPosition());
                    }
                }

                while (!declLiterals.empty()) {
                    declLiterals.pop();
                }
            }

            pendingIdsCount = 0;
            pendingInitCount = 0;
            isInitialized = false;
            break;
        }

        if(isInitialized && mt.varType != VariableTypes::FUNCTION){
            mt.isInitialized = true;
            if(mt.varType == VariableTypes::ARRAY){
                int arrSize = mt.arrSize;
                if (static_cast<int>(literals.size()) < arrSize) {
                    throw SemanticError("Inicialização do vetor incompleta!", token->getPosition());
                }
                
                for(int i = 0; i < arrSize; i++){
                    std::pair<DataTypes, std::string> currLiteral = literals.top();
                    literals.pop();
                    CompatibilityResult comp = checkAssignmentCompatibility(mt.dataType, currLiteral.first);
                    if (comp == CompatibilityResult::Error) {
                        throw SemanticError("O tipo não é compatível.", token->getPosition());
                    }
                    if (comp == CompatibilityResult::Warn) {
                        logWarning("Compatibilidade de tipos com aviso.", token);
                    }
                    mt.value += currLiteral.second + (arrSize-1 != i ? ";" : "");
                }

            }else if(mt.varType == VariableTypes::SCALAR){
                if (literals.size() == 1) {
                    std::pair<DataTypes, std::string> currLiteral = literals.top();
                    literals.pop();
                    CompatibilityResult comp = checkAssignmentCompatibility(mt.dataType, currLiteral.first);
                    if (comp == CompatibilityResult::Error) {
                        throw SemanticError("O tipo não é compatível.", token->getPosition());
                    }
                    if (comp == CompatibilityResult::Warn) {
                        logWarning("Compatibilidade de tipos com aviso.", token);
                    }
                    mt.value = currLiteral.second;
                } else {
                    while (!literals.empty()) {
                        literals.pop();
                    }
                }
            }else if(mt.varType == VariableTypes::PARAMETER){
                for(int i = 0; i < currFuncParamatersCounter; i++){
                    
                }
            }
        }

        // NOTE
        // Após a inicialização
        // marcar como false para resetar.
        isInitialized = false;

        std::string currId = ids.top();
        ids.pop();

        Scope *targetScope = currentScope;
        if (mt.varType == VariableTypes::FUNCTION && currentScope != nullptr) {
            targetScope = currentScope->previous_scope;
        }

        if(!stManager.insertSymbol(currId, mt, targetScope)) {
            if (mt.varType == VariableTypes::FUNCTION) {
                MetaData existing = stManager.returnMetaData(currId, targetScope);
                if (existing.varType == VariableTypes::FUNCTION && !existing.isInitialized && mt.isInitialized) {
                    existing.isInitialized = true;
                    if (!stManager.updateSymbol(currId, existing, targetScope)) {
                        throw SemanticError("Erro ao atualizar a funcao.", token->getPosition());
                    }
                } else {
                    throw SemanticError("Funcao ja declarada no escopo!", token->getPosition());
                }
            } else {
                throw SemanticError("Variável já declarada no escopo!", token->getPosition());
            }
        }

        if (mt.varType == VariableTypes::FUNCTION) {
            currentFunctionName.clear();
        }
        break;
    }

    case TAMANHO_VETOR: {
        if (literals.empty()) {
            throw SemanticError("Tamanho do vetor invalido!", token->getPosition());
        }

        std::pair<DataTypes, std::string> sizeLiteral = literals.top();
        literals.pop();

        if (sizeLiteral.first != DataTypes::INT) {
            throw SemanticError("Tamanho do vetor deve ser inteiro!", token->getPosition());
        }

        int size = 0;
        try {
            size = std::stoi(sizeLiteral.second);
        } catch (const std::exception &) {
            throw SemanticError("Tamanho do vetor invalido!", token->getPosition());
        }

        if (size <= 0) {
            throw SemanticError("Tamanho do vetor deve ser positivo!", token->getPosition());
        }

        pendingArraySize *= size;
        hasPendingArraySize = true;
        break;
    }

    // NOTE 
    // Para ambos os casos:
    // A Função deve estar declarada no escopo GLOBAL (0)
    // mas seus parâmetros devem estar no devido escopo
    // -> então exemplos como:
    //      fn fizz->bool(a: ent, b: ent);
    //      fn buzz->bool(a: ent, b: ent);
    // devem estar na árvore como GLOBAL (0)
    //                   fizz(1) <-|  |-> buzz (2)
    case ENTRA_ESCOPO_PARAMETROS:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
        currentScope = stManager.enterScope(currentScope);
        skipNextBlockScope = true;
        skipNextBlockExit = false;
        break;

    case SAI_ESCOPO_PARAMETROS:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
        currentScope = stManager.exitScope(currentScope);
        skipNextBlockScope = false;
        skipNextBlockExit = false;
        break;

    case CONTA_ID_PENDENTE:
        pendingIdsCount++;
        break;

    case CONTA_INIT_PENDENTE:
        pendingInitCount++;
        break;

    case INICIA_CONTEXTO_INIT:
        inInitContext = true;
        break;

    case FINALIZA_CONTEXTO_INIT:
        inInitContext = false;
        break;

    default:
        break;
    }
}

std::vector<SymbolRow> Semantico::getSymbolRows() const{
    return stManager.collectSymbolsPreorder();
}

void Semantico::logDeclaredButNotUsed() {
    std::vector<SymbolRow> symbols = stManager.collectSymbolsPreorder();
    for(SymbolRow symbol: symbols){
        if(symbol.isUsed) continue;
        std::string formatted = "[AVISO] " + varTypeEnumToString(symbol.varType) + " " + symbol.symbol + " está declarada mas não está sendo utilizada.";
        messages.push_back(formatted);
    }
}

void Semantico::logNonInitializedVariables() {
    std::vector<SymbolRow> symbols = stManager.collectSymbolsPreorder();
    for(SymbolRow symbol: symbols){
        if(symbol.isInitialized) continue;
        if (symbol.varType != VariableTypes::SCALAR && symbol.varType != VariableTypes::ARRAY) continue;
        std::string formatted = "[AVISO] " + varTypeEnumToString(symbol.varType) + " " + symbol.symbol + " não foi inicializado(a), usando lixo de memória.";
        messages.push_back(formatted);
    }
}

