#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "../code_generator/CodeGenator.h"
#include "../enums/Operators.h"
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
    void attributionSumPipeline(std::stack<SemanticValue> &expressionValues, SemanticValue left,
                                SemanticValue right, Operators op, const Token *token);
    void consumeBooleanCondition(const Token *token);
    void emitCompoundAssignment(const std::string &id, const MetaData *metadata,
                                SemanticValue value, const Token *token);
    std::string functionLabel(const std::string &functionName, int sequence) const;
    std::string parameterLabel(const std::string &paramName, const MetaData &metadata) const;
    std::string variableLabel(const std::string &varName, const MetaData &metadata) const;
    std::string variableLabelForCurrentContext(const std::string &varName) const;
    std::string returnLabel(const std::string &functionName, int sequence) const;
    std::string storageLabelFor(const std::string &id, const Token *token);
    bool isPendingDeclarationId(const std::string &id) const;
    std::vector<SymbolRow> parametersForFunction(const std::string &functionName) const;
    void finishFunctionCall(bool pushReturnValue, const Token *token);
    void validateMainFunction() const;

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
    bool hasPendingCompoundAssignment = false;
    Operators pendingAssignmentOperator = Operators::SUM;

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
    int currentFunctionSequence = -1;
    int functionCounter = 1;
    bool hasMainFunction = false;
    bool currentFunctionHasReturn = false;
    DataTypes currentReturnType = DataTypes::VOID;
    std::vector<SemanticValue> pendingArgs;
    std::string pendingCallName;
    bool isReadingArgs = false;

    std::stack<Operators> operators;
    std::stack<Operators> conditionsOperator;

    int conditionalCounter = 0;
    std::stack<int> ifController;
    std::stack<bool> elseController;

    int loopCounter = 0;
    std::stack<int> loopController;
    std::stack<bool> forScopeController;
};

#endif
