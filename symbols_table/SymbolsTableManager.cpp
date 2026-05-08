#include "SymbolsTableManager.h"
#include <iostream>
#include <stdexcept>
#include <memory>

SymbolsTableManager::SymbolsTableManager() {
    scopes = std::make_unique<Scope>();
    scopes->previous_scope = nullptr;
}

SymbolsTableManager::~SymbolsTableManager(){
    scopes = nullptr;
}

Scope *SymbolsTableManager::enterScope(std::unique_ptr<Scope> &currScope){
    auto newScope = std::make_unique<Scope>();
    newScope->previous_scope = currScope.get();
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

bool SymbolsTableManager::insertSymbol(std::string symbolName, MetaData metaData, std::unique_ptr<Scope> &scope){
    try{
        scope->symbols[symbolName] = metaData;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao inserir o símbolo: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool SymbolsTableManager::deleteSymbol(std::string symbolName, std::unique_ptr<Scope> &scope){
    try{
        scope->symbols.erase(symbolName);
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao deletar o símbolo: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool SymbolsTableManager::useSymbol(std::string symbolName, std::unique_ptr<Scope> &scope){
    try{
        auto it = scope->symbols.find(symbolName);
        if(it == scope->symbols.end()) return false;
        scope->symbols[symbolName].isUsed = true;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao colocar o símbolo como usado: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool SymbolsTableManager::initializeSymbol(std::string symbolName, std::unique_ptr<Scope> &scope){
    try{
        auto it = scope->symbols.find(symbolName);
        if(it == scope->symbols.end()) return false;
        scope->symbols[symbolName].isInitialized = true;
    }catch(const std::runtime_error &e){
        std::cerr << "Erro ao colocar o símbolo como inicializado: " << e.what() << std::endl;
        return false;
    }
    return true;
}