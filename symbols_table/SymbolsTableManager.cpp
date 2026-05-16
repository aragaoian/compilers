#include "SymbolsTableManager.h"
#include <iostream>
#include <stdexcept>
#include <memory>

SymbolsTableManager::SymbolsTableManager() {
    scopes = std::make_unique<Scope>();
    scopes->previous_scope = nullptr;
    scopes->id = scopeIdCounter++;
}

SymbolsTableManager::~SymbolsTableManager(){
    scopes = nullptr;
}

void SymbolsTableManager::reset(){
    scopeIdCounter = 0;
    scopes = std::make_unique<Scope>();
    scopes->previous_scope = nullptr;
    scopes->id = scopeIdCounter++;
}

Scope *SymbolsTableManager::enterScope(Scope *currScope){
    if(currScope == nullptr){
        return nullptr;
    }
    auto newScope = std::make_unique<Scope>();
    newScope->previous_scope = currScope;
    newScope->id = scopeIdCounter++;
    try{
        // NOTE
        // usar o std::move por causa do unique_ptr
        // (que não pode ser copiado).
        // Quando usado o std::move, a ownership do newScope
        // passa a ser do vector.
        currScope->inner_scopes.push_back(std::move(newScope));
        return currScope->inner_scopes.back().get();
    }catch(const std::runtime_error& e){
        std::cerr << "Erro ao inserir escopo: " << e.what() << std::endl;
        return nullptr;
    }
}

Scope *SymbolsTableManager::exitScope(Scope *currScope){
    if(currScope == nullptr || currScope->previous_scope == nullptr){
        return nullptr;
    }

    try{
        currScope = currScope->previous_scope;
        return currScope;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao retornar ao escopo anterior: " << e.what() << std::endl;
        return nullptr;
    }
}

bool SymbolsTableManager::validateVariableScope(std::string symbolName, Scope* currScope) {
    if (currScope == nullptr)
        return false;

    auto it = currScope->symbols.find(symbolName);
    if (it != currScope->symbols.end())
        return true;

    return validateVariableScope(symbolName, currScope->previous_scope);
}

MetaData *SymbolsTableManager::findSymbol(std::string symbolName, Scope *currScope){
    if(currScope == nullptr) return nullptr;

    auto it = currScope->symbols.find(symbolName);
    if(it != currScope->symbols.end()){
        return &it->second;
    }

    return findSymbol(symbolName, currScope->previous_scope);
}

Scope *SymbolsTableManager::getRootScope(){
    return scopes.get();
}

static void collectScopeSymbolsPreorder(const Scope *scope, std::vector<SymbolRow> &rows)
{
    if(scope == nullptr){
        return;
    }

    for(const auto &entry : scope->symbols)
    {
        const MetaData &meta = entry.second;
        rows.push_back({
            entry.first,
            meta.varType,
            meta.dataType,
            meta.arrSize,
            meta.ownerFunction,
            meta.isUsed,
            meta.isInitialized,
            meta.sequence,
            scope->id
        });
    }

    for(const auto &child : scope->inner_scopes)
    {
        collectScopeSymbolsPreorder(child.get(), rows);
    }
}

std::vector<SymbolRow> SymbolsTableManager::collectSymbolsPreorder() const
{
    std::vector<SymbolRow> rows;
    collectScopeSymbolsPreorder(scopes.get(), rows);
    return rows;
}

bool SymbolsTableManager::insertSymbol(std::string symbolName, MetaData metaData, Scope *scope){
    if(scope == nullptr){
        return false;
    }
    try{
        auto it = scope->symbols.find(symbolName);
        if(it != scope->symbols.end()){
            return false;
        }else{
            scope->symbols[symbolName] = metaData;
            return true;
        }
        
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao inserir o símbolo: " << e.what() << std::endl;
        return false;
    }
}

bool SymbolsTableManager::updateSymbol(std::string symbolName, const MetaData &metaData, Scope *scope){
    if(scope == nullptr){
        return false;
    }
    try{
        auto it = scope->symbols.find(symbolName);
        if(it == scope->symbols.end()){
            return false;
        }
        it->second = metaData;
        return true;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao atualizar o simbolo: " << e.what() << std::endl;
        return false;
    }
}

bool SymbolsTableManager::deleteSymbol(std::string symbolName, Scope *scope){
    if(scope == nullptr){
        return false;
    }
    try{
        scope->symbols.erase(symbolName);
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao deletar o símbolo: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool SymbolsTableManager::useSymbol(std::string symbolName, Scope *scope){
    if(scope == nullptr){
        return false;
    }

    try{
        MetaData *meta = findSymbol(symbolName, scope);
        if(meta == nullptr) return false;
        meta->isUsed = true;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao colocar o símbolo como usado: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool SymbolsTableManager::initializeSymbol(std::string symbolName, Scope *scope){
    if(scope == nullptr){
        return false;
    }
    try{
        MetaData *meta = findSymbol(symbolName, scope);
        if(meta == nullptr) return false;
        meta->isInitialized = true;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao colocar o símbolo como inicializado: " << e.what() << std::endl;
        return false;
    }
    return true;
}

MetaData *SymbolsTableManager::returnMetaData(std::string symbolName, Scope *currScope){
    MetaData *meta = findSymbol(symbolName, currScope);
    if(meta == nullptr) return nullptr;
    return meta;
}