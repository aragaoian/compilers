#ifndef SYMBOLSTABLE_H    
#define SYMBOLSTABLE_H

#include <unordered_map>
#include <vector>
#include <stack>
#include <string>

enum class VariableTypes { VARIABLE, ARRAY, FUNCTION, PARAMETER, ARGUMENT};
enum class DataTypes { INT, FLOAT, BOOLEAN, CHAR, STRING};

struct Symbol {
    std::string var_name;
    VariableTypes var_type;
    DataTypes data_type;
    bool isUsed = false;
    bool isInitialized = false;
    int sequence;
};

class SymbolsTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
public:
    SymbolsTable();
    ~SymbolsTable();

    bool clearTable();
    std::vector<std::unordered_map<std::string, Symbol>> returnScopes();

    bool insertSymbol(Symbol symbol);
    bool deleteSymbol(std::string var_name, int scope);
    bool updateSymbol(std::string, int scope);

    bool enterScope();
    bool exitScope();
};

#endif