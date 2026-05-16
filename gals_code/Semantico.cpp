#include "Semantico.h"
#include "Constants.h"

#include <algorithm>
#include <iostream>

/*
TODO
1. Considerar separar a declaração 
(ter uma ação semântica para declaração de variável, outra para declaração de parâmetro, outra para função, etc)
3. Os valores não precisam estar na tabela de símbolos (isso não é pra ser resolvido em tempo de compilacão) [X]
4. Colocar a abertura de escopo e leitura de parametros no for
5. Criar os warning certos na atribuicao [X]
*/

#define GUARDA_ID 1
#define GUARDA_TIPO_DADO 2
#define GUARDA_LITERAL 3
#define TIPO_VARIAVEL 4
#define TIPO_VETOR 5
#define TIPO_PARAMETRO 6
#define TIPO_ARGUMENTO 7
#define TIPO_Função 8
#define ENTRA_ESCOPO 9
#define SAI_ESCOPO 10
#define ATRIBUICAO 11
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
#define AVALIA_EXPRESSAO 22
#define GUARDA_OPERADOR 23
#define AVALIA_RETORNO 24

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

static Operators operatorFromToken(TokenId tokenId){
    switch (tokenId)
    {
    case t_SUM:
        return Operators::SUM;
    case t_SUB:
        return Operators::SUB;
    case t_MUL:
        return Operators::MUL;
    case t_DIV:
        return Operators::DIV;
    case t_AND:
        return Operators::AND;
    case t_OR:
        return Operators::OR;
    case t_NOT:
        return Operators::NOT;
    default:
        return Operators::SUM;
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
    skipCodeBlock = false;
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

    case TIPO_Função:
        variableTypes.push(VariableTypes::FUNCTION);
        if (!ids.empty()) {
            currentFunctionName = ids.top();
        }
        break;

    case ENTRA_ESCOPO:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }

        if (skipCodeBlock) {
            skipCodeBlock = false;
            break;
        }

        currentScope = stManager.enterScope(currentScope);
        break;

    case SAI_ESCOPO:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }

        currentScope = stManager.exitScope(currentScope);
        break;

    case ATRIBUICAO:{
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }

        std::string id = ids.top();
        std::cout << id << std::endl;
        ids.pop();

        MetaData *mt = stManager.returnMetaData(id, currentScope);
        if(mt == nullptr){
            break;
        }
        mt->isInitialized = true;
        break;
    }

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

        MetaData *mt = stManager.returnMetaData(id, currentScope);
        if (mt != nullptr) {
            if (inInitContext) {
                declLiterals.push({mt->dataType, ""});
            } else {
                literals.push({mt->dataType, ""});
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
                    throw SemanticError("Parâmetro inválido.", token->getPosition());
                }
                variableTypes.pop();

                if (variableTypes.empty()) {
                    throw SemanticError("Parâmetro inválido.", token->getPosition());
                }
                VariableTypes paramKind = variableTypes.top();
                variableTypes.pop();

                if (dataTypes.empty()) {
                    throw SemanticError("Parâmetro inválido.", token->getPosition());
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
                        } else {
                            while (!declLiterals.empty()) {
                                declLiterals.pop();
                            }
                        }
                    }
                }

                if (ids.empty()) {
                    throw SemanticError("Parâmetro inválido.", token->getPosition());
                }
                std::string paramId = ids.top();
                ids.pop();

                if (!stManager.insertSymbol(paramId, pmt, currentScope)) {
                    throw SemanticError("Parâmetro já declarado no escopo!", token->getPosition());
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
                int groups = std::min(initCount, idCount);
                if (initCount > idCount) {
                    throw SemanticError("Quantidade de inicializadores de vetor maior que quantidade de IDs", token->getPosition());
                }

                for (int i = 0; i < groups; i++) {
                    if (static_cast<int>(declLiterals.size()) < mt.arrSize) {
                        throw SemanticError("Inicialização do vetor incompleta!", token->getPosition());
                    }

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
                    }
                }
                if (!declLiterals.empty()) {
                    throw SemanticError("Inicialização do vetor excede o tamanho!", token->getPosition());
                }

                for (int i = 0; i < idCount; i++) {
                    MetaData vmt = mt;
                    if (isInitialized && i < initCount) {
                        vmt.isInitialized = true;
                    }

                    if(!stManager.insertSymbol(idsOrdered[static_cast<std::size_t>(i)], vmt, currentScope)) {
                        throw SemanticError("Variável já declarada no escopo!", token->getPosition());
                    }
                }
            } else {
                if (initCount > idCount) {
                    throw SemanticError("Quantidade de inicializadores maior que quantidade de IDs", token->getPosition());
                }

                for (int i = 0; i < idCount; i++) {
                    MetaData vmt = mt;
                    if (isInitialized && i < initCount) {
                        vmt.isInitialized = true;
                        if (!declLiterals.empty()) {
                            const auto &currLiteral = declLiterals.top();
                            CompatibilityResult comp = checkAssignmentCompatibility(vmt.dataType, currLiteral.first);
                            declLiterals.pop();
                            if (comp == CompatibilityResult::Error) {
                                throw SemanticError("O tipo não é compatível.", token->getPosition());
                            }
                            if (comp == CompatibilityResult::Warn) {
                                logWarning("Compatibilidade de tipos com aviso.", token);
                            }
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
                MetaData *existing = stManager.returnMetaData(currId, targetScope);
                if (existing->varType == VariableTypes::FUNCTION && !existing->isInitialized && mt.isInitialized) {
                    existing->isInitialized = true;
                    if (!stManager.updateSymbol(currId, *existing, targetScope)) {
                        throw SemanticError("Erro ao atualizar a Função.", token->getPosition());
                    }
                } else {
                    throw SemanticError("Função já declarada no escopo!", token->getPosition());
                }
            } else {
                throw SemanticError("Variável já declarada no escopo!", token->getPosition());
            }
        }

        currentReturnType = mt.dataType;

        if (mt.varType == VariableTypes::FUNCTION) {
            currentFunctionName.clear();
        }
        break;
    }

    case TAMANHO_VETOR: {
        if (literals.empty()) {
            throw SemanticError("Tamanho do vetor inválido!", token->getPosition());
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
            throw SemanticError("Tamanho do vetor inválido!", token->getPosition());
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
        skipCodeBlock = true;
        break;

    case SAI_ESCOPO_PARAMETROS:
        if (currentScope == nullptr){
            currentScope = stManager.getRootScope();
        }
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

    case AVALIA_EXPRESSAO:{
        auto right = literals.top();
        literals.pop();

        auto left = literals.top();
        literals.pop();

        Operators op = operators.top();
        operators.pop();

        auto leftType = left.first;
        auto rightType = right.first;

        switch(op){

            case Operators::SUM:{

                // string + anything
                if(leftType == DataTypes::STRING ||
                rightType == DataTypes::STRING){

                    literals.push({DataTypes::STRING, ""});
                    break;
                }

                // char + string
                if(leftType == DataTypes::CHAR &&
                rightType == DataTypes::STRING){

                    literals.push({DataTypes::STRING, ""});
                    break;
                }

                // int + int
                if(leftType == DataTypes::INT &&
                rightType == DataTypes::INT){

                    literals.push({DataTypes::INT, ""});
                    break;
                }

                // float combinations
                if((leftType == DataTypes::FLOAT &&
                    rightType == DataTypes::FLOAT) ||

                (leftType == DataTypes::FLOAT &&
                    rightType == DataTypes::INT) ||

                (leftType == DataTypes::INT &&
                    rightType == DataTypes::FLOAT)){

                    if(leftType == DataTypes::INT &&
                    rightType == DataTypes::FLOAT){
                        logWarning("Perda de precisão.", token);
                    }

                    literals.push({DataTypes::FLOAT, ""});
                    break;
                }

                // char + char
                if(leftType == DataTypes::CHAR &&
                rightType == DataTypes::CHAR){
                    literals.push({DataTypes::CHAR, ""});
                    break;
                }

                // char + int
                if(leftType == DataTypes::CHAR &&
                rightType == DataTypes::INT){
                    logWarning("Considerando apenas o primeiro dígito", token);
                    literals.push({DataTypes::CHAR, ""});
                    break;
                }

                throw SemanticError("Tipos incompatíveis para SUM");
            }

            case Operators::SUB:
            case Operators::MUL:
            case Operators::DIV:{

                // string invalid
                if(leftType == DataTypes::STRING ||
                rightType == DataTypes::STRING){

                    throw SemanticError("Tipo STRING inválido nessa operação");
                }

                // char invalid with float
                if((leftType == DataTypes::CHAR &&
                    rightType == DataTypes::FLOAT) ||

                (leftType == DataTypes::FLOAT &&
                    rightType == DataTypes::CHAR)){

                    throw SemanticError("CHAR incompatível com FLOAT");
                }

                // char invalid with int for these ops
                if((leftType == DataTypes::CHAR &&
                    rightType == DataTypes::INT) ||

                (leftType == DataTypes::INT &&
                    rightType == DataTypes::CHAR)){

                    throw SemanticError("CHAR incompatível com INT");
                }

                // char char
                if(leftType == DataTypes::CHAR &&
                rightType == DataTypes::CHAR){
                    literals.push({DataTypes::CHAR, ""});
                    break;
                }

                // int int
                if(leftType == DataTypes::INT &&
                rightType == DataTypes::INT){
                    literals.push({DataTypes::INT, ""});
                    break;
                }

                // float combinations
                if((leftType == DataTypes::FLOAT &&
                    rightType == DataTypes::FLOAT) ||
                (leftType == DataTypes::FLOAT &&
                    rightType == DataTypes::INT) ||
                (leftType == DataTypes::INT &&
                    rightType == DataTypes::FLOAT)){
                    literals.push({DataTypes::FLOAT, ""});
                    break;
                }

                throw SemanticError("Tipos incompatíveis");
            }

            case Operators::AND:
            case Operators::OR:{

                if(leftType != DataTypes::BOOLEAN ||
                rightType != DataTypes::BOOLEAN){

                    throw SemanticError("AND/OR requer BOOLEAN");
                }

                literals.push({DataTypes::BOOLEAN, ""});
                break;
            }

            case Operators::NOT:{

                if(rightType != DataTypes::BOOLEAN){
                    throw SemanticError("NOT requer BOOLEAN");
                }

                literals.push({DataTypes::BOOLEAN, ""});
                break;
            }
        }

        break;
    }

    case GUARDA_OPERADOR:
        operators.push(operatorFromToken(token->getId()));
        break;

    case AVALIA_RETORNO:{
        auto returnType = literals.top();
        literals.pop();
        auto 
        break;
    }

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

