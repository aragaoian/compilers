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
    bool isUsed = false;
    bool isInitialized = false;
    int sequence = 0;
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

    bool clearTable();
    Scope *enterScope(std::unique_ptr<Scope> &currScope);
    Scope *exitScope(Scope *currScope);
    bool returnScopes();

    bool insertSymbol(std::string symbolName, MetaData metaData, std::unique_ptr<Scope> &scope);
    bool deleteSymbol(std::string symbolName, std::unique_ptr<Scope> &scope);
    bool useSymbol(std::string symbolName, std::unique_ptr<Scope> &scope);
    bool initializeSymbol(std::string symbolName, std::unique_ptr<Scope> &scope);
    MetaData returnMetaData(std::string symbolName, std::unique_ptr<Scope> &scope);

};

#endif