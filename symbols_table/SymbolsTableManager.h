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
    int arrSize = 0;
    std::string ownerFunction;
    bool isUsed = false;
    bool isInitialized = false;
    int sequence = 0;

    bool operator==(const MetaData& other) const {
        return isInitialized == other.isInitialized;
    }
};

struct SymbolRow {
    std::string symbol;
    VariableTypes varType;
    DataTypes dataType;
    int arrSize = 0;
    std::string ownerFunction;
    bool isUsed = false;
    bool isInitialized = false;
    int sequence = 0;
    int scope;
};

struct Scope {
    std::unordered_map<std::string, MetaData> symbols;
    Scope *previous_scope;
    int id = 0;
    std::vector<std::unique_ptr<Scope>> inner_scopes;
};

class SymbolsTableManager {
private:
    std::unique_ptr<Scope> scopes;
    int scopeIdCounter = 0;
    MetaData *findSymbol(std::string symbolName, Scope *currScope);
public:
    SymbolsTableManager();
    ~SymbolsTableManager();
    void reset();
    std::vector<SymbolRow> collectSymbolsPreorder() const;

    Scope *enterScope(Scope *currScope);
    Scope *exitScope(Scope *currScope);
    Scope *getRootScope();
    bool returnScopes();
    bool validateVariableScope(std::string symbolName, Scope *currScope);

    bool insertSymbol(std::string symbolName, MetaData metaData, Scope *scope);
    bool updateSymbol(std::string symbolName, const MetaData &metaData, Scope *scope);
    bool deleteSymbol(std::string symbolName, Scope *scope);
    bool useSymbol(std::string symbolName, Scope *scope);
    bool initializeSymbol(std::string symbolName, Scope *scope);
    MetaData *returnMetaData(std::string symbolName, Scope *currScope);

};

#endif