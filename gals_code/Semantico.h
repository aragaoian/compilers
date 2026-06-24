#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "../enums/Operators.h"
#include "../code_generator/CodeGenator.h"
#include "../symbols_table/SymbolsTableManager.h"
#include "SemanticError.h"
#include "Token.h"
#include <stack>
#include <string>
#include <utility>
#include <vector>

enum class ValueKind { LITERAL, VARIABLE, EXPRESSION, ACCUMULATOR, VECTOR_ACCESS };

enum class IoContext { NONE, INPUT_COMMAND, OUTPUT_COMMAND };

struct SemanticValue {
    DataTypes dataType;
    std::string lexeme;
    ValueKind kind = ValueKind::LITERAL;
};

class Semantico {
  public:
    Semantico();
    void executeAction(int action, const Token *token);
    std::vector<SymbolRow> getSymbolRows() const;
    std::string getGeneratedCode();
    void logDeclaredButNotUsed();
    void logNonInitializedVariables();
    std::vector<std::string> getMessages() const;
    void reset();

  private:
    void logInfo(const std::string &message, const Token *token);
    void logWarning(const std::string &message, const Token *token);
    int parseCodegenInteger(const SemanticValue &value, const Token *token) const;
    void emitLoadValue(const SemanticValue &value, const Token *token);
    void emitApplyOperator(Operators op, const SemanticValue &value, const Token *token);
    void emitStoreInitializedValue(const std::string &id, const SemanticValue &value,
                                   const Token *token);
    void materializeAccumulator(SemanticValue &value);
    void freeExpressionTemp(const SemanticValue &value);
    std::vector<std::string> messages;

    SymbolsTableManager stManager;
    Scope *currentScope = nullptr;
    CodeGenerator codeGenerator;

    std::stack<std::string> ids;
    std::stack<VariableTypes> variableTypes;
    std::stack<DataTypes> dataTypes;
    std::stack<SemanticValue> literals;
    std::stack<SemanticValue> declLiterals;

    std::vector<SemanticValue> inputSemanticValues;
    std::vector<SemanticValue> outputSemanticValues;
    IoContext currentIoContext = IoContext::NONE;
    bool hasPendingVectorAssignment = false;
    std::string pendingVectorAssignmentIndexTemp;

    std::stack<int> arrSizes;
    int pendingArraySize = 1;
    bool hasPendingArraySize = false;

    bool isAttribution = false;
    bool isInitialized = false;
    bool isUsed = false;
    bool inInitContext = false;

    bool skipCodeBlock = false;

    int pendingIdsCount = 0;
    int pendingInitCount = 0;
    std::vector<std::string> pendingIds;

    int currFuncParamatersCounter = 0;
    std::string currentFunctionName;
    DataTypes currentReturnType = DataTypes::VOID;
    std::vector<SemanticValue> pendingArgs;

    std::stack<Operators> operators;
    std::stack<Operators> conditionsOperator;
    
    int conditionalCounter = 0;
    std::stack<int> ifController;
    std::stack<bool> elseController;

    int loopCounter = 0;
    std::stack<int> loopController;
};

#endif
