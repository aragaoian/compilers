#ifndef SYMBOLSTABLE_H    
#define SYMBOLSTABLE_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

enum class VariableTypes { SCALAR, ARRAY, FUNCTION, PARAMETER, ARGUMENT };
enum class DataTypes { INT, FLOAT, BOOLEAN, CHAR, STRING, VOID };

struct MetaData {
    VariableTypes varType;
    DataTypes dataType;
    std::string value;
    bool isUsed = false;
    bool isInitialized = false;
    int sequence = 0;
};

struct SymbolRow {
    std::string symbol;
    VariableTypes varType;
    DataTypes dataType;
    std::string value;
    bool isUsed = false;
    bool isInitialized = false;
    int sequence = 0;
    int scope;
};

struct Scope {
    std::unordered_map<std::string, MetaData> symbols;
    Scope *previous_scope;
    std::vector<std::unique_ptr<Scope>> inner_scopes;
};

class SymbolsTableManager {
private:
    std::unique_ptr<Scope> scopes;
public:
    SymbolsTableManager();
    ~SymbolsTableManager();
    void reset();

    Scope *enterScope(Scope *currScope);
    Scope *exitScope(Scope *currScope);
    bool returnScopes();

    Scope *getRootScope();
    std::vector<SymbolRow> collectSymbolsPreorder() const;
    bool insertSymbol(std::string symbolName, MetaData metaData, Scope *scope);
    bool deleteSymbol(std::string symbolName, Scope *scope);
    bool useSymbol(std::string symbolName, Scope *scope);
    bool initializeSymbol(std::string symbolName, Scope *scope);
    MetaData returnMetaData(std::string symbolName, Scope *scope);

};

#endif