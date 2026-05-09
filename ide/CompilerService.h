#ifndef COMPILER_SERVICE_H
#define COMPILER_SERVICE_H

#include "../symbols_table/SymbolsTableManager.h"

#include <string>
#include <vector>

struct CompilationResult
{
    bool success;
    std::string message;
    std::vector<SymbolRow> symbols;
};

class CompilerService
{
public:
    CompilationResult compile(const std::string &sourceCode) const;

private:
    static std::string normalizeSource(const std::string &sourceCode);
    static std::string formatPosition(const std::string &sourceCode, int position);
};

#endif
